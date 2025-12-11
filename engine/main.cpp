#include "core/Application.h"

void engine_main()
{
    Application::instance().start();
}

// void headless_main()
// {
    // Application::instance(true).start();
// }

int main(int argc, char** argv)
{
    engine_main();
    return 0;
}
