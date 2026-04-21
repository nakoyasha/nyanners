#include "OutputPanel.h"
#include "imgui.h"

using namespace TestApp::Panels;

void OutputPanel::draw() {
	ImGui::Begin("Output");
	ImGuiListClipper clipper;
	clipper.Begin(messages.size());

	while (clipper.Step()) {
		for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; ++index) {
			const auto log = messages[index];
			const auto message = log.message.c_str();
			if (log.level == Nyanners::Core::LogLevel::Error) {
				ImGui::TextColored(ImVec4(0.92f, 0.31, 0.33, 1.0f), message);
			} else if (log.level == Nyanners::Core::LogLevel::Warning) {
				ImGui::TextColored(ImVec4(255 / 255, 255 / 255, 143 / 255, 1.0f), message);
			} else if (log.level == Nyanners::Core::LogLevel::Info) {
				ImGui::TextUnformatted(message);
			}
		}
	}
	clipper.End();

	ImGui::SetScrollHereY(1.0f);
	// ImGui::EndChild();
	ImGui::End();
}