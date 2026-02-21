#include "core/Application.h"

void engine_main()
{
    Application::instance().start();
}

int main(int argc, char** argv)
{
    Nyanners::Logger::log("Let's initialize the engine, despite us being in the app!");
    engine_main();
    return 0;
}
