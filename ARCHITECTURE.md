# ESP32 E-Paper Visual Timer — Architecture

## 1. Purpose & scope

A battery-friendly physical **visual timer**: the user sets a duration with a
rotary encoder, presses to start, and watches a shrinking arc/bar on a 4.2"
e-paper panel — the embedded sibling of the *visualtimer* Android app. The
firmware is structured so that the future connectivity story (BLE companion app,
Focusmate API, Google Calendar) and battery hardware drop in without rewrites.

Design goals, in priority order:

1. **Correctness of the timer core**, verifiable without hardware.
2. **Testability** — domain logic is pure C++ and unit-tested on the host.
3. **Replaceability** — every piece of hardware sits behind an interface.
4. **Power frugality** — e-paper holds its image at zero power; the device sleeps.

## 2. Layered architecture (SOLID)

Dependencies point **downward only**. Upper layers depend on **interfaces**
(HAL), never on concrete drivers. `src/main.cpp` is the **composition root** —
the single place that names concrete types and wires the object graph. This is
what makes Dependency Inversion real rather than aspirational.

```
Application : main.cpp / App / TimerStateMachine / UiController     (orchestration)
Domain/Svc  : Timer, Duration, RenderModel, HybridRefreshPolicy ... (pure C++, no Arduino)
HAL         : IDisplay IClock IRotaryInput IButtonInput IIndicator
              ISettingsStore IBleService IPower                      (abstractions)
Drivers     : GxEpdDisplay Ds3231Clock Esp32RotaryInput OneButtonInput
              GpioIndicator NvsSettingsStore NimBleService EspPowerManager
Platform    : Arduino-ESP32 + FreeRTOS + vendor libraries
```

Mapping to SOLID:

- **S**RP — domain holds *what* the timer does; drivers hold *how* hardware does it.
- **O**CP — add a timer mode by adding a state class; add hardware by adding a driver.
- **L**SP — fakes in `test/fakes/` substitute for real drivers in tests.
- **I**SP — small role interfaces (`IDisplay`, `IClock`, …) instead of one god-object.
- **D**IP — application/domain depend on HAL interfaces; concretes injected at the root.

### HAL interfaces (`include/hal/`)

| Interface | Responsibility | Concrete driver |
|---|---|---|
| `IDisplay` | `clear / drawFull / drawPartial / hibernate` | `GxEpdDisplay` |
| `IClock` | `now / setTime / setAlarm / clearAlarm / temperatureC` | `Ds3231Clock` |
| `IRotaryInput` | `poll → delta`, `takePress` | `Esp32RotaryInput` |
| `IButtonInput` | `poll`, `takeEvent → ButtonEvent` | `OneButtonInput` |
| `IIndicator` | `set(IndicatorState)` | `GpioIndicator` |
| `ISettingsStore` | `load / save` (Repository) | `NvsSettingsStore` |
| `IBleService` | `begin / notifyState / setCommandSink` | `NimBleService` |
| `IPower` | `enterDeepSleep / wakeReason` | `EspPowerManager` |

Reserved future seam: `IBattery` (LiPo / BMS / fuel gauge).

### Domain (`include/domain/`, `src/domain/`)

Zero Arduino includes, so it compiles and runs on the host:

- `Duration` — seconds value object, arithmetic, `mm:ss` / `hh:mm` formatting, clamp.
- `Timer` — countdown / count-up / pomodoro tick logic.
- `TimerSnapshot` — immutable view consumed by rendering and BLE.
- `SettingsModel` — defaults + schema version (forward-compatible persistence).
- `RenderModel` + `DirtyRegion` — what to draw and what changed.
- `InputCommand` + `ICommandSink` — uniform commands from any input source.
- `IRefreshPolicy` — the full-vs-partial e-paper decision.

### Application (`include/app/`, `src/app/`)

`TimerStateMachine` (owns `Timer`, drives state objects), `UiController` (maps
commands to state actions and builds the `RenderModel`, diffing for the dirty
region), and `App` (lifecycle / loop step). The composition root lives in `main.cpp`.

## 3. Design patterns

| Pattern | Where | Why |
|---|---|---|
| **State** | `TimerStateMachine`: Idle / Setting / Running / Paused / Finished | Behavior is mode-dependent; each state owns transitions, redraw cadence, wake policy. |
| **Strategy** | `IRefreshPolicy` → `HybridRefreshPolicy` | Isolates the tricky e-paper refresh decision so it is tunable and unit-testable. |
| **Command** | `InputCommand` + `ICommandSink` | Encoder, buttons, and BLE all emit the same commands → BLE control == physical control. |
| **Observer / queue** | FreeRTOS queues input → UI → render/BLE | Decouples fast producers from slow (blocking) consumers. |
| **Repository** | `ISettingsStore` over NVS | Domain stays storage-agnostic; swapped for an in-memory fake in tests. |
| **Facade** | `GxEpdDisplay` over GxEPD2 + Adafruit_GFX | Hides the paging loop, init quirks, and BUSY handling behind four methods. |
| **Adapter** | `Esp32RotaryInput` over ESP32Encoder/PCNT | Swap the encoder library without touching application code. |
| **Dependency Injection** | composition root in `main.cpp` | One wiring point; everything above HAL is unit-testable. |

## 4. E-paper rendering strategy (the hard part)

Full refresh on this 4.2" panel takes ≈1.5–2 s with visible black/white
flashing; partial refresh is ≈0.3–0.5 s with no flash but accumulates
**ghosting**. The strategy is **hybrid partial + periodic full + dirty regions**:

1. **Cadence is granularity-aware, not per-second.** The timer reads as a
   shrinking arc/bar. With more than a minute remaining, redraw at most once per
   minute; in the final minute, redraw once per second. Most of a session
   redraws rarely.
2. **Dirty-region partial refresh.** `UiController` diffs the new `RenderModel`
   against the last and emits a `DirtyRegion` bounding box; the display facade
   issues a partial-window refresh over only that box. **X and width are snapped
   to multiples of 8** (an SSD1683 partial-window constraint).
3. **Ghosting control.** `HybridRefreshPolicy` counts partial refreshes and
   forces a full refresh after N (≈10–20) or after M minutes, then resets the
   counter. Every state entry and the Finished frame use a full refresh so each
   screen starts clean; the Finished frame persists at zero power.
4. **No-op skipping.** When nothing changed, the policy returns `None` and no
   draw happens — saving power and panel wear.

```cpp
struct RefreshDecision { enum class Kind { None, Partial, Full }; Kind kind; DirtyRegion region; };
class IRefreshPolicy {
public:
    virtual ~IRefreshPolicy() = default;
    virtual RefreshDecision decide(const RenderModel& next, const RenderModel& last) = 0;
};
```

`HybridRefreshPolicy` is pure logic and is fully unit-tested.

## 5. Concurrency model

Arduino-ESP32 already runs FreeRTOS, so the design uses a small, fixed set of
tasks and queues rather than a naive super-loop or a task-per-everything:

- **UiTask** (core 1) — consumes `inputQueue` (encoder / button / BLE / tick),
  steps the state machine, pushes to `renderQueue` and `bleQueue`.
- **RenderTask** (core 1) — consumes `renderQueue` and performs the long,
  blocking e-paper draws, so input and the 1 Hz tick never stall.
- **TimerTask** (1 Hz via `vTaskDelayUntil`) — emits tick commands; absolute time
  is anchored to the DS3231 (read on boot/wake) so it survives deep sleep.
- **BLE** runs on NimBLE's own task (core 0); app tasks are pinned to core 1.

ISR safety: ESP32Encoder owns its PCNT ISR (the app only `poll()`s a delta);
OneButton is `tick()`-polled. Only BLE callbacks may need `xQueueSendFromISR`.

> The skeleton ships a minimal `App::loop()` step plus the real queue/task shape
> documented above, kept simple enough to compile and reason about.

## 6. Power management

The e-paper holds its image at **zero power** and the DS3231 keeps accurate time
on a coin cell drawing microamps — so the device can deep-sleep showing the
timer/clock and wake only on user input or a scheduled alarm.

- Enter `esp_deep_sleep_start()` when Idle past a timeout, **after** a final full
  draw and `display.hibernate()`.
- **Wake sources:** `ext0` on the DS3231 SQW/INT pin (alarm; active-low,
  open-drain → needs a pull-up; wake on level 0) and `ext1` on a mask of
  button/encoder-push pins (RTC-capable GPIOs only).
- `IPower::wakeReason()` maps `esp_sleep_get_wakeup_cause()` to a domain enum so
  the state machine decides what to show on wake.

LiPo/BMS and fuel-gauge logic are deferred behind a future `IBattery` seam; an
ADC1 pin is reserved in `Pins.h` for battery sensing (ADC2 is unusable with WiFi on).

## 7. Connectivity (future, abstracted now)

The companion app, Focusmate, and Google Calendar all reduce to two needs:
**receive commands** (start/stop/set) and **publish state**. Both are expressed
today through `IBleService` + `ICommandSink`, so:

- A BLE write characteristic feeds `InputCommand`s into the same queue as the
  encoder — physical and remote control share one path.
- A BLE notify characteristic publishes `TimerSnapshot` to subscribers.
- A future WiFi/cloud sync service implements the same `ICommandSink` /
  publish contract; nothing in the domain or UI changes.

The skeleton includes a NimBLE GATT stub (one service: a notify "state"
characteristic and a writable "command" characteristic).

## 8. Testing

Two PlatformIO environments; **Unity** for native tests:

- `env:native` — pure domain only (`build_src_filter = +<domain/>`); runs in CI
  without hardware.
- `env:esp32dev` — `pio run` is the compile-green gate.

Native coverage: `Duration` math/format/clamp; `Timer` tick/pause/finish and
count-up vs countdown; `TimerStateMachine` legal and rejected transitions;
`HybridRefreshPolicy.decide()` (partial/full counter, dirty bbox, no-change →
None); `SettingsModel` versioning via `FakeSettingsStore`. In-memory fakes in
`test/fakes/` implement the HAL interfaces (e.g. `FakeClock` with advanceable
virtual time, `FakeDisplay` recording draw calls and regions).

## 9. Hardware notes & gotchas (encoded in `Pins.h`)

- **Avoid GPIO0/2/12/15 for buttons** — boot strapping pins; the wrong level at
  reset prevents boot.
- **GPIO34–39 are input-only with no internal pull-ups** — add external pull-ups.
- **Deep-sleep wake needs RTC-capable GPIOs** (0,2,4,12–15,25–27,32–39).
- **ADC2 is unusable while WiFi is on** — reserve an ADC1 pin (32–39) for battery sense.
- **I2C:** 4.7 kΩ pull-ups on SDA/SCL (the DS3231 module usually includes them);
  the SQW/INT pin is open-drain and needs a pull-up for clean EXT0 wake.
- **Power integrity:** 100 nF decoupling at each IC plus a 10–100 µF bulk cap on
  the 3V3 rail to avoid brownout resets on radio TX spikes / weak USB cables.
- **Inputs:** encoder 100 nF A→GND / B→GND with ~10 kΩ pull-ups; 100 nF across
  each button.
- **Status LED:** series 220–470 Ω resistor. The **SN74AHCT125N** is a 5V-powered
  *unidirectional* buffer (tie the active channel's OE low) used to level-shift a
  3.3 V GPIO up to a 5 V WS2812 data line (~330 Ω in series) — do not feed 5 V back
  into a 3.3 V GPIO.
- **E-paper:** dedicated CS/DC/RST/BUSY GPIOs; pass BUSY to the GxEPD2 constructor;
  `hibernate()` after each screen.
- **Possible BOM gaps:** external pull-ups for 34–39 inputs, a 3V3 bulk cap, series
  resistors (LED + WS2812 line), and a future LiPo charger/BMS (e.g. TP4056 + protection).
