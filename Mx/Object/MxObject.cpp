#include "MxObject.h"

namespace Mix {

    MX_IMPLEMENT_RTTI_NoParent_NoCreateFunc(Object);

    std::vector<Object*> Object::mObjectList;

    bool Object::RegisterFactoryFunc(const std::string & _typeName, FactoryFunction _func) {

        auto funcMap = GlobalClassFactoryFuncMap::getInstance().lock();

        if (funcMap->count(_typeName) != 0)
            throw RepeatFactoryFuncExcep(_typeName);

        try {
            funcMap->insert({ _typeName, _func });
        }
        catch (std::out_of_range& e) {
            std::cout << e.what() << std::endl;
            return false;
        }

        return true;
    }
}
