//
// Created by Haruka on 19.02.2026.
//

#include "libImmediate.h"

#include "imgui.h"
#include "rlImGui.h"
#include "core/Logger.h"
#include "lua/reflection/Reflection.h"

void libImmediate_open(lua_State *context) {
	reflection_luaPushStruct(context, {
		{"startImmediate", &libImmediate_start},
		{"text", &libImmediate_text},
		{"inputText", &libImmediate_inputText},
		{"button", &libImmediate_button},
{"combo", &libImmediate_combo},
	{"getWindowSize", &libImmediate_getWindowSize},
{"setWindowSize", &libImmediate_setWindowSize},
		{"getWindowPosition", &libImmediate_getWindowPosition},
{"setWindowPosition", &libImmediate_setWindowPosition},
		{"endImmediate", &libImmediate_end},
	});

	// Push WindowFlags table
	reflection_luaPushStruct(context, {
		{"None", ImGuiWindowFlags_None},
		{"NoTitleBar", ImGuiWindowFlags_NoTitleBar},
		{"NoResize", ImGuiWindowFlags_NoResize},
		{"NoMove", ImGuiWindowFlags_NoMove},
		{"NoScrollbar", ImGuiWindowFlags_NoScrollbar},
		{"NoScrollWithMouse", ImGuiWindowFlags_NoScrollWithMouse},
		{"NoCollapse", ImGuiWindowFlags_NoCollapse},
		{"AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize},
		{"NoBackground", ImGuiWindowFlags_NoBackground},
		{"NoSavedSettings", ImGuiWindowFlags_NoSavedSettings},
		{"NoMouseInputs", ImGuiWindowFlags_NoMouseInputs},
		{"MenuBar", ImGuiWindowFlags_MenuBar},
		{"HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar},
		{"NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing},
		{"NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus},
		{"AlwaysVerticalScrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar},
		{"AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar},
		{"NoNavInputs", ImGuiWindowFlags_NoNavInputs},
		{"NoNavFocus", ImGuiWindowFlags_NoNavFocus},
		{"UnsavedDocument", ImGuiWindowFlags_UnsavedDocument},
	});

	lua_setfield(context, -2, "flags");

	lua_setglobal(context, "immediate");
}

int libImmediate_start(lua_State *context) {
	rlImGuiBegin();
	const std::string windowName = luaL_checkstring(context, 1);

	ImGuiWindowFlags flags = 0;

	if (lua_gettop(context) >= 2 && !lua_isnil(context, 2)) {
		flags = luaL_checkinteger(context, 2);
	}

	ImGui::Begin(windowName.c_str(), nullptr, flags);

	return 0;
}

int libImmediate_text(lua_State *context) {
	const std::string text = luaL_checkstring(context, -1);
	ImGui::Text(text.c_str());
	return 0;
}

struct libImmediate_LuaInput {
	char buffer[1024];
	// int callbackId;
};

std::map<std::string, libImmediate_LuaInput*> inputs;

int libImmediate_inputTextCallbackHandler(lua_State* context, const std::string text, libImmediate_LuaInput* input) {
	lua_pushstring(context, input->buffer);

	if (ImGui::InputText(text.c_str(), input->buffer, sizeof(input->buffer))) {
		lua_pushboolean(context, true);
	} else {
		lua_pushboolean(context, false);
	}

	return 2;
}

int libImmediate_getWindowSize(lua_State* context) {
	ImVec2 size = ImGui::GetWindowSize();

	reflection_luaPushStruct(context, {
				{"X", LuaValue(size.x)},
				{"Y", LuaValue(size.y)}
	});
	return 1;
}

int libImmediate_getWindowPosition(lua_State* context) {
	ImVec2 size = ImGui::GetWindowPos();

	reflection_luaPushStruct(context, {
				{"X", LuaValue(size.x)},
				{"Y", LuaValue(size.y)}
	});
	return 1;
}

int libImmediate_setWindowPosition(lua_State* context) {
	const float* luaVector = luaL_checkvector(context, -1);
	ImGui::SetWindowPos(ImVec2(luaVector[0], luaVector[1]));

	return 0;
}

int libImmediate_setWindowSize(lua_State* context) {
	const float* luaVector = luaL_checkvector(context, -1);
	ImGui::SetWindowSize(ImVec2(luaVector[0], luaVector[1]));

	return 0;
}

int libImmediate_inputText(lua_State *context) {
	const std::string text = luaL_checkstring(context, -1);
	// const int callbackRef = lua_getref(context, -1);

	auto it = inputs.find(text);

	if (it == inputs.end()) {
		auto input = new libImmediate_LuaInput{};
		inputs.insert({text, input});
		return libImmediate_inputTextCallbackHandler(context, text, input);
	} else {
		libImmediate_LuaInput* input = it->second;
		return libImmediate_inputTextCallbackHandler(context, text, input);
	}

}

int libImmediate_button(lua_State* context) {
	const std::string text = luaL_checkstring(context, -1);

	if (ImGui::Button(text.c_str())) {
		lua_pushboolean(context, true);
		return 1;
	} else {
		lua_pushboolean(context, false);
		return 0;
	}
}

// i blatantly took this from chatgpt because i really can't be bothered anymore,
// sorry! - haruka

struct libImmediate_LuaCombo {
	int currentIndex = 0;
};

std::map<std::string, libImmediate_LuaCombo*> combos;
int libImmediate_combo(lua_State* context) {
	const std::string label = luaL_checkstring(context, 1);
	luaL_checktype(context, 2, LUA_TTABLE);

	// Collect items from Lua table
	std::vector<std::string> items;
	std::vector<const char*> itemPtrs;

	lua_pushnil(context);
	while (lua_next(context, 2) != 0) {
		const char* item = luaL_checkstring(context, -1);
		items.emplace_back(item);
		lua_pop(context, 1);
	}

	for (auto& s : items) {
		itemPtrs.push_back(s.c_str());
	}

	// Get or create combo state
	auto it = combos.find(label);
	libImmediate_LuaCombo* combo;

	if (it == combos.end()) {
		combo = new libImmediate_LuaCombo{};
		combos.insert({label, combo});
	} else {
		combo = it->second;
	}

	bool changed = ImGui::Combo(
		label.c_str(),
		&combo->currentIndex,
		itemPtrs.data(),
		static_cast<int>(itemPtrs.size())
	);

	// to account for lua weirdness
	lua_pushinteger(context, combo->currentIndex + 1);
	lua_pushboolean(context, changed);

	return 2;
}

int libImmediate_end(lua_State *context) {
	ImGui::End();
	rlImGuiEnd();
	return 0;
}
