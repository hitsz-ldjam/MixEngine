#pragma once
#ifndef MX_RESOURCE_LOADER_H_
#define MX_RESOURCE_LOADER_H_

#include "MxResourceParserBase.hpp"
#include "MxParserRegister.hpp"
#include "../Engine/MxModuleBase.h"

namespace Mix {
	namespace Resource {
		class ResourceBase;

		class ResourceLoader : public ModuleBase {
		public:
			/**
			 * \brief Initialize the Resources
			 */
			void init();


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
			std::shared_ptr<ResourceBase> load(const std::string& _file) const;

			template<typename _Ty>
			std::shared_ptr<_Ty> load(const std::string& _file) const;

			/**
			* \brief Load file from disk, parse it using loader that supports specified type
			* \param _file The path of the file
			* \param _type
			* \return The object referring to the resource
			*/
			std::shared_ptr<ResourceBase> load(const std::string& _file, const ResourceType _type);

			template<typename _Ty>
			std::shared_ptr<_Ty> load(const std::string& _file, const ResourceType _type);

		private:
			std::shared_ptr<ParserRegister> mLoaderRegister;
		};

		template <typename _Ty>
		std::shared_ptr<_Ty> ResourceLoader::load(const std::string& _file) const {
			return std::dynamic_pointer_cast<_Ty>(load(_file));
		}

		template <typename _Ty>
		std::shared_ptr<_Ty> ResourceLoader::load(const std::string& _file, const ResourceType _type) {
			return std::dynamic_pointer_cast<_Ty>(load(_file, _type));
		}
	}
}

#endif
