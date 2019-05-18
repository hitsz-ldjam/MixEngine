#pragma once
#include "MxResourceParserBase.hpp"
#include "MxParserRegister.hpp"

namespace Mix {
    namespace Resource {
        class ResourceBase;

        class Resources {
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
            std::shared_ptr<Object> load(const std::string& _file);

            /**
            * \brief Load file from disk, parse it using loader that supports specified type
            * \param _file The path of the file
            * \param _type
            * \return The object referring to the resource
            */
            std::shared_ptr<Object> load(const std::string& _file, const ResourceType _type);


            /**
             * \brief Unload all resource
             */
            void unloadAll();

        private:
            std::shared_ptr<ParserRegister> mLoaderRegister;
            ResourceRefMgr mResourceRefMgr;

            bool isResourceAlreadyLoaded(const std::filesystem::path& _path) const;
            static std::filesystem::path GetGenericPath(const std::string& _file);
        };
    }
}
