#pragma once
#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/Graphics/MxMaterial.h"
#include "../Mx/Graphics/Texture/MxTexture.h"

namespace Scripts {
    using namespace Mix;

    class Loading :public Script {
        MX_DECLARE_RTTI;
    public:

    private:
        void awake() override;

        void start() override;

        void update() override;

    private:
        std::vector<std::shared_ptr<Texture2D>> mTextures;
        std::shared_ptr<Material> mMaterial;

        uint32_t mCurrFrame = 0;
        float mFps = 12.0f;
        float mFrameGap = 0.0f;

        float mMinFps = 1.0f;
        float mMaxFps = 120.0f;
    };
}
