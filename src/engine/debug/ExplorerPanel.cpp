#include "ExplorerPanel.h"
#include "Application.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "scripting/reflections/DataTypes.h"
#include "utils/ImGuiColor3.h"
#include <algorithm>

using namespace Nyanners::Debug::UI;
using namespace Nyanners::Services;

ExplorerPanel::ExplorerPanel() : Instance("ExplorerPanel") {
	activeDm = Nyanners::Application::instance()->currentModel;
	selectionService =
	  activeDm->get_service<SelectionService>("SelectionService");
}

void ExplorerPanel::render_vector(
  const glm::vec3 &values,
  const ReflectionProperty &property,
  Instance *instance
) const {
	float vec_float[3] = {values.x, values.y, values.z};

	if (ImGui::DragFloat3("##VectorEditor", vec_float, 0.5)) {
		property.set(
		  instance, glm::vec3(vec_float[0], vec_float[1], vec_float[2]), nullptr
		);
	}
}

void ExplorerPanel::render_vector(
  const glm::vec2 &values,
  const ReflectionProperty &property,
  Instance *instance
) const {
	float vec_float[2] = {values.x, values.y};

	if (ImGui::DragFloat2("##VectorEditor", vec_float, 0.5)) {
		property.set(instance, glm::vec2(vec_float[0], vec_float[1]), nullptr);
	}
}

void ExplorerPanel::render_instance(const std::shared_ptr<Instance> &instance) {
	const ImGuiStyle &style = ImGui::GetStyle();
	auto alphabetically = instance->children;

	std::ranges::sort(
	  alphabetically,
	  [](const std::shared_ptr<Instance> &a, const std::shared_ptr<Instance> &b) {
		  return b->name < a->name;
	  }
	);

	ImGui::PushID(instance.get());
	ImGuiTreeNodeFlags flags =
	  ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

	if (instance->children.empty()) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool isOpened =
	  ImGui::TreeNodeEx(static_cast<const void *>(nullptr), flags, "%s", "");

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("InstanceContext");
	}

	if (ImGui::BeginPopupContextItem("InstanceContext")) {
		if (ImGui::MenuItem("Clone")) {
			try {
				const auto clone = instance->clone();
				instance->parent.lock()->add_child(clone);
			} catch (std::runtime_error& err) {
				std::string error = err.what();

				Core::Logger::log_error(std::format("Cloning of object {} ({}) failed: {}", instance->name, instance->baseName, error));
			}
		}

		if (ImGui::MenuItem("Destroy")) {
			instance->parent.lock()->remove_child(instance);
		}
		ImGui::EndPopup();
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
		selectionService->set_selection(instance);
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8.0f);

	const auto& icon = classIcons.find(instance->baseName);

	if (icon != classIcons.end()) {
		ImGui::ImageWithBg(
		  icon->second->get_texture_handle(),
		  ImVec2(20.f, 20.f),
		  ImVec2(0.f, 1.f),
		  ImVec2(1.f, 0.f),
		  ImVec4(),
		  instance->active ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(.5f, .5f, .5f, 1.f)
		);
	} else {
		ImGui::ImageWithBg(
		  classIcons.at("Unknown")->get_texture_handle(),
		  ImVec2(20.f, 20.f),
		  ImVec2(0.f, 1.f),
		  ImVec2(1.f, 0.f),
		  ImVec4(),
		  instance->active ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(.5f, .5f, .5f, 1.f)
		);
	}

	ImGui::SameLine();
	ImGui::Text(instance->name.c_str());

	if (isOpened) {
		for (const auto &child : alphabetically) {
			render_instance(child);
		}

		if (!instance->children.empty()) {
			ImGui::TreePop();
		}
	}


	ImGui::PopID();
}

void ExplorerPanel::draw() {
	ImGui::Begin("Explorer");
	ImGui::SetNextItemOpen(true);
	render_instance(activeDm);
	ImGui::End();

	ImGui::Begin("Properties");

	auto windowSize = ImGui::GetWindowSize();

	if (selectionService->currentSelection != nullptr) {
		std::shared_ptr<Instance> &selection = selectionService->currentSelection;
		const auto properties =
			ReflectionService::get_properties(selectionService->currentSelection);

		ImGui::PushID(selection.get());

		// TODO: figure this out
		// std::map<std::string, std::vector<ReflectionProperty>>
		//   propertiesByCategory{};
		//
		// for (const auto &property : properties) {
		// 	if (auto result = propertiesByCategory.find(property.category);
		// 	    result != propertiesByCategory.end()) {
		// 		result->second.push_back(property);
		// 	} else {
		// 		propertiesByCategory.emplace(
		// 		  property.category, std::vector<ReflectionProperty>{}
		// 		);
		// 	}
		// }
		//
		// for (const auto &[category, properties] : propertiesByCategory) {
		// 	if (properties.empty()) {
		// 		continue;
		// 	}
		//
		// 	if (ImGui::CollapsingHeader(category.c_str())) {
		// 	}
		// }

		if (ImGui::BeginTable("editor", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |  ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
			ImGui::TableSetupColumn("property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			for (const auto &property : properties) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(property.name.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(-FLT_MIN);
				display_property(selection, property);

				ImGui::PopItemWidth();
			}

			ImGui::EndTable();
		}

		if (selection->baseName == "Script") {
			const auto script = std::dynamic_pointer_cast<Instances::Script>(selection);
			const float size = ImGui::CalcTextSize("Reload Script").x + ImGui::GetStyle().FramePadding.x * 2.0f;
			const float avail = ImGui::GetContentRegionAvail().x;

			const float off = (avail - size) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

			if (ImGui::Button("Reload Script", ImVec2(100.0f, 0))) {
				Core::Logger::log(std::format("Trying to reload {}", script->name));
				script->reload();
			}
		}


		ImGui::PopID();
	} else {
		ImGui::Text("No Instance selected");
	}

	ImGui::End();

	ImGui::Begin("ImGui Style Editor");
	ImGui::ShowStyleEditor();
	ImGui::End();
}

StringValueCache &ExplorerPanel::get_or_make_string_cache(
  const std::shared_ptr<Instance> &instance,
  const std::string &property,
  const std::string &value
) {
	for (auto &buffer : stringValueBuffers) {
		if (buffer.property == property && buffer.instance == instance) {
			return buffer;
		}
	}

	StringValueCache &cache = stringValueBuffers.emplace_back();
	cache.instance = instance;
	cache.property = property;
	cache.value = value;
	std::copy_n(
	  value.begin(),
	  std::min(value.size(), cache.buffer.size()),
	  cache.buffer.data()
	);

	stringValueBuffers.push_back(cache);
	return cache;
}

void ExplorerPanel::display_property(
  const std::shared_ptr<Instance> &selection, const ReflectionProperty &property
) {
	const auto& instance = selection.get();

	ImGui::PushID(property.name.c_str());
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	ReflectionValue value;
	property.get(instance, value, nullptr);

	// ImGui::Text(property.name.c_str());
	// ImGui::SameLine();

	if (property.has_flag(ReflectionPropertyFlags::ReadOnly)) {
		ImGui::BeginDisabled(true);
	}

	if (property.type == String) {
		auto &buffer = get_or_make_string_cache(
		  selection, property.name, std::get<std::string>(value)
		);

		if (ImGui::InputText(
		      "##TextInput",
		      buffer.buffer.data(),
		      buffer.buffer.size(),
		      ImGuiInputTextFlags_EnterReturnsTrue
		    )) {
			property.set(instance, buffer.buffer.data(), nullptr);
		}
	} else if (property.type == Boolean) {
		auto boolean = std::get<bool>(value);

		if (ImGui::Checkbox("##BooleanInput", &boolean)) {
			property.set(instance, boolean, nullptr);
		};
	} else if (property.type == Number) {
		double number = std::get<double>(value);

		if (ImGui::InputDouble("##NumberInput", &number, 1)) {
			property.set(instance, number, nullptr);
		};
	} else if (property.type == Integer) {
		int number = std::get<int>(value);

		if (ImGui::InputInt("##NumberInput", &number, 1)) {
			property.set(instance, number, nullptr);
		};
	} else if (property.type == Vector3) {
		const auto vector = std::get<glm::vec3>(value);
		render_vector(vector, property, instance);
	} else if (property.type == Vector2) {
		const auto vector = std::get<glm::vec2>(value);
		render_vector(vector, property, instance);
	} else if (property.type == Color) {
		const auto color = std::get<DataTypes::Color3>(value);
		display_color_property(color, property, instance);
	} else if (property.type == ReflectionPropertyType::Instance) {
		auto& instance = std::get<std::shared_ptr<Instance>>(value);

		if (instance != nullptr) {
			ImGui::Text(instance->name.c_str());
			instance.reset();
		} else {
			ImGui::TextUnformatted("none :(");
		}

	}
	else {
		ImGui::TextColored(
		  color3_to_imvec4({255, 0, 0, 255}), "No compatible value"
		);
	}

	if (property.has_flag(ReflectionPropertyFlags::ReadOnly)) {
		ImGui::EndDisabled();
	}


	ImGui::PopID();
}

void ExplorerPanel::display_color_property(
  const DataTypes::Color3 &color,
  const ReflectionProperty &property,
  Instance *instance
) {
	float color_float[4] = {
	  static_cast<float>(color.r) / 255.0f,
	  static_cast<float>(color.g) / 255.0f,
	  static_cast<float>(color.b) / 255.0f,
	  static_cast<float>(color.alpha) / 255.0f
	};

	if (ImGui::ColorEdit4("", color_float)) {
		property.set(
		  instance,
		  DataTypes::Color3(
		    static_cast<int>(color_float[0] * 255.0f),
		    static_cast<int>(color_float[1] * 255.0f),
		    static_cast<int>(color_float[2] * 255.0f),
		    static_cast<int>(color_float[3] * 255.0f)
		  ),
		  nullptr
		);
		Nyanners::Core::Logger::log(
		  std::format(
		    "{},{},{}.{}",
		    color_float[0],
		    color_float[1],
		    color_float[2],
		    color_float[3]
		  )
		);
	}
}