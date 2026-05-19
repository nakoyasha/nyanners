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
  const glm::vec3 &values, const ReflectionProperty& property, Instance *instance
) const {
	float vec_float[3] = {values.x, values.y, values.z};

	if (ImGui::DragFloat3("##VectorEditor", vec_float, 0.5)) {
		property.set(instance, glm::vec3(vec_float[0], vec_float[1], vec_float[2]), nullptr);
	}
}

void ExplorerPanel::render_vector(
  const glm::vec2 &values, const ReflectionProperty& property, Instance *instance
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

	if (ImGui::IsItemClicked()) {
		selectionService->set_selection(instance);
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8.0f);

	const auto icon = classIcons.find(instance->baseName);

	if (icon != classIcons.end()) {
		ImGui::ImageWithBg(
		  icon->second->get_texture_handle(),
		  ImVec2(16.f, 16.f),
		  ImVec2(0.f, 1.f),
		  ImVec2(1.f, 0.f),
		  ImVec4(),
		  instance->active ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(.5f, .5f, .5f, 1.f)
		);
	} else {
		ImGui::ImageWithBg(
		  classIcons.at("Unknown")->get_texture_handle(),
		  ImVec2(16.f, 16.f),
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

	if (selectionService->currentSelection != nullptr) {
		std::shared_ptr<Instance> &selection = selectionService->currentSelection;
		static char searchBuffer[1024];
		ImGui::InputText(
		  "Search for properties", searchBuffer, IM_ARRAYSIZE(searchBuffer)
		);
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

		for (const auto &property : properties) {
			display_property(selection, property);
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
		if (buffer.value == property && buffer.instance == instance) {
			return buffer;
		}
	}

	StringValueCache &cache = stringValueBuffers.emplace_back();
	cache.instance = instance;
	cache.value = value;
	std::copy(
	  value.begin(),
	  value.begin() + std::min(value.size(), cache.buffer.size()),
	  cache.buffer.data()
	);

	stringValueBuffers.push_back(cache);
	return cache;
}
void ExplorerPanel::display_property(
  const std::shared_ptr<Instance> &selection, const ReflectionProperty &property
) {
	auto instance = selection.get();

	ImGui::PushID(property.name.c_str());

	ReflectionValue value;
	property.get(instance, value, nullptr);

	ImGui::Text(property.name.c_str());
	ImGui::SameLine();

	if (property.type == String) {
		auto &buffer = get_or_make_string_cache(selection, property.name, std::get<std::string>(value));

		if (ImGui::InputText("##TextInput",buffer.buffer.data(),buffer.buffer.size(),ImGuiInputTextFlags_EnterReturnsTrue)) {
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
	} else if (property.type == Vector3) {
		auto vector = std::get<glm::vec3>(value);
		render_vector(vector, property, instance);
	} else if (property.type == Vector2) {
		auto vector = std::get<glm::vec2>(value);
		render_vector(vector, property, instance);
	} else if (property.type == Color) {
		auto color = std::get<DataTypes::Color3>(value);
		display_color_property(color, property, instance);
	}
	else {
		ImGui::TextColored(color3_to_imvec4({255, 0, 0, 255}), "No compatible value");
	}

	// switch (property.type) {
	// 	case Unknown:
	// 		break;
	// 	case String:
	// 		break;
	// 	case Number:
	// 		break;
	// 	case Boolean:
	// 		break;
	// 	case ReflectionPropertyType::Instance:
	// 		break;
	// 	case Vector3:
	// 		break;
	// 	case Vector2:
	// 		break;
	// 	case Color:
	// 		break;
	// 	case UserData:
	// 		break;
	// }

	// try {
	// 	property.get(selection, script->context);
	// 	const int type = lua_type(script->context, -1);
	//
	// 	ImGui::Text(property.name.c_str());
	// 	ImGui::SameLine();
	//
	// 	if (type == LUA_TNUMBER) {
	// 		double value = lua_tonumber(script->context, -1);
	//
	// 		if (ImGui::InputDouble("##NumberInput", &value, 1)) {
	// 			lua_pushnumber(script->context, (int)value);
	// 			property.set(selection, script->context);
	// 		};
	//
	// 	} else if (type == LUA_TSTRING) {
	// 		const std::string value = lua_tostring(script->context, -1);
	// 		auto &buffer = get_or_make_string_cache(instance, property.name, value);
	//
	// 		if (ImGui::InputText(
	// 		      "##TextInput",
	// 		      buffer.buffer.data(),
	// 		      buffer.buffer.size(),
	// 		      ImGuiInputTextFlags_EnterReturnsTrue
	// 		    )) {
	// 			std::string string = buffer.buffer.data();
	//
	// 			lua_pushstring(script->context, string.c_str());
	// 			property.set(selection, script->context);
	// 		};
	//
	// 	} else if (type == LUA_TBOOLEAN) {
	// 		bool value = lua_toboolean(script->context, -1);
	//
	// 		if (ImGui::Checkbox("##BooleanInput", &value)) {
	// 			lua_pushboolean(script->context, value);
	// 			property.set(selection, script->context);
	// 		};
	// 	} else if (type == LUA_TUSERDATA) {
	// 		const auto *childInstance =
	// 								ReflectionService::get_instance_from_context(script->context, -1);
	// 		auto *color3 = ReflectionService::get_userdata_from_context<
	// 			Nyanners::DataTypes::Color3>(script->context, -1, 0x05);
	// 		auto *vec3 = ReflectionService::get_userdata_from_context<glm::vec3>(
	// 			script->context, -1, 0x06
	// 		);
	//
	// 		auto *vec2 = ReflectionService::get_userdata_from_context<glm::vec3>(
	// 			script->context, -1, 0x07
	// 		);
	//
	// 		const auto isInstance =
	// 			childInstance == nullptr || childInstance->pointer == nullptr;
	// 		const auto isColor3 = color3 != nullptr;
	// 		const auto isVec3 = vec3 != nullptr;
	// 		const auto isVec2 = vec2 != nullptr;
	//
	// 		if (!isInstance && !isColor3 && !isVec3 && !isVec2) {
	// 			ImGui::Text("None");
	// 		} else {
	// 			if (childInstance != nullptr) {
	// 				ImGui::Text(childInstance->pointer->name.c_str());
	// 			}
	//
	// 			if (color3 != nullptr) {
	// 				display_color_property(instance, *color3);
	// 			}
	//
	// 			if (vec3 != nullptr) {
	// 				render_vec3(*vec3, property, selection);
	// 			}
	//
	// 			if (vec2 != nullptr) {
	// 				render_vec2(*vec2, property, selection);
	// 			}
	// 		}
	// 	} else {
	// 		ImGui::Text(property.name.c_str());
	// 		ImGui::SameLine();
	// 		ImGui::Text("idk lol");
	// 	}
	//
	// 	lua_pop(script->context, -1);
	// } catch (void *exception) {
	// 	ImGui::Text("idk lol");
	// }
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

	if (ImGui::ColorEdit4(property.name.c_str(), color_float)) {
		property.set(instance, DataTypes::Color3(
			color_float[0] * 255.0f,
			color_float[1] * 255.0f,
			color_float[2] * 255.0f,
			color_float[3] * 255.0f
			), nullptr);
		Nyanners::Core::Logger::log(
		  std::format("{},{},{}.{}", color_float[0], color_float[1], color_float[2], color_float[3])
		);
	}
}