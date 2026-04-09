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

	ImGui::End();
}