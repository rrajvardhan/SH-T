#include "Application.hpp"
#include "Log.hpp"

int
main()
{
  Application app;
  if (!app.init())
  {
    LOG_ERROR("app failed to initialize.");
    return -1;
  }
  app.run();

  return 0;
}
