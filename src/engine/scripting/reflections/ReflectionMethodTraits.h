#pragma once
#include "ReflectionValueIO.h"
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "instances/services/ScriptService.h"

// NOTICE OF AI-GENERATED CODE: yeah this was just lazily thrown in lmao.
// i don't particularly have the energy for this kind of c++ yet
namespace Nyanners::Scripting::Reflection {
	template<typename T>
	struct ReflectionMemberFunctionTraits;

	template<typename Return, typename Object, typename... Args>
	struct ReflectionMemberFunctionTraits<Return (Object::*)(Args...)> {
		using object_type = Object;
		using return_type = Return;
		using arguments = std::tuple<Args...>;
	};

	template<typename Return, typename Object, typename... Args>
	struct ReflectionMemberFunctionTraits<Return (Object::*)(Args...) const>
			: ReflectionMemberFunctionTraits<Return (Object::*)(Args...)> {
	};

	template<auto method>
	struct ReflectionMethodInvoker {
		using traits = ReflectionMemberFunctionTraits<decltype(method)>;
		using ObjectType = typename traits::object_type;
		using ReturnType = typename traits::return_type;
		using Arguments = typename traits::arguments;

		template<size_t... indices>
		static int invoke_impl(
			Instances::Object *instance,
			lua_State *context,
			std::index_sequence<indices...>) {
			auto *object = dynamic_cast<ObjectType *>(instance);
			if (object == nullptr) {
				luaL_error(
					context, "method called on an object that is not *this* object, what???"
				);
				return 0;
			}


			if constexpr (std::is_void_v<ReturnType>) {
				Services::ScriptService::set_active_context(context);
				std::invoke(
					method,
					object,
					read_argument<
						std::remove_cvref_t<std::tuple_element_t<indices, Arguments> >
					>(context, static_cast<int>(indices) + 2)...
				);
				return 0;
			} else {
				Services::ScriptService::set_active_context(context);
				auto result = std::invoke(
					method,
					object,
					read_argument<
						std::remove_cvref_t<std::tuple_element_t<indices, Arguments> >
					>(context, static_cast<int>(indices) + 2)...
				);
				push_value(context, result);
				return 1;
			}
		}

		static int invoke(Instances::Object *instance, lua_State *context) {
			return invoke_impl(
				instance,
				context,
				std::make_index_sequence<std::tuple_size_v<Arguments> >{}
			);
		}
	};
}
