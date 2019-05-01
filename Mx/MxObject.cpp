#include "MxObject.h"

namespace Mix {

    MX_IMPLEMENT_RTTI_NoParent_NoCreateFunc(Object);

    std::vector<Object*> Object::mObjectList;

    bool Object::registerFactoryFunc(const std::string & typeName, FactoryFunction func) {

        auto funcMap = GlobalClassFactoryFuncMap::getInstance().lock();

        if (funcMap->count(typeName) != 0)
            throw RepeatFactoryFuncExcep(typeName);

        try {
            funcMap->insert({ typeName, func });
        }
        catch (std::out_of_range& e) {
            std::cout << e.what() << std::endl;
            return false;
        }

        return true;
    }
}
