#include <BanubaSdkManager.hpp>

#include "BanubaClientToken.hpp"

using namespace bnb::interfaces;

int main()
{
#if BNB_GL_BACKEND
    effect_player::set_render_backend(render_backend_type::opengl);
#else
    effect_player::set_render_backend(render_backend_type::wgpu);
#endif
    
    BanubaSdkManager sdk(
        "Realtime Camera Preview",
        {BanubaSdkManager::sdk_resources_path(), BNB_RESOURCES_FOLDER},
        BNB_CLIENT_TOKEN);
    sdk.load_effect("effects/TrollGrandma", true);
    sdk.process_camera();
    sdk.run_main_loop();
    return 0;
}
