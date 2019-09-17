#pragma once
#ifndef MX_RESOURCE_LOADER_H_
#define MX_RESOURCE_LOADER_H_

#include "MxResourceParserBase.hpp"
#include "MxParserRegister.hpp"
#include "../Engine/MxModuleBase.h"

namespace Mix {
	class ResourceBase;

	class ResourceLoader : public ModuleBase {
	public:
		static ResourceLoader* Get();

		/**
		 * \brief Initialize the Resources
		 */
		void load() override;

		void init() override {}


		/**
		 * \brief Return the ParserRegister
		 */
		std::weak_ptr<ParserRegister> getLoaderRegister() const {
			return mLoaderRegister;
		}

		/**
		* \brief Load file from disk
		* \note This function will infer the file type from the extension of the file
		*       and will return null shared_ptr when there is no extension.
		* \param _file The path of the file
		* \return The object referring to the resource
		*/
		std::shared_ptr<ResourceBase> load(const std::string& _file, void* _additionalParam = nullptr) const;

		template<typename _Ty>
		std::shared_ptr<_Ty> load(const std::string& _file, void* _additionalParam = nullptr) const;

		/**
		* \brief Load file from disk, parse it using loader that supports specified type
		* \param _file The path of the file
		* \param _type
		* \return The object referring to the resource
		*/
		std::shared_ptr<ResourceBase> load(const std::string& _file, const ResourceType _type, void* _additionalParam = nullptr);

		template<typename _Ty>
		std::shared_ptr<_Ty> load(const std::string& _file, const ResourceType _type, void* _additionalParam = nullptr);

	private:
		std::shared_ptr<ParserRegister> mLoaderRegister;
	};

	template <typename _Ty>
	std::shared_ptr<_Ty> ResourceLoader::load(const std::string& _file, void* _additionalParam) const {
		return std::dynamic_pointer_cast<_Ty>(load(_file, _additionalParam));
	}

	template <typename _Ty>
	std::shared_ptr<_Ty> ResourceLoader::load(const std::string& _file, const ResourceType _type, void* _additionalParam) {
		return std::dynamic_pointer_cast<_Ty>(load(_file, _type, _additionalParam));
	}
}

#endif
