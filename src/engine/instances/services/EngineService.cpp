#include "EngineService.h"
#include "core/Logger.h"

#include <source_location>
#include <format>

#include "ReflectionService.h"
#include "RenderingService.h"
#include "scripting/reflections/ReflectionDescriptorRegistry.h"

#if defined(_WIN32) || defined(_WIN64)
	#define WIN32_MEAN_AND_LEAN;
	#include <windows.h>
	#define STRICT_TYPED_ITEMIDS
	#include <shobjidl.h>
	#include <wrl/client.h>
    using Microsoft::WRL::ComPtr;
#elif defined(__APPLE__) || defined(__MACH__)
#elif defined(__linux__)
#endif

using namespace Nyanners::Services;

namespace Nyanners::Scripting {
	static auto engineDescriptor = Reflection::ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		ReflectionService::register_enum("EnginePlatform", {
		  {"Unknown", static_cast<int>(Core::EnginePlatform::Unknown)},
		  {"Windows", static_cast<int>(Core::EnginePlatform::Windows)},
		  {"Linux", static_cast<int>(Core::EnginePlatform::Linux)},
		  {"Mac", static_cast<int>(Core::EnginePlatform::Mac)},
		  {"Android", static_cast<int>(Core::EnginePlatform::Android)}
		});
		ReflectionService::create_descriptor("EngineService", {"Instance"}, {ReflectionInstanceFlags::Service, ReflectionInstanceFlags::NotSerializable})
        .add_property_chained<EngineService, std::string, &EngineService::get_version>("EngineVersion", String, {ReflectionPropertyFlags::NotSerializable})
            .add_property_chained<EngineService, std::string, &EngineService::get_branch>("Branch", String, {ReflectionPropertyFlags::NotSerializable})
            .add_property_chained<EngineService, std::string, &EngineService::get_build_time>("BuildTime", String, {ReflectionPropertyFlags::NotSerializable})
		.add_method<&EngineService::native_data_test>("native_data_test", UserData, {})
		.add_enum_property_chained<EngineService, Core::EnginePlatform, &EngineService::get_platform>(
            "Platform", "EnginePlatform", {ReflectionPropertyFlags::NotSerializable}
		);
	});
}

Nyanners::Core::EngineInfo EngineService::engineInfo;
Nyanners::Core::EnginePlatform EngineService::platform;

EngineService::EngineService(): Instance("EngineService") {
	engineInfo.version = "0.4.0";
	engineInfo.branch = GIT_BRANCH;
	engineInfo.buildTime = __TIMESTAMP__;
#if defined(_WIN32) || defined(_WIN64)
	platform = Core::EnginePlatform::Windows;
#elif defined(__APPLE__) || defined(__MACH__)
	platform = Core::EnginePlatform::Mac;
#elif defined(__linux__)
	platform = Core::EnginePlatform::Linux;
#else
	platform = Core::EnginePlatform::Unknown;
#endif
}

[[noreturn]]
void EngineService::panic(const std::string_view &panicMessage) {
  const auto& location = std::source_location::current();
  Core::Logger::log(std::format("PANIC! From {}\n {}", location.file_name(), panicMessage));

	#ifdef NDEBUG
    std::terminate();
	#else
	__builtin_trap();
	#endif
}

Nyanners::Instances::Signal<Nyanners::DataTypes::Vector2> EngineService::onWindowResized;
Nyanners::Instances::Signal<const sf::Event*> EngineService::onInternalEvent;

void EngineService::handle_event(const sf::Event *event) {
  onInternalEvent.fire(event);

  if (const auto *resizedEvent = event->getIf<sf::Event::Resized>()) {
    onWindowResized.fire({resizedEvent->size.x, resizedEvent->size.y});
  }
}

void EngineService::open_url(const std::string &url) {
#if defined(_WIN32) || defined(_WIN64)
	ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__) || defined(__MACH__)
	throw std::runtime_error("Unimplemented for current platform");
#elif defined(__linux__)
	throw std::runtime_error("Unimplemented for current platform");
#endif
}

std::filesystem::path EngineService::prompt_save_file(const std::filesystem::path& startPath, const std::string fileType) {
#if defined(_WIN32) || defined(_WIN64)
	// NOTICE OF AI-GENERATED CODE:
	// I'm sorry but the Win32 API is a genuine fucking mess.
	// I don't know how a person is meant to stay sane trying to work with it.
	// The MS example for the modern save file dialog literally has >100 lines of nested SUCCEEDED
	// checks.

	constexpr COMDLG_FILTERSPEC saveTypes[] =
	{
		{L"Scene File",       L"*.nscene.json"},
	};

    const HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool shouldUninitialize =
        SUCCEEDED(coInit);

    if (FAILED(coInit) && coInit != RPC_E_CHANGED_MODE) {
        throw std::runtime_error("CoInitializeEx failed");
    }

    ComPtr<IFileSaveDialog> dialog;

    HRESULT hr = CoCreateInstance(
        CLSID_FileSaveDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dialog)
    );

    if (FAILED(hr)) {
        if (shouldUninitialize)
            CoUninitialize();

        throw std::runtime_error("Failed to create Windows save dialog");
    }

    // Native save-dialog behavior.
    DWORD options = 0;
    if (SUCCEEDED(dialog->GetOptions(&options))) {
        options |= FOS_FORCEFILESYSTEM;
        options |= FOS_OVERWRITEPROMPT;
        options |= FOS_PATHMUSTEXIST;

        dialog->SetOptions(options);
    }

    dialog->SetTitle(L"Save File");
	dialog->SetFileTypes(std::size(saveTypes), saveTypes);
    dialog->SetOkButtonLabel(L"Save");

    // Interpret startPath as either:
    //   C:\foo\bar.ext  -> initial folder C:\foo, filename bar.ext
    //   C:\foo\         -> initial folder C:\foo
    std::filesystem::path folder;
    std::filesystem::path filename;

    std::error_code ec;

    if (std::filesystem::is_directory(startPath, ec)) {
        folder = startPath;
    } else {
        folder = startPath.parent_path();
        filename = "";
    }

    // Set the initial folder.
    if (!folder.empty()) {
        ComPtr<IShellItem> folderItem;

        hr = SHCreateItemFromParsingName(
            folder.wstring().c_str(),
            nullptr,
            IID_PPV_ARGS(&folderItem)
        );

        if (SUCCEEDED(hr)) {
            dialog->SetFolder(folderItem.Get());
        }
    }

    // Set the initial filename.
    if (!filename.empty()) {
        dialog->SetFileName(filename.wstring().c_str());
    }

    // Generic "All files" filter.
    const COMDLG_FILTERSPEC filters[] = {
        { L"All files", L"*.*" }
    };

    dialog->SetFileTypes(std::size(filters),filters);
    dialog->SetFileTypeIndex(1);

    hr = dialog->Show(nullptr);

    // User pressed Cancel / closed the dialog.
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        if (shouldUninitialize)
            CoUninitialize();

        return {};
    }

    if (FAILED(hr)) {
        if (shouldUninitialize)
            CoUninitialize();

        throw std::runtime_error("Windows save dialog failed");
    }

    ComPtr<IShellItem> result;
    hr = dialog->GetResult(&result);

    if (FAILED(hr)) {
        if (shouldUninitialize)
            CoUninitialize();

        throw std::runtime_error("Failed to get selected save path");
    }

    PWSTR path = nullptr;
    hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

    if (FAILED(hr) || path == nullptr) {
        if (shouldUninitialize)
            CoUninitialize();

        throw std::runtime_error("Failed to get selected file path");
    }

    std::filesystem::path resultPath(path);
    CoTaskMemFree(path);

    if (shouldUninitialize)
        CoUninitialize();

    return resultPath;
#elif defined(__APPLE__) || defined(__MACH__)
    throw std::runtime_error("Unimplemented for current platform");
#elif defined(__linux__)
    throw std::runtime_error("Unimplemented for current platform");
#else
    throw std::runtime_error("Unsupported platform");
#endif
}
