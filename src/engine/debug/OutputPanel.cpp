#include "OutputPanel.h"
#include "imgui.h"
#include "utils/ImGuiColor3.h"

using namespace Nyanners::Debug::UI;

void OutputPanel::draw() {
	ImGui::Begin("Output");
	ImGuiListClipper clipper;
	clipper.Begin(messages.size());

	while (clipper.Step()) {
		for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; ++index) {
			const auto log = messages[index];
			const auto message = log.message.c_str();

			if (log.level == Nyanners::Core::LogLevel::Error) {
				ImGui::TextColored(color3_to_imvec4({234, 79, 84, 255}), message);
			} else if (log.level == Nyanners::Core::LogLevel::Warning) {
				ImGui::TextColored(color3_to_imvec4({255, 255, 143, 255}), message);
			} else if (log.level == Nyanners::Core::LogLevel::Info) {
				ImGui::TextUnformatted(message);
			} else if (log.level == Nyanners::Core::LogLevel::Debug) {
				ImGui::TextColored(color3_to_imvec4({38, 216, 222, 255}), message);
			}
		}
	}
	clipper.End();

	ImGui::SetScrollHereY(1.0f);
	// ImGui::EndChild();
	ImGui::End();
}