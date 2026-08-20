#include "EngineService.h"

extern "C" {
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#define MA_ENABLE_DECODERS
#define MA_ENABLE_ENCODERS
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
}
#include "ReflectionService.h"
#include "SoundService.h"
#include "lualib.h"

using namespace Nyanners::Services;
namespace Nyanners::Scripting {
	static auto soundDescriptor =
	  ReflectionDescriptorRegistry::instance()->create_registrator([]() {
		  ReflectionService::create_descriptor("SoundService", {"Instance"})
		    .add_property_chained<
		      SoundService,
		      double,
		      &SoundService::get_volume,
		      &SoundService::set_volume>("GlobalVolume", Number)
		    .add_method_anon(
		      "play_sound",
		      [](Instances::Object *instance, lua_State *context) -> int {
			      const auto soundName = luaL_checkstring(context, -1);
			      const auto soundService = static_cast<SoundService *>(instance);

			      soundService->play_sound(soundName);
			      return 0;
		      },
		      Null
		    );
	  });
}

SoundService::SoundService() : Instance("SoundService") {
	if (
	  const auto result = ma_engine_init(nullptr, &soundEngine);
	  result != MA_SUCCESS
	) {
		EngineService::panic(
		  std::format(
		    "{}, {}",
		    "SoundService initialization failed!",
		    ma_result_to_string(result)
		  )
		);
	}
}

void SoundService::play_sound(const std::string &path) {
	if (
	  const auto result =
	    ma_engine_play_sound(&soundEngine, path.c_str(), nullptr);
	  result != MA_SUCCESS
	) {
		Core::Logger::log_error(
		  std::format(
		    "Failed playing sound {}: {}", path, ma_result_to_string(result)
		  )
		);
	};
}

ma_sound *SoundService::create_handle(const std::string &path) {
	auto *sound = new ma_sound();

	if (
	  const auto result = ma_sound_init_from_file(
	    &soundEngine, path.c_str(), 0, nullptr, nullptr, sound
	  );
	  result != MA_SUCCESS
	) {
		Core::Logger::log_debug(
		  std::format(
		    "Failed loading sound {}: {}", path, ma_result_to_string(result)
		  )
		);
		return nullptr;
	}

	return sound;
}

std::string SoundService::ma_result_to_string(const ma_result &result) {
	// NOTICE OF AI-GENERATED CODE: Filling this in manually would be a massive chore.
	switch (result) {
		case MA_SUCCESS:
			return "Success";
		case MA_ERROR:
			return "Generic error";
		case MA_INVALID_ARGS:
			return "Invalid arguments";
		case MA_INVALID_OPERATION:
			return "Invalid operation";
		case MA_OUT_OF_MEMORY:
			return "Out of memory";
		case MA_OUT_OF_RANGE:
			return "Out of range";
		case MA_ACCESS_DENIED:
			return "Access denied";
		case MA_DOES_NOT_EXIST:
			return "Does not exist";
		case MA_ALREADY_EXISTS:
			return "Already exists";
		case MA_TOO_MANY_OPEN_FILES:
			return "Too many open files";
		case MA_INVALID_FILE:
			return "Invalid file";
		case MA_TOO_BIG:
			return "File too large";
		case MA_PATH_TOO_LONG:
			return "Path too long";
		case MA_NAME_TOO_LONG:
			return "Name too long";
		case MA_NOT_DIRECTORY:
			return "Not a directory";
		case MA_IS_DIRECTORY:
			return "Is a directory";
		case MA_DIRECTORY_NOT_EMPTY:
			return "Directory not empty";
		case MA_AT_END:
			return "End of file";
		case MA_NO_SPACE:
			return "No space available";
		case MA_BUSY:
			return "Resource busy";
		case MA_IO_ERROR:
			return "I/O error";
		case MA_INTERRUPT:
			return "Interrupted";
		case MA_UNAVAILABLE:
			return "Unavailable";
		case MA_ALREADY_IN_USE:
			return "Already in use";
		case MA_BAD_ADDRESS:
			return "Bad address";
		case MA_BAD_SEEK:
			return "Bad seek";
		case MA_BAD_PIPE:
			return "Bad pipe";
		case MA_DEADLOCK:
			return "Deadlock";
		case MA_TOO_MANY_LINKS:
			return "Too many links";
		case MA_NOT_IMPLEMENTED:
			return "Not implemented";
		case MA_NO_MESSAGE:
			return "No message";
		case MA_BAD_MESSAGE:
			return "Bad message";
		case MA_NO_DATA_AVAILABLE:
			return "No data available";
		case MA_INVALID_DATA:
			return "Invalid data";
		case MA_TIMEOUT:
			return "Timeout";
		case MA_NO_NETWORK:
			return "No network";
		case MA_NOT_UNIQUE:
			return "Not unique";
		case MA_NOT_SOCKET:
			return "Not a socket";
		case MA_NO_ADDRESS:
			return "No address";
		case MA_BAD_PROTOCOL:
			return "Bad protocol";
		case MA_PROTOCOL_UNAVAILABLE:
			return "Protocol unavailable";
		case MA_PROTOCOL_NOT_SUPPORTED:
			return "Protocol not supported";
		case MA_PROTOCOL_FAMILY_NOT_SUPPORTED:
			return "Protocol family not supported";
		case MA_ADDRESS_FAMILY_NOT_SUPPORTED:
			return "Address family not supported";
		case MA_SOCKET_NOT_SUPPORTED:
			return "Socket not supported";
		case MA_CONNECTION_RESET:
			return "Connection reset";
		case MA_ALREADY_CONNECTED:
			return "Already connected";
		case MA_NOT_CONNECTED:
			return "Not connected";
		case MA_CONNECTION_REFUSED:
			return "Connection refused";
		case MA_NO_HOST:
			return "Host not found";
		case MA_IN_PROGRESS:
			return "Operation in progress";
		case MA_CANCELLED:
			return "Cancelled";
		case MA_MEMORY_ALREADY_MAPPED:
			return "Memory already mapped";

		case MA_CRC_MISMATCH:
			return "CRC mismatch";

		case MA_FORMAT_NOT_SUPPORTED:
			return "Format not supported";
		case MA_DEVICE_TYPE_NOT_SUPPORTED:
			return "Device type not supported";
		case MA_SHARE_MODE_NOT_SUPPORTED:
			return "Share mode not supported";
		case MA_NO_BACKEND:
			return "No backend available";
		case MA_NO_DEVICE:
			return "No device available";
		case MA_API_NOT_FOUND:
			return "API not found";
		case MA_INVALID_DEVICE_CONFIG:
			return "Invalid device configuration";
		case MA_LOOP:
			return "Loop detected";
		case MA_BACKEND_NOT_ENABLED:
			return "Backend not enabled";

		case MA_DEVICE_NOT_INITIALIZED:
			return "Device not initialized";
		case MA_DEVICE_ALREADY_INITIALIZED:
			return "Device already initialized";
		case MA_DEVICE_NOT_STARTED:
			return "Device not started";
		case MA_DEVICE_NOT_STOPPED:
			return "Device not stopped";

		case MA_FAILED_TO_INIT_BACKEND:
			return "Failed to initialize backend";
		case MA_FAILED_TO_OPEN_BACKEND_DEVICE:
			return "Failed to open backend device";
		case MA_FAILED_TO_START_BACKEND_DEVICE:
			return "Failed to start backend device";
		case MA_FAILED_TO_STOP_BACKEND_DEVICE:
			return "Failed to stop backend device";

		default:
			return std::format(
			  "Unknown miniaudio error ({})", static_cast<int>(result)
			);
	}
}

void SoundService::set_volume(const double newVolume) {
	this->volume = newVolume;
	ma_engine_set_volume(&soundEngine, newVolume);
}

SoundService::~SoundService() {
	ma_engine_uninit(&soundEngine);
}
