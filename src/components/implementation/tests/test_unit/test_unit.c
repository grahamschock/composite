#include <cos_component.h>
#include <llprint.h>
#include "unity.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {

}

void test_function_should_doBlahAndBlah(void) {
  TEST_ASSERT_EQUAL(1,1);

}

void test_function_should_doAlsoDoBlah(void) {
  TEST_ASSERT_EQUAL(1,2);
}

void
cos_init(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_function_should_doBlahAndBlah);
  RUN_TEST(test_function_should_doAlsoDoBlah);
  UNITY_END();
  while(1);

}
