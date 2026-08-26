//
// Created by Haruka on 23.08.2026.
//

#include "ScriptEditor.h"

#include "imgui.h"

using namespace Nyanners::Debug;

ScriptEditor::ScriptEditor(std::shared_ptr<Instances::Script> script): Instance("ScriptEditor") {
	this->openScript = script;
}

void ScriptEditor::imgui_render() {
	ImGui::Begin(std::format("{}###ScriptEditor", this->openScript->name).c_str());
	auto string = this->openScript->source.c_str();

	// ImGui::InputTextMultiline("###Text", string, 1024 * 512, ImVec2(512, 512), 0, 0, nullptr);
	ImGui::End();
}
