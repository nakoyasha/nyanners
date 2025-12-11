#include "UIDrawable.h"
#include "LayerCollector.h"

#include "lua/reflection/Reflection.h"
#include "core/Logger.h"

using namespace Nyanners::Instances;

void UIDrawable::setPosition(const float x, const float y)  {
	position.setX(x);
	position.setY(y);

	renderingRectangle.x = position.absoluteX;
	renderingRectangle.y = position.absoluteY;
}

void UIDrawable::setSize(const float x, const float y) {
	size.setX(x);
	size.setY(y);

	renderingRectangle.width = size.absoluteX;
	renderingRectangle.height = size.absoluteY;
}

void UIDrawable::update() {
	position.recomputeSize();
	size.recomputeSize();

	renderingRectangle.width = size.absoluteX;
	renderingRectangle.height = size.absoluteY;
};

int UIDrawable::luaNewIndex(lua_State *context, std::string keyName, bool keyValue)  {
	if (keyName == "Visible") {
		this->m_visible = keyValue;
		return 0;
	}

	return Instance::luaNewIndex(context, keyName, keyValue);
};

int UIDrawable::luaNewIndex(lua_State *context, std::string keyName, float keyValue) {
	if (keyName == "ZIndex") {
		// i'm pretty sure
		// casting here breaks the integer
		setZIndex(keyValue);
		return 0;
	}

	return Instance::luaNewIndex(context, keyName, keyValue);
}

int UIDrawable::luaNewIndex(lua_State *context, std::string keyName, Vector2 keyValue) {
	if (keyName == "Position") {
		this->setPosition(keyValue.x, keyValue.y);
		return 0;
	} else if (keyName == "Size") {
		this->setSize(keyValue.x, keyValue.y);
		return 0;
	}
	else {
		return Instance::luaNewIndex(context, keyName, keyValue);
	}
}

int UIDrawable::luaIndex(lua_State *context, const std::string keyName) {
	if (keyName == "Size") {
		reflection_luaPushStruct(context, {
				{"X", LuaValue(size.absoluteX)},
				{"Y", LuaValue(size.absoluteY)}
		});
		return 1;
	} else if (keyName == "Position") {
		reflection_luaPushStruct(context, {
				{"X", LuaValue(position.absoluteX)},
				{"Y", LuaValue(position.absoluteY)}
		});
		return 1;
	}

	return Instance::luaIndex(context, keyName);
}

bool UIDrawable::isUI() {
	Logger::log("Calling UIDrawable::isUI()");
	return true;
}

UIDrawable::UIDrawable(std::string className) : Instance(className)
{
	this->properties.insert({"ZIndex",
	{Reflection::ReflectionPropertyType::Number, &zIndex}
	});

	this->m_className = className;
	this->m_name = "Instance";
}

void UIDrawable::setZIndex(const int zIndex) {
	this->zIndex = zIndex;

	if (m_parent != nullptr) {
		if (auto* collector = dynamic_cast<UI::LayerCollector*>(this->m_parent)) {
			Logger::log("UIDrawable::setZIndex: telling parent to resort");
			collector->resortVector();
		} else {
			Logger::log("UIDrawable::setZIndex: uh oh! could not find LayerCollector, so ZIndex will not be applied. That's bad.");
		}
	} else {
		Logger::log("UIDrawable::setZIndex: parent is null, can't even TRY to find a LayerCollector. Sigh...");
	}
}
