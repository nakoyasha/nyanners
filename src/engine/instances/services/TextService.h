#pragma once
#include "freetype/freetype.h"
#include "instances/Instance.h"

namespace Nyanners::Services {
	class TextService : public Instances::Instance {
	public:
		TextService();
		FT_Face load_glyph();
	private:
		FT_Library ft;
		std::vector<FT_Face> fonts;
	};
}