#pragma once
#ifndef MX_PARSER_REGISTER_H_
#define MX_PARSER_REGISTER_H_

#include <memory>
#include "MxResourceParserBase.hpp"

namespace Mix {
		class ParserRegister {
		public:
			ParserRegister() = default;

			/**
			 * \brief Register a resource parser
			 * \param _parser the parser to be registered
			 * \note The same parser will only be registered once
			 */
			void registerParser(const std::shared_ptr<ResourceParserBase>& _parser) {
				mLoaders.insert(_parser);
			}

			/**
			 * \brief Remove a registered resource parser
			 * \param _parser the parser to be removed
			 */
			void removeParser(const std::shared_ptr<ResourceParserBase>& _parser) {
				mLoaders.erase(_parser);
			}


			/**
			 * \brief Find a loader that supports the specified type
			 */
			std::shared_ptr<ResourceParserBase> findLoaderByType(const ResourceType _type) {
				for (auto& loader : mLoaders) {
					if (loader->isSupport(_type))
						return loader;
				}
				return nullptr;
			}

			/**
			 * \brief Find a loader that supports the specified extension
			 */
			std::shared_ptr<ResourceParserBase> findLoaderByExt(const std::string& _ext) {
				for (auto& loader : mLoaders) {
					if (loader->isSupport(_ext))
						return loader;
				}
				return nullptr;
			}

		private:
			std::unordered_set<std::shared_ptr<ResourceParserBase>> mLoaders;

		};

}
#endif
