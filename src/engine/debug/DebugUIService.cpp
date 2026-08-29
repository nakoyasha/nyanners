#include "debug/DebugUIService.h"

#include "Application.h"
#include "CommandBar.h"
#include "ExplorerPanel.h"
#include "imgui.h"
#include "imgui_freetype.h"
#include "imgui_impl_opengl3.h"
#include "MainMenubar.h"
#include "OutputPanel.h"
#include "ViewportPanel.h"
#include "instances/drawable/Drawable.h"
#include "instances/services/RenderingService.h"
#include "instances/services/user/InputService.h"

using namespace Nyanners::Services;
bool DebugUIService::renderWindows = false;

namespace Nyanners::Scripting {
	auto debugUIService = ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		ReflectionService::create_descriptor("DebugUIService", {"Instance"})
			.add_property_chained<DebugUIService, bool, &DebugUIService::get_demo_open, &DebugUIService::set_demo_open>("DemoWindowEnabled", Boolean)
			.add_constructor<DebugUIService>();
	});
}

DebugUIService::DebugUIService() : Instance("DebugUIService") {
	if (!Application::instance()->is_rendering_enabled()) {
		return;
	}

	ImGuiIO &io = ImGui::GetIO();
	ImGui::StyleColorsLight();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle &style = ImGui::GetStyle();
	style.FrameBorderSize = 1.0f;
	ImVec4 *colors = style.Colors;

	static ImFontConfig fontConfig;
	fontConfig.MergeMode = true;
	fontConfig.OversampleH = fontConfig.OversampleV = 1;
	fontConfig.FontLoaderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;

	static constexpr ImWchar range[] = { 0x1, 0x1FFF, 0 };

	io.Fonts->AddFontDefault();

	io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSans.ttf", 16.0f);
	io.Fonts->AddFontFromFileTTF("assets/fonts/NotoEmoji-Regular.ttf", 16.0f, &fontConfig, range);

	// NOTICE OF AI-GENERATED CODE:
	// i could not be bothered to do this part, sorry!

	constexpr ImVec4 accent =
	  ImVec4(245.0f / 255.0f, 66.0f / 255.0f, 102.0f / 255.0f, 1.0f);
	constexpr ImVec4 accentHover = ImVec4(accent.x, accent.y, accent.z, 0.5f);
	constexpr ImVec4 accentActive = ImVec4(accent.x, accent.y, accent.z, 0.9f);
	constexpr ImVec4 accentDim = ImVec4(accent.x, accent.y, accent.z, 0.35f);
	constexpr ImVec4 accentDimSel = ImVec4(accent.x, accent.y, accent.z, 0.7f);

	//
	// Frames / Inputs
	//
	colors[ImGuiCol_FrameBg] = accentDim;
	colors[ImGuiCol_FrameBgHovered] = accentHover;
	colors[ImGuiCol_FrameBgActive] = accentActive;

	colors[ImGuiCol_SliderGrab] = accent;
	colors[ImGuiCol_SliderGrabActive] = accentActive;

	//
	// Buttons
	//
	colors[ImGuiCol_Button] = accentDim;
	colors[ImGuiCol_ButtonHovered] = accentHover;
	colors[ImGuiCol_ButtonActive] = accentActive;

	//
	// Headers (TreeNode, Selectable, CollapsingHeader)
	//
	colors[ImGuiCol_Header] = accentDim;
	colors[ImGuiCol_HeaderHovered] = accentHover;
	colors[ImGuiCol_HeaderActive] = accentActive;

	//
	// Tabs
	//
	colors[ImGuiCol_Tab] = accentDim;
	colors[ImGuiCol_TabHovered] = accentHover;
	colors[ImGuiCol_TabActive] = accentActive;
	colors[ImGuiCol_TabSelected] = accentActive;
	colors[ImGuiCol_TabSelectedOverline] = accent;

	colors[ImGuiCol_TabDimmed] = accentDim;
	colors[ImGuiCol_TabDimmedSelected] = accentDimSel;
	colors[ImGuiCol_TabDimmedSelectedOverline] = accentDimSel;

	//
	// Title bars
	//
	colors[ImGuiCol_TitleBg] = accentDim;
	colors[ImGuiCol_TitleBgActive] = accentActive;
	colors[ImGuiCol_TitleBgCollapsed] = accentDim;

	//
	// Scrollbars
	//
	colors[ImGuiCol_ScrollbarGrab] = accentDim;
	colors[ImGuiCol_ScrollbarGrabHovered] = accentHover;
	colors[ImGuiCol_ScrollbarGrabActive] = accentActive;

	//
	// Check / Radio / Markers
	//
	colors[ImGuiCol_CheckMark] = accent;

	//
	// Text / Selection / Navigation
	//
	colors[ImGuiCol_TextLink] = accent;
	colors[ImGuiCol_TextSelectedBg] = accentHover;
	colors[ImGuiCol_NavHighlight] = accentHover;
	colors[ImGuiCol_NavCursor] = accentActive;

	//
	// Resize / Drag / Separators
	//
	colors[ImGuiCol_ResizeGrip] = accentDim;
	colors[ImGuiCol_ResizeGripHovered] = accentHover;
	colors[ImGuiCol_ResizeGripActive] = accentActive;

	colors[ImGuiCol_Separator] = accentDim;
	colors[ImGuiCol_SeparatorHovered] = accentHover;
	colors[ImGuiCol_SeparatorActive] = accentActive;

	//
	// Drag & Drop
	//
	colors[ImGuiCol_DragDropTarget] = accent;

	InputService::instance()->onInput->connect([](const std::shared_ptr<Instances::InputObject> &event) {
		if (event->keyCode == Input::KeyCode::F8 && event->state == Input::InputState::Began) {
			renderWindows = !renderWindows;
		}
	});

	debugViewport = new Core::Rendering::Viewport();
	viewportFramebuffer = new Resources::FrameBuffer(1280, 720);
}

DebugUIService::~DebugUIService() {
	if (viewportFramebuffer != nullptr) {
		delete viewportFramebuffer;
		viewportFramebuffer = nullptr;
	}

	if (!Application::instance()->is_rendering_enabled()) {
		return;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void DebugUIService::draw_imgui() const {
	if (!Application::instance()->is_rendering_enabled()) {
		return;
	}

	ImGuiIO &io = ImGui::GetIO();
	const auto size = RenderingService::renderer->get_window_size();
	debugViewport->size = {size.x, size.y};

	io.DisplaySize =
	  ImVec2(static_cast<float>(size.x), static_cast<float>(size.y));

	ImGui::NewFrame();
	ImGui_ImplOpenGL3_NewFrame();

	ImGui::DockSpaceOverViewport(
	  0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode
	);

	if (renderWindows) {
		for (const auto &child : children) {
			const auto drawable = std::dynamic_pointer_cast<Drawable>(child);

			if (drawable == nullptr) {
				continue;
			}

			drawable->draw();
		}

		if (demoWindowOpen) {
			ImGui::ShowDemoWindow();
		}
	}

	ImGui::Render();
}

bool DebugUIService::get_demo_open() const {
	return this->demoWindowOpen;
}

void DebugUIService::set_demo_open(const bool isOpened) {
	this->demoWindowOpen = isOpened;
}

void DebugUIService::add_standard_elements(FrameBuffer *framebuffer) {
	if (framebuffer == nullptr) {
		framebuffer = viewportFramebuffer;
	}
	if (framebuffer == nullptr) {
		framebuffer = new Resources::FrameBuffer(1280, 720);
		viewportFramebuffer = framebuffer;
	}

	add_child(std::make_shared<Debug::UI::ExplorerPanel>());
	add_child(std::make_shared<Debug::UI::ViewportPanel>(framebuffer, debugViewport));
	add_child(std::make_shared<Debug::UI::CommandBar>());
	add_child(std::make_shared<Debug::UI::OutputPanel>());
	add_child(std::make_shared<Debug::UI::MainMenubar>());
}

void DebugUIService::on_frame_end() {
	if (!Application::instance()->is_rendering_enabled()) {
		return;
	}

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

static ImGuiKey toImGuiKey(const sf::Keyboard::Key key) {
	switch (key) {
		case sf::Keyboard::Key::Tab:
			return ImGuiKey_Tab;
		case sf::Keyboard::Key::Left:
			return ImGuiKey_LeftArrow;
		case sf::Keyboard::Key::Right:
			return ImGuiKey_RightArrow;
		case sf::Keyboard::Key::Up:
			return ImGuiKey_UpArrow;
		case sf::Keyboard::Key::Down:
			return ImGuiKey_DownArrow;
		case sf::Keyboard::Key::PageUp:
			return ImGuiKey_PageUp;
		case sf::Keyboard::Key::PageDown:
			return ImGuiKey_PageDown;
		case sf::Keyboard::Key::Home:
			return ImGuiKey_Home;
		case sf::Keyboard::Key::End:
			return ImGuiKey_End;
		case sf::Keyboard::Key::Insert:
			return ImGuiKey_Insert;
		case sf::Keyboard::Key::Delete:
			return ImGuiKey_Delete;
		case sf::Keyboard::Key::Backspace:
			return ImGuiKey_Backspace;
		case sf::Keyboard::Key::Space:
			return ImGuiKey_Space;
		case sf::Keyboard::Key::Enter:
			return ImGuiKey_Enter;
		case sf::Keyboard::Key::Escape:
			return ImGuiKey_Escape;
		case sf::Keyboard::Key::A:
			return ImGuiKey_A;
		case sf::Keyboard::Key::C:
			return ImGuiKey_C;
		case sf::Keyboard::Key::V:
			return ImGuiKey_V;
		case sf::Keyboard::Key::X:
			return ImGuiKey_X;
		case sf::Keyboard::Key::Y:
			return ImGuiKey_Y;
		case sf::Keyboard::Key::Z:
			return ImGuiKey_Z;
		default:
			return ImGuiKey_None;
	}
}

void DebugUIService::handle_event(
  const sf::Window *window, const sf::Event *event
) {
	if (!Application::instance()->is_rendering_enabled()) {
		return;
	}
	ImGuiIO &io = ImGui::GetIO();

	if (const auto *moved = event->getIf<sf::Event::MouseMoved>()) {
		if (moved->position.x > window->getSize().x || moved->position.x < 0) {
			return;
		}

		if (moved->position.y > window->getSize().y || moved->position.y < 0) {
			return;
		}

		io.MousePos = ImVec2(
		  static_cast<float>(moved->position.x),
		  static_cast<float>(moved->position.y)
		);
	} else if (const auto *mousePressed =
	             event->getIf<sf::Event::MouseButtonPressed>()) {
		if (mousePressed->button == sf::Mouse::Button::Left) {
			io.MouseDown[0] = true;
		} else if (mousePressed->button == sf::Mouse::Button::Right) {
			io.MouseDown[1] = true;
		} else if (mousePressed->button == sf::Mouse::Button::Middle) {
			io.MouseDown[2] = true;
		}
	} else if (const auto *mouseReleased =
	             event->getIf<sf::Event::MouseButtonReleased>()) {
		if (mouseReleased->button == sf::Mouse::Button::Left) {
			io.MouseDown[0] = false;
		} else if (mouseReleased->button == sf::Mouse::Button::Right) {
			io.MouseDown[1] = false;
		} else if (mouseReleased->button == sf::Mouse::Button::Middle) {
			io.MouseDown[2] = false;
		}
	} else if (const auto *textEntered = event->getIf<sf::Event::TextEntered>()) {
		if (textEntered->unicode != 0x10000) {
			io.AddInputCharacter(static_cast<unsigned short>(textEntered->unicode));
		}
	} else if (const auto *resized = event->getIf<sf::Event::Resized>()) {
		io.DisplaySize = ImVec2(
		  static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)
		);
	} else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

		ImGuiKey imguiKey = toImGuiKey(keyPressed->code);
		if (imguiKey != ImGuiKey_None) {
			io.AddKeyEvent(imguiKey, true);
		}

		// Modifiers
		io.AddKeyEvent(ImGuiKey_LeftCtrl, keyPressed->control);
		io.AddKeyEvent(ImGuiKey_LeftShift, keyPressed->shift);
		io.AddKeyEvent(ImGuiKey_LeftAlt, keyPressed->alt);
		io.AddKeyEvent(ImGuiKey_LeftSuper, keyPressed->system);

	} else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		ImGuiKey imguiKey = toImGuiKey(keyReleased->code);
		if (imguiKey != ImGuiKey_None) {
			io.AddKeyEvent(imguiKey, false);
		}

		// Modifiers
		io.AddKeyEvent(ImGuiKey_LeftCtrl, keyReleased->control);
		io.AddKeyEvent(ImGuiKey_LeftShift, keyReleased->shift);
		io.AddKeyEvent(ImGuiKey_LeftAlt, keyReleased->alt);
		io.AddKeyEvent(ImGuiKey_LeftSuper, keyReleased->system);
	}
}