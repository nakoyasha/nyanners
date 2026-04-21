#include "TextService.h"
#include "core/Logger.h"

using namespace Nyanners::Services;

TextService::TextService() : Instance("TextService") {
	if (FT_Init_FreeType(&ft))
	{
		Core::Logger::log("Failed to initialize FreeType");
	}
}