// Native Unity tests for the e-paper refresh Strategy.
#include <unity.h>

#include "app/HybridRefreshPolicy.h"
#include "domain/Layout.h"
#include "domain/RenderModel.h"

using namespace domain;

void setUp() {}
void tearDown() {}

static RenderModel modelWithDisplay(int32_t secs) {
    RenderModel m;
    m.display = Duration(secs);
    return m;
}

static void test_first_decision_is_full() {
    app::HybridRefreshPolicy policy(3);
    RenderModel m;
    const RefreshDecision d = policy.decide(m, m);
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Full),
                      static_cast<int>(d.kind));
}

static void test_no_change_is_none() {
    app::HybridRefreshPolicy policy(3);
    RenderModel m;
    policy.decide(m, m);                 // consume the forced first full
    const RefreshDecision d = policy.decide(m, m);
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::None),
                      static_cast<int>(d.kind));
}

static void test_digit_change_is_byte_aligned_partial() {
    app::HybridRefreshPolicy policy(10);
    RenderModel a = modelWithDisplay(0);
    RenderModel b = modelWithDisplay(1);
    policy.decide(a, a); // forced full
    const RefreshDecision d = policy.decide(b, a);
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Partial),
                      static_cast<int>(d.kind));
    TEST_ASSERT_GREATER_THAN_INT16(0, d.region.w);
    TEST_ASSERT_EQUAL_INT16(0, d.region.x % 8); // X snapped to 8px
    TEST_ASSERT_EQUAL_INT16(0, d.region.w % 8); // width snapped to 8px
}

static void test_partial_budget_forces_full() {
    app::HybridRefreshPolicy policy(3);
    RenderModel a = modelWithDisplay(0);
    RenderModel b = modelWithDisplay(1);
    policy.decide(a, a); // forced full, counter = 0

    // 3 changing frames -> 3 partials.
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Partial),
                      static_cast<int>(policy.decide(b, a).kind));
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Partial),
                      static_cast<int>(policy.decide(a, b).kind));
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Partial),
                      static_cast<int>(policy.decide(b, a).kind));
    TEST_ASSERT_EQUAL_INT(3, policy.partialsSinceFull());

    // Budget spent -> cleansing full, counter resets.
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Full),
                      static_cast<int>(policy.decide(a, b).kind));
    TEST_ASSERT_EQUAL_INT(0, policy.partialsSinceFull());
}

static void test_request_full_overrides_no_change() {
    app::HybridRefreshPolicy policy(10);
    RenderModel m;
    policy.decide(m, m); // forced first full
    policy.requestFull();
    const RefreshDecision d = policy.decide(m, m); // identical, but forced
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Full),
                      static_cast<int>(d.kind));
}

static void test_fraction_change_dirties_arc() {
    app::HybridRefreshPolicy policy(10);
    RenderModel a;
    a.fractionRemaining = 1.0F;
    RenderModel b = a;
    b.fractionRemaining = 0.5F;
    policy.decide(a, a); // forced full
    const RefreshDecision d = policy.decide(b, a);
    TEST_ASSERT_EQUAL(static_cast<int>(RefreshDecision::Kind::Partial),
                      static_cast<int>(d.kind));
    // Region must cover the arc band.
    TEST_ASSERT_LESS_OR_EQUAL_INT16(layout::kArc.y, d.region.y);
    TEST_ASSERT_GREATER_THAN_INT16(0, d.region.h);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_first_decision_is_full);
    RUN_TEST(test_no_change_is_none);
    RUN_TEST(test_digit_change_is_byte_aligned_partial);
    RUN_TEST(test_partial_budget_forces_full);
    RUN_TEST(test_request_full_overrides_no_change);
    RUN_TEST(test_fraction_change_dirties_arc);
    return UNITY_END();
}
