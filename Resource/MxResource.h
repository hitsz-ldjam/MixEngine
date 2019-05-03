#pragma once
#include "../Mx/MxObject.h"
#include "../MxVk/MxVkGraphics.h"
#include "MxResourceBase.h"
#include "MxResourceParser.h"
#include "../Utils/MxReferenceMgr.h"
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

namespace Mix {
    namespace Resource {
        class LoaderRegister {
        public:
            LoaderRegister() = default;

            /**
             * \brief Register a resource parser
             * \param _parser the parser to be registered
             * \note The same parser will only be registered once
             */
            void RegisterLoader(const std::shared_ptr<ResourceParserBase> _parser) {
                mLoaders.insert(_parser);
            }

            /**
             * \brief Remove a registered resource parser
             * \param _parser the parser to be removed
             */
            void RemoveLoader(const std::shared_ptr<ResourceParserBase> _parser) {
                mLoaders.erase(_parser);
            }


            /**
             * \brief Find a loader that supports the specified type
             */
            std::shared_ptr<ResourceParserBase> FindLoaderByType(const ResourceType _type);

            /**
             * \brief Find a loader that supports the specified extension
             */
            std::shared_ptr<ResourceParserBase> FindLoaderByExt(const std::string& _ext);

        private:
            std::unordered_set<std::shared_ptr<ResourceParserBase>> mLoaders;

        };

        class Resources {
        public:
            /**
             * \brief Initialize the Resources
             */
            void Initialize();


            /**
             * \brief Return the LoaderRegister
             */
            std::weak_ptr<LoaderRegister> GetLoaderRegister() const {
                return mLoaderRegister;
            }

            /**
            * \brief Load file from disk
            * \note This function will infer the file type from the extension of the file
            *       and will return null shared_ptr when there is no extension.
            * \param _file The path of the file
            * \return The object referring to the resource
            */
            std::shared_ptr<Object> Load(const std::string& _file);

            /**
            * \brief Load file from disk, parse it using loader that supports specified type
            * \param _file The path of the file
            * \param _type
            * \return The object referring to the resource
            */
            std::shared_ptr<Object> Load(const std::string& _file, const ResourceType _type);


            /**
             * \brief Unload all resource
             */
            void UnloadAll();

        private:
            std::shared_ptr<LoaderRegister> mLoaderRegister;
            ResourceRefMgr mResourceMap;

            bool IsResourceAlreadyLoaded(const std::filesystem::path& _path) const;
            static std::filesystem::path GetGenericPath(const std::string& _file);
        };
    }
}
