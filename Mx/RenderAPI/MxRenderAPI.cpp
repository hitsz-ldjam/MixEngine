#include "MxRenderAPI.h"
#include "MxRenderAPIManager.h"

namespace Mix {
    RenderAPI* RenderAPI::Get() {
        return RenderAPIManager::Get()->getRenderAPI();
    }

    std::shared_ptr<GPUProgram> RenderAPI::createGPUProgram(const GPUProgramDesc& _desc) const {
        return mGPUProgramManager.create(_desc);
    }
}
