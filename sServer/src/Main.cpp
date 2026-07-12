#include "sCore/include/Example.h"
#include "spdlog/spdlog.h"

int main()
{
  Example::Test();

  // test spdlog
  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);
}