#include "stdlib.h"
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "./instances/DataModel.h"
#include "./instances/Rectangle.h"
#include "./instances/Script.h"

// #include "./lua/luaImport.h"
#include "./lua/system.h"
#include "./lua/utils.h"

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

void engine_exit() { exit(1); }

void engine_panic(std::string message)
{
    system((std::string("kdialog --title \"Engine Failure\" --error \"") + message + std::string("\""))
            .c_str());
    engine_exit();
}

int main()
{
    Nyanners::Instances::DataModel dataModel;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Engine");

    SetTargetFPS(60);

    Nyanners::Instances::Script* script = new Nyanners::Instances::Script;
    Nyanners::Instances::Rectangle* shape = new Nyanners::Instances::Rectangle;

    dataModel.addChild(script);
    dataModel.addChild(shape);
    script->loadFromFile("system/luau/autorun.luau");

    while (!WindowShouldClose()) {
        BeginDrawing();

        int fps = GetFPS();
        char* newTitle;

        if (0 > asprintf(&newTitle, "Engine FPS: %d", fps)) {
            engine_panic("Allocation error on window title");
        }

        SetWindowTitle(newTitle);
        free(newTitle);
        dataModel.update();

        // DrawText("hiii", (GetScreenWidth() / 2), (GetScreenHeight() / 2), 20,
        //  WHITE);
        ClearBackground(BLACK);
        EndDrawing();
    }

    return 0;
}