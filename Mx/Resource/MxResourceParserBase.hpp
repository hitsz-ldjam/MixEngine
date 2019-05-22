#pragma once
#ifndef MX_RESOURCE_PARSER_BASE_H_
#define MX_RESOURCE_PARSER_BASE_H_

#include "MxResourceBase.h"
#include "../Object/MxObject.h"

#include <set>
#include <unordered_set>

namespace Mix {
    namespace Resource {
        class ResourceParserBase {
        public:
            explicit ResourceParserBase(ResourceRefMgr* _mgr) :mRefMgr(_mgr) {}

            ResourceParserBase(const ResourceParserBase& _other) = default;

            ResourceParserBase(ResourceParserBase&& _other) noexcept = default;

            ResourceParserBase& operator=(const ResourceParserBase& _other) = default;

            ResourceParserBase& operator=(ResourceParserBase&& _other) noexcept = default;

            /**
             * \brief Load file and parse it with specified type
             */
            virtual Object* load(const std::filesystem::path& _path, const ResourceType _type) = 0;

            /**
             * \brief Load file and parse it with specified extension
             */
            virtual Object* load(const std::filesystem::path& _path, const std::string& _ext) = 0;

            /**
             * \brief Parse a resource 
             * \param _key Key used to find resource in ResourceRefMgr
             */
            virtual Object* parse(const Guid& _key) = 0;

            virtual ~ResourceParserBase() = default;

            /**
             * \brief Check if ResourceType _type is supported by this parser
             */
            bool isSupport(const ResourceType _type) const {
                return mSupportedTypes.count(_type);
            }

            /**
             * \brief Check if extension _ext is supported by this parser
             */
            bool isSupport(const std::string& _ext) const {
                return mSupportedExts.count(_ext);
            }

            /**
             * \brief Get all types supported by this parser
             */
            const std::set<ResourceType>& allSupportedTypes() const {
                return mSupportedTypes;
            }

            /**
             * \brief Get all extension supported by this parser
             */
            const std::unordered_set<std::string>& allSupportedExts()const {
                return mSupportedExts;
            }

        protected:
            ResourceRefMgr* mRefMgr;
            std::set<ResourceType> mSupportedTypes;
            std::unordered_set<std::string> mSupportedExts;

        };
    }
}

#endif
