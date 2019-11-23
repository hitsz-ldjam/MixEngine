#pragma once
#ifndef MX_RENDER_API_MANAGER_H_
#define MX_RENDER_API_MANAGER_H_

#include "../Engine/MxModuleBase.h"

namespace Mix {
    class RenderAPI;
    class Window;

    class RenderAPIManager :public ModuleBase {
    public:
        static RenderAPIManager* Get();

        ~RenderAPIManager();

        void loadRenderAPI(const std::string& _name, Window* _window);

        RenderAPI* getRenderAPI() const { return mActiveRenderAPI; };

    private:
        RenderAPI* mActiveRenderAPI = nullptr;
    };
}

#endif
