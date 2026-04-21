#include "CommandBar.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

using namespace TestApp::Panels;

CommandBar::CommandBar() : Instance("CommandBarPanel") {
	script = std::make_shared<Nyanners::Instances::Script>();
	script->initialize_script();
	script->name = "<eval>";
}

void CommandBar::draw() {
	ImGui::Begin("CommandBar", nullptr, ImGuiWindowFlags_NoTitleBar);

	ImGui::SameLine();
	ImGui::Text("Run:");
	ImGui::PushItemWidth(-FLT_MIN);
	ImGui::SameLine();
	if (ImGui::InputText("##CommandBar", &commandInput, ImGuiInputTextFlags_EnterReturnsTrue)) {
		script->set_source(commandInput);
		script->run_script();
	};
	ImGui::PopItemWidth();

	ImGui::Begin("Test Suite");
	std::string test = "DataModel.World.ena.Color = Color3.new(255, 0, 0, 255)";
	if (ImGui::Button("Run Set Color Test")) {
		script->set_source(test);
		script->run_script();
	};
	ImGui::End();

	ImGui::End();
}