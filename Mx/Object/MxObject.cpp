#include "MxObject.h"
#include "../Exceptions/MxExceptions.hpp"
#include "../Log/MxLog.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_PARENT_NO_CREATE_FUNC(Object)

    std::vector<Object*> Object::sObjectList;

    bool Object::RegisterFactoryFunc(const std::string& _typeName, FactoryFunction _func) {
        auto funcMap = GlobalClassFactoryFuncMap::GetInstance().lock();

        if(funcMap->count(_typeName) != 0)
            throw FactoryFunctionRepetitionException(_typeName);

        try {
            funcMap->insert({_typeName, _func});
        }
        catch(std::out_of_range& e) {
            Log::Error(e.what());
            return false;
        }

        return true;
    }
}
