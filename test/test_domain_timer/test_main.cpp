// Native Unity tests for the timer domain + state machine.
#include <unity.h>

#include "app/TimerStateMachine.h"
#include "domain/Duration.h"
#include "domain/SettingsModel.h"
#include "domain/Timer.h"
#include "fakes/FakeSettingsStore.h"

using namespace domain;

void setUp() {}
void tearDown() {}

// ----------------------------------------------------------- Duration ------
static void test_duration_clamps_negative_to_zero() {
    TEST_ASSERT_EQUAL_INT32(0, Duration(-50).seconds());
}

static void test_duration_saturating_subtraction() {
    const Duration d = Duration(30) - Duration(100);
    TEST_ASSERT_EQUAL_INT32(0, d.seconds());
}

static void test_duration_parts() {
    const Duration d = Duration::fromSeconds(3725); // 1h 02m 05s
    TEST_ASSERT_EQUAL_INT32(1, d.hoursPart());
    TEST_ASSERT_EQUAL_INT32(2, d.minutesPart());
    TEST_ASSERT_EQUAL_INT32(5, d.secondsPart());
}

static void test_duration_format_mmss_and_hms() {
    char buf[16];
    TEST_ASSERT_EQUAL_STRING("05:09", Duration(309).format(buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("1:02:05", Duration(3725).format(buf, sizeof(buf)));
}

static void test_duration_clamped_range() {
    const Duration lo = Duration(60);
    const Duration hi = Duration(600);
    TEST_ASSERT_EQUAL_INT32(60, Duration(10).clamped(lo, hi).seconds());
    TEST_ASSERT_EQUAL_INT32(600, Duration(9999).clamped(lo, hi).seconds());
    TEST_ASSERT_EQUAL_INT32(300, Duration(300).clamped(lo, hi).seconds());
}

// -------------------------------------------------------------- Timer ------
static void test_timer_countdown_ticks_to_finish() {
    Timer t(Duration(3), TimerMode::Countdown);
    TEST_ASSERT_FALSE(t.isFinished());
    t.tick();
    t.tick();
    TEST_ASSERT_EQUAL_INT32(1, t.remaining().seconds());
    t.tick();
    TEST_ASSERT_TRUE(t.isFinished());
    TEST_ASSERT_EQUAL_INT32(0, t.remaining().seconds());
    // No underflow past finish.
    t.tick();
    TEST_ASSERT_EQUAL_INT32(0, t.remaining().seconds());
}

static void test_timer_fraction_remaining() {
    Timer t(Duration(100), TimerMode::Countdown);
    TEST_ASSERT_FLOAT_WITHIN(0.001F, 1.0F, t.fractionRemaining());
    for (int i = 0; i < 25; ++i) t.tick();
    TEST_ASSERT_FLOAT_WITHIN(0.001F, 0.75F, t.fractionRemaining());
}

static void test_timer_countup_displays_elapsed() {
    Timer t(Duration(10), TimerMode::CountUp);
    t.tick();
    t.tick();
    TEST_ASSERT_EQUAL_INT32(2, t.displayValue().seconds());
}

static void test_timer_adjust_total_clamped() {
    Timer t(Duration(120), TimerMode::Countdown);
    t.adjustTotal(-9999, Duration(60), Duration(3600));
    TEST_ASSERT_EQUAL_INT32(60, t.total().seconds());
    t.adjustTotal(+9999, Duration(60), Duration(3600));
    TEST_ASSERT_EQUAL_INT32(3600, t.total().seconds());
}

// ------------------------------------------------- TimerStateMachine -------
static app::TimerStateMachine makeSm() {
    SettingsModel s;
    s.defaultDuration = Duration(3);
    s.mode = TimerMode::Countdown;
    app::TimerStateMachine sm;
    sm.begin(s);
    return sm;
}

static void test_sm_starts_idle() {
    app::TimerStateMachine sm = makeSm();
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Idle),
                      static_cast<int>(sm.phase()));
}

static void test_sm_idle_start_runs() {
    app::TimerStateMachine sm = makeSm();
    sm.dispatch(InputCommand::StartPause);
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Running),
                      static_cast<int>(sm.phase()));
}

static void test_sm_running_pause_resume() {
    app::TimerStateMachine sm = makeSm();
    sm.dispatch(InputCommand::StartPause); // -> Running
    sm.dispatch(InputCommand::StartPause); // -> Paused
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Paused),
                      static_cast<int>(sm.phase()));
    sm.dispatch(InputCommand::StartPause); // -> Running
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Running),
                      static_cast<int>(sm.phase()));
}

static void test_sm_runs_to_finished_and_flags_once() {
    app::TimerStateMachine sm = makeSm(); // 3 s
    sm.dispatch(InputCommand::StartPause);
    sm.dispatch(InputCommand::Tick);
    sm.dispatch(InputCommand::Tick);
    sm.dispatch(InputCommand::Tick);
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Finished),
                      static_cast<int>(sm.phase()));
    TEST_ASSERT_TRUE(sm.takeJustFinished());
    TEST_ASSERT_FALSE(sm.takeJustFinished()); // edge-triggered, cleared
}

static void test_sm_finished_ack_returns_idle() {
    app::TimerStateMachine sm = makeSm();
    sm.dispatch(InputCommand::StartPause);
    for (int i = 0; i < 3; ++i) sm.dispatch(InputCommand::Tick);
    sm.dispatch(InputCommand::StartPause); // ack
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Idle),
                      static_cast<int>(sm.phase()));
}

static void test_sm_idle_ignores_tick() {
    app::TimerStateMachine sm = makeSm();
    sm.dispatch(InputCommand::Tick); // must not start counting
    TEST_ASSERT_EQUAL(static_cast<int>(TimerPhase::Idle),
                      static_cast<int>(sm.phase()));
    TEST_ASSERT_EQUAL_INT32(3, sm.snapshot().display.seconds());
}

static void test_sm_setting_adjusts_duration() {
    app::TimerStateMachine sm = makeSm();
    sm.dispatch(InputCommand::EnterMenu); // -> Setting
    const int32_t before = sm.snapshot().total.seconds();
    sm.dispatch(InputCommand::Increment); // +1 step (60s)
    TEST_ASSERT_EQUAL_INT32(before + app::TimerStateMachine::kStepSeconds,
                            sm.snapshot().total.seconds());
}

// ------------------------------------------------- SettingsModel/store -----
static void test_settings_store_roundtrip() {
    fakes::FakeSettingsStore store;
    SettingsModel out;
    TEST_ASSERT_FALSE(store.load(out)); // empty

    SettingsModel in;
    in.defaultDuration = Duration(1234);
    in.mode = TimerMode::CountUp;
    store.save(in);

    SettingsModel loaded;
    TEST_ASSERT_TRUE(store.load(loaded));
    TEST_ASSERT_EQUAL_INT32(1234, loaded.defaultDuration.seconds());
    TEST_ASSERT_EQUAL(static_cast<int>(TimerMode::CountUp),
                      static_cast<int>(loaded.mode));
}

static void test_settings_schema_mismatch_rejected() {
    fakes::FakeSettingsStore store;
    SettingsModel bad;
    bad.schemaVersion = 999;
    store.seed(bad);
    SettingsModel out;
    TEST_ASSERT_FALSE(store.load(out)); // incompatible schema
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_duration_clamps_negative_to_zero);
    RUN_TEST(test_duration_saturating_subtraction);
    RUN_TEST(test_duration_parts);
    RUN_TEST(test_duration_format_mmss_and_hms);
    RUN_TEST(test_duration_clamped_range);

    RUN_TEST(test_timer_countdown_ticks_to_finish);
    RUN_TEST(test_timer_fraction_remaining);
    RUN_TEST(test_timer_countup_displays_elapsed);
    RUN_TEST(test_timer_adjust_total_clamped);

    RUN_TEST(test_sm_starts_idle);
    RUN_TEST(test_sm_idle_start_runs);
    RUN_TEST(test_sm_running_pause_resume);
    RUN_TEST(test_sm_runs_to_finished_and_flags_once);
    RUN_TEST(test_sm_finished_ack_returns_idle);
    RUN_TEST(test_sm_idle_ignores_tick);
    RUN_TEST(test_sm_setting_adjusts_duration);

    RUN_TEST(test_settings_store_roundtrip);
    RUN_TEST(test_settings_schema_mismatch_rejected);
    return UNITY_END();
}
