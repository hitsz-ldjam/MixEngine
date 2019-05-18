#pragma once
#ifndef MX_RESOURCE_PARSER_BASE_H_
#define MX_RESOURCE_PARSER_BASE_H_

#include <set>
#include <unordered_set>
#include "MxResourceBase.h"

namespace Mix {
    namespace Resource {
        class ResourceParserBase {
        public:
            explicit ResourceParserBase(ResourceRefMgr* _mgr) :mRefMgr(_mgr) {}

            ResourceParserBase(const ResourceParserBase& _other) = default;

            ResourceParserBase(ResourceParserBase&& _other) noexcept = default;

            ResourceParserBase& operator=(const ResourceParserBase& _other) = default;

            ResourceParserBase& operator=(ResourceParserBase&& _other) noexcept = default;

            virtual std::shared_ptr<Object> load(const std::filesystem::path& _path, const ResourceType _type) = 0;

            virtual std::shared_ptr<Object> load(const std::filesystem::path& _path, const std::string& _ext) = 0;

            virtual std::shared_ptr<Object> parse(const Guid& _key) = 0;

            virtual ~ResourceParserBase() = default;

            bool isSupport(const ResourceType _type) const {
                return mSupportedTypes.count(_type);
            }

            bool isSupport(const std::string& _ext) const {
                return mSupportedExts.count(_ext);
            }

            const std::set<ResourceType>& allSupportedTypes() const {
                return mSupportedTypes;
            }

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
