#pragma once
#ifndef MX_MODULE_HOLDER_H_
#define MX_MODULE_HOLDER_H_

#include "../Utils/MxGeneralBase.hpp"
#include <typeindex>
#include <unordered_map>
#include "MxModuleBase.h"
#include "../Log/MxLog.h"

namespace Mix {
	class ModuleHolder : GeneralBase::NoCopyBase {
	public:
		template<typename _Ty>
		bool has() const {
			static_assert(std::is_base_of<ModuleBase, _Ty>::value, "A Module must be a derived class of ModuleBase");

			return mModules.count(std::type_index(typeid(_Ty)));
		}

		template<typename _Ty>
		_Ty* get() const {
			static_assert(std::is_base_of<ModuleBase, _Ty>::value, "A Module must be a derived class of ModuleBase");

			auto it = mModules.find(std::type_index(typeid(_Ty)));
			if (it != mModules.end() && it->second)
				return static_cast<_Ty*>(it->second.get());
			else {
				Debug::Log::Error("No requested Module");
				return nullptr;
			}
		}

		template<typename _Ty, typename... _Args>
		_Ty* add(_Args&&... _args) {
			static_assert(std::is_base_of<ModuleBase, _Ty>::value, "A Module must be a derived class of ModuleBase");

			mModules.emplace(std::type_index(typeid(_Ty)), std::make_unique<_Ty>(std::forward<_Args>(_args)...));
			mAddOrder.push_back(std::type_index(typeid(_Ty)));
			return dynamic_cast<_Ty*>(mModules[std::type_index(typeid(_Ty))].get());
		}

		template<typename _Ty>
		void remove() {
			static_assert(std::is_base_of<ModuleBase, _Ty>::value, "A Module must be a derived class of ModuleBase");

			mModules.erase(std::type_index(typeid(_Ty)));
			auto it = std::find(mAddOrder.begin(), mAddOrder.end(), std::type_index(typeid(_Ty)));
			mAddOrder.erase(it);
		}

		std::vector<ModuleBase*> getAllOrdered() const;

		~ModuleHolder();

	private:
		std::vector<std::type_index> mAddOrder;
		std::unordered_map<std::type_index, std::unique_ptr<ModuleBase>> mModules;
	};

	inline std::vector<ModuleBase*> ModuleHolder::getAllOrdered() const {
		std::vector<ModuleBase*> result;
		for (auto index : mAddOrder)
			result.push_back(mModules.at(index).get());
		return result;
	}

	inline ModuleHolder::~ModuleHolder() {
		while (!mAddOrder.empty()) {
			mModules.erase(mAddOrder.back());
			mAddOrder.pop_back();
		}
	}
}

#endif
