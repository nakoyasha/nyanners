#include "ReflectionService.h"

#include <ranges>

#include "Application.h"
#include "EngineService.h"
#include "IOService.h"
#include "UIService.h"
#include "core/Logger.h"
#include "instances/DataModel.h"
#include "instances/drawable/TextLabel.h"
#include "lualib.h"

using namespace Nyanners::Services;
std::map<std::string, ReflectionClass> ReflectionService::classes;

ReflectionInstance* ReflectionService::get_instance_from_context(
    lua_State* context, const int id)
{
    auto* instance = static_cast<ReflectionInstance*>(
        lua_touserdatatagged(context, id, LUA_SCRIPT_INSTANCE_TAG));

    return instance;
}

void ReflectionService::reflect_class(
    lua_State* context, const std::shared_ptr<Instance>& instance)
{
    const auto descriptor = classes.find(instance->baseName);

    if (descriptor == classes.end())
        throw std::runtime_error(std::format(
            "Class {} is missing a Reflection descriptor", instance->baseName));

    auto* selfUser = static_cast<ReflectionInstance*>(lua_newuserdatatagged(
        context, sizeof(ReflectionInstance), LUA_SCRIPT_INSTANCE_TAG));
    new (selfUser) ReflectionInstance {
        .pointer = instance,
        .descriptor = &descriptor->second,
    };

    if (luaL_newmetatable(context, "instance")) {
        luaL_Reg sRegs[] = {
            { "__index",
                [](lua_State* context) {
                    auto* instance = get_instance_from_context(context, 1);
                    if (instance == nullptr) {
                        throw std::runtime_error(
                            "Instance userdata is null or invalid userdata passed");
                    }

                    return instance_index(context, instance);
                } },
            { "__newindex",
                [](lua_State* context) {
                    auto* instance = get_instance_from_context(context, 1);
                    if (instance == nullptr) {
                        throw std::runtime_error(
                            "Instance userdata is null or invalid userdata passed");
                    }

                    return instance_new_index(context, instance);
                } },
            {
                "__tostring",
                [](lua_State* context) {
                    lua_pushstring(context, "Instance");
                    return 1;
                },
            },
            { nullptr, nullptr }
        };

        luaL_register(context, nullptr, sRegs);
    }

    lua_setreadonly(context, -1, true);
    lua_setmetatable(context, -2);
}

ReflectionClass ReflectionService::create_reflection(
    const ReflectionClass& descriptor)
{
    auto existingClass = classes.find(descriptor.className);

    if (existingClass != classes.end()) {
        return existingClass->second;
    }

    classes.insert(std::make_pair(descriptor.className, descriptor));

    return descriptor;
}

int ReflectionService::instance_index(lua_State* context,
    const ReflectionInstance* instance)
{
    const std::string propertyName = luaL_checkstring(context, -1);

    if (instance == nullptr) {
        throw std::runtime_error("Instance userdata is null");
    }

    if (instance->pointer == nullptr) {
        throw std::runtime_error("Instance pointer is null");
    }

    if (propertyName == "Name") {
        lua_pushstring(context, instance->pointer->name.c_str());
        return 1;
    }
    if (propertyName == "ClassName") {
        lua_pushstring(context, instance->pointer->baseName.c_str());
        return 1;
    }

    if (propertyName == "Parent") {
        if (instance->pointer->parent == nullptr) {
            lua_pushnil(context);
            return 1;
        }

        reflect_class(context, instance->pointer->parent);
        return 1;
    }

    if (propertyName == "find_first_child") {
        lua_pushcfunction(
            context,
            [](lua_State* context) {
                const auto* instance = get_instance_from_context(context, 1);
                if (instance == nullptr) {
                    throw std::runtime_error(
                        "Instance userdata is null or invalid userdata passed");
                }

                const std::string childName = luaL_checkstring(context, -1);
                const auto child = instance->pointer->find_first_child<Instance>(childName);

                if (child == nullptr) {
                    lua_pushnil(context);
                    return 1;
                }

                reflect_class(context, child);
                return 1;
            },
            "find_first_child");

        return 1;
    }

    for (const auto& property : instance->descriptor->properties) {
        if (property.name == propertyName) {
            return property.get(instance->pointer.get(), context);
        }
    }
    if (auto child = instance->pointer->find_first_child<Instance>(propertyName)) {
        reflect_class(context, child);
        return 1;
    } else {
        luaL_error(context,
            std::format("{}::{} is an invalid property and or child",
                instance->pointer->baseName, propertyName)
                .c_str());
        return 0;
    }

}

int ReflectionService::instance_new_index(lua_State* context,
    const ReflectionInstance* instance)
{
    const std::string propertyName = luaL_checkstring(context, -2);

    if (instance == nullptr) {
        throw std::runtime_error("Instance userdata is null");
    }

    if (instance->pointer == nullptr) {
        throw std::runtime_error("Instance pointer is null");
    }

    for (const auto& property : instance->descriptor->properties) {
        if (propertyName == "Name") {
            std::string newValue = luaL_checkstring(context, -1);
            instance->pointer->name = newValue;
            return 0;
        }
        if (propertyName == "ClassName") {
            luaL_error(context, "Cannot modify a read-only property");
            return 0;
        }
        if (propertyName == "Parent") {
            auto* newParent = get_instance_from_context(context, -1);

            if (newParent == nullptr) {
                if (instance->pointer->parent != nullptr) {
                    instance->pointer->parent->remove_child(
                        instance->pointer->shared_from_this());
                }
            } else {
                newParent->pointer->add_child(instance->pointer->shared_from_this());
            }

            return 0;
        }
        if (property.name == propertyName) {
            if (property.readOnly == true) {
                luaL_error(context, "Cannot modify a read-only property");
                return 0;
            }

            property.set(instance->pointer.get(), context);
            return 0;
        }

        luaL_error(context, std::format("{}::{} is an invalid property", instance->pointer->baseName, propertyName).c_str());
        return 0;
    }

    return 0;
}

void ReflectionService::register_reflections()
{
    create_reflection(
        { .className = "DataModel",
            .base = "Instance",
            .isService = true,
            .constructor =
                []() {
                    throw std::runtime_error(
                        "You can't make a DataModel, as it's a singleton.");
                    return nullptr;
                },
            .properties = { {
                .name = "get_service",
                .type = ReflectionPropertyType::Method,
                .get =
                    [](const Instance* instance, lua_State* context) {
                        lua_pushcfunction(
                            context,
                            [](lua_State* context) {
                                const std::string service = luaL_checkstring(context, -1);
                                const auto foundService = Application::instance()
                                                              ->currentModel->get_service<Instance>(service);

                                if (foundService == nullptr) {
                                    luaL_error(context,
                                        "Cannot create non-existent service");
                                    return 0;
                                }

                                Services::ReflectionService::reflect_class(context,
                                    foundService);
                                return 1;
                            },
                            "get_service");
                        return 1;
                    },
            } } });

    create_reflection(
        { .className = "UIService",
            .base = "Instance",
            .isService = true,
            .constructor = []() { return std::make_shared<UIService>(); },
            .properties = { } });

    create_reflection(
        { .className = "IOService",
            .base = "Instance",
            .isService = true,
            .constructor = []() { return std::make_shared<IOService>(); },
            .properties = { {
                .name = "read_file",
                .type = ReflectionPropertyType::Method,
                .get =
                    [](const Instance* instance, lua_State* context) {
                        lua_pushcfunction(
                            context,
                            [](lua_State* context) {
                                const std::string path = luaL_checkstring(context, -1);

                                try {
                                    const std::string result = Services::IOService::read_file(path);
                                    lua_pushstring(context, result.c_str());
                                    return 1;
                                } catch (std::runtime_error& e) {
                                    Core::Logger::log(e.what());
                                    luaL_error(context, e.what());
                                }

                                return 1;
                            },
                            "read_file");
                        return 1;
                    },
            } } });

    create_reflection(
        { .className = "EngineService",
            .isService = true,
            .constructor = []() { return std::make_shared<EngineService>(); },
            .properties = { {
                .name = "panic",
                .type = ReflectionPropertyType::Method,
                .get =
                    [](const Instance* instance, lua_State* context) {
                        lua_pushcfunction(
                            context,
                            [](lua_State* context) {
                                const std::string message = luaL_checkstring(context, -1);
                                EngineService::panic(message);
                                return 0;
                            },
                            "panic");
                        return 1;
                    },
            } } });
    create_reflection(
        { .className = "TextLabel",
            .isService = false,
            .properties = {
                { .name = "Text",
                    .type = ReflectionPropertyType::String,
                    .get =
                        [](const Instance* instance, lua_State* context) {
                            const auto* label = static_cast<const Instances::TextLabel*>(instance);
                            lua_pushstring(context, label->getText().c_str());

                            return 1;
                        },
                    .set =
                        [](Instance* instance, lua_State* context) {
                            auto* label = static_cast<Instances::TextLabel*>(instance);
                            const std::string text = luaL_checkstring(context, -1);

                            label->setText(text);
                        } } } });
}
