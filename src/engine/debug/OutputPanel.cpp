#include "OutputPanel.h"
#include "imgui.h"
#include "utils/ImGuiColor3.h"

using namespace Nyanners::Debug::UI;

OutputPanel::OutputPanel(): Instance("OutputWindow") {
	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-Regular.ttf");

	Services::ConsoleService::onMessage.connect([this](const Core::LogLevel logLevel, std::string message) {
		messages.push_back({
			.level = logLevel,
			.message = message,
		});
	});
}

void OutputPanel::draw() {
	ImGui::Begin("Output");
	ImGui::PushFont(font);
	ImGuiListClipper clipper;
	clipper.Begin(messages.size());

	while (clipper.Step()) {
		for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; ++index) {
			const auto log = messages[index];
			const auto message = log.message.c_str();

			if (log.level == Core::LogLevel::Error) {
				ImGui::TextColored(color3_to_imvec4({234, 79, 84, 255}), message);
			} else if (log.level == Core::LogLevel::Warning) {
				ImGui::TextColored(color3_to_imvec4({255, 255, 143, 255}), message);
			} else if (log.level == Core::LogLevel::Info) {
				ImGui::TextUnformatted(message);
			} else if (log.level == Core::LogLevel::Debug) {
				ImGui::TextColored(color3_to_imvec4({38, 216, 222, 255}), message);
			}
		}
	}
	clipper.End();

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
		// Core::Logger::log_debug(std::format("{}, {}", ImGui::GetScrollY(), ImGui::GetScrollMaxY()));
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::PopFont();
	ImGui::End();
}
