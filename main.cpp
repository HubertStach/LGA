#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

#include <algorithm>
#include <stdio.h>

#include "automat.h"

int screenWidth = 1280;
int screenHeight = 768;

const int x_size = 400;
const int y_size = 400;
bool is_random = true;
bool auto_play = false;

int curr_turn = 0;

Automat automat;

int main(int argc, char* argv[]) {
    // Initialization

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "lga");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    // Our UI state
    ImVec4 bkgndColour(0.1f, 0.1f, 0.1f, 1.0f);

    // Main game loop
    bool running = true;
    bool showDemoWindow = false;

    automat.init(x_size, y_size, screenHeight, screenWidth);

    while (running && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(rlImGuiColors::Convert(bkgndColour));

        // start ImGui Content
        rlImGuiBegin();

        // Our menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Quit"))
                    running = false;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                if (ImGui::MenuItem("Demo Window", nullptr, showDemoWindow))
                    showDemoWindow = !showDemoWindow;

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // This is the main display area, that fills the app's window
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        //ImGui::SetNextWindowViewport(viewport->ID);
        //float toolbarWidth = std::max(250.0f, ImGui::GetContentRegionAvail().x * 0.25f);
        float toolbarWidth = 250.0f;
        if (ImGui::Begin("Main Window", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoBringToFrontOnFocus)) {

            // The toolbar
            const float minScale = 0.1f;
            const float maxScale = 4.0f;

            ImGui::BeginChild("Toolbar", ImVec2(toolbarWidth, 0.0f));

            ImGui::NewLine();
            ImGui::Checkbox("Automatyczne odtwarzanie", &auto_play);
            ImGui::LabelText("Tura", "%d", curr_turn);

            bool next_turn = false;

            if (ImGui::Button("Nowa tura")) {
                next_turn = true;
            }

            if ((automat.initialised && next_turn)
                || automat.initialised && auto_play) {

                curr_turn++;
                automat.simulate_curr_state();
            }

            ImGui::EndChild();

            ImGui::SameLine();

            // The main display

            ImGui::PushStyleColor(ImGuiCol_ChildBg, bkgndColour);
            ImGui::BeginChild("Main", ImVec2(0.0f, 0.0f),
                ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::End();

        if (showDemoWindow) {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        // End ImGui Content
        rlImGuiEnd();

        // ##### INSERT ANY ADDITIONAL RENDERING YOU WANT TO DO OVER THE GUI HERE #####

        //mainStart to szerokoœæ od której 
        float mainStart = toolbarWidth + 15;

        if (automat.initialised) {
            automat.visualise();
        }

        EndDrawing();
    }
    rlImGuiShutdown();

    // Cleanup
    rlImGuiShutdown();
    CloseWindow();

    return EXIT_SUCCESS;
}