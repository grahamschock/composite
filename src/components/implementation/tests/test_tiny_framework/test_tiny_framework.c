#include <cos_component.h>
#include <llprint.h>
#include <tinytest.h>

void
test_fail()
{
  ASSERT("This test will fail", 1 == 0);
}
void
test_pass()
{
  ASSERT("This test will pass", 1 == 1);
}
void
cos_init(void)
{
  RUN(test_fail);
  RUN(test_pass);
  TEST_REPORT();
  while(1);
}
