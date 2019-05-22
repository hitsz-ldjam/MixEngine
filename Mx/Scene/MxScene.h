#pragma once
#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include "../Window/MxWindow.h"
#include "../GameObject/MxGameObject.h"

namespace Mix {
    // todo: temp demo scene
    class Scene {
    public:
        explicit Scene(const std::string &_str);

        ~Scene();

        void init();

        void update();

        void fixedUpate();

        void lateUpate();

        void addGameObject(GameObject* _obj);

        void removeGameObject(GameObject* _obj);

        void getLoaded();

        std::vector<GameObject*> getRootGameObject();

        bool isLoad();

        size_t buildIndex() const;

        std::string getName() const;
    private:
        std::vector<GameObject*> mHierarchy;
        size_t mBuildIndex;
        bool mIsLoaded;
        std::string mName;

        Window* mWindow;
    };
}

#endif
