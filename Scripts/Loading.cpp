#include "Loading.h"
#include "../Mx/Resource/MxResourceLoader.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/Component/Renderer/MxRenderer.h"
#include "../Mx/Log/MxLog.h"
#include "../Mx/Time/MxTime.h"
#include "../Mx/Input/MxInput.h"

namespace Scripts {
    MX_IMPLEMENT_RTTI(Loading, Script);

    void Loading::awake() {
    }

    void Loading::start() {

        auto param = GetAdditionalParam<Texture2D>();
        param.mipLevel = 1;
        param.samplerInfo.minFilter = TextureFilterMode::Linear;
        param.samplerInfo.magFilter = TextureFilterMode::Linear;
        param.samplerInfo.mipFilter = TextureMipSampleMode::Linear;

        mTextures.reserve(13);
        for (uint32_t i = 0; i < 13; ++i) {
            mTextures.push_back(ResourceLoader::Get()->load<Texture2D>("Resource/Textures/Loading/" + std::to_string(i) + ".png", &param));
        }

        mMaterial = mGameObject->getComponent<Renderer>()->getMaterial();
        mMaterial->setTexture("diffuseTex", mTextures[0]);
    }

    void Loading::update() {
        mMaterial->setTexture("diffuseTex", mTextures[mCurrFrame]);
        mFrameGap += Time::DeltaTime();
        if (mFrameGap >= 1 / mFps) {
            mCurrFrame = (mCurrFrame + 1) % 13;
            mFrameGap = 0.0f;
        }

        if (Input::Get()->isButtonHold(ButtonCode::Up))
            mFps += 0.5f;
        if (Input::Get()->isButtonHold(ButtonCode::Down))
            mFps -= 0.5f;
        mFps = std::clamp(mFps, mMinFps, mMaxFps);
    }
}
