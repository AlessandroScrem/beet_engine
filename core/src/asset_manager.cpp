#include <beet/asset_manager.h>

namespace beet {

void AssetManager::on_awake() {
    s_assetManager = std::ref(*this);
    // TODO LOAD ALL ASSETS IN "ASSET/RES" FOLDER
    load_assets_manual();
    load_assets_serialize();
}
void AssetManager::on_destroy() {
    for (auto it = m_assets.begin(); it != m_assets.end(); ++it) {
        it->second->on_destroy();
    }
    m_assets.clear();
    log::debug("AssetManager destroyed");
}

void AssetManager::load_assets_manual() {
    //=Gen Textures===
    const std::string fb_tex = "fallbackTexture";
    m_assets.insert({fb_tex, std::make_shared<components::Texture>()});
    std::static_pointer_cast<components::Texture>(m_assets[fb_tex])->generate_default_asset();

    const std::string white_tex = "whiteTexture";
    m_assets.insert({white_tex, std::make_shared<components::Texture>()});
    std::static_pointer_cast<components::Texture>(m_assets[white_tex])
        ->generate_solid_color_texture(vec4(1), white_tex);

    //=Gen Meshes=====

    const std::string fallbackMesh = "fallbackMesh";
    m_assets.insert({fallbackMesh, std::make_shared<components::Mesh>()});
    std::static_pointer_cast<components::Mesh>(m_assets[fallbackMesh])->generate_default_asset();

    //=From File======
    AssetManager::load_asset<components::Texture>("UV_Grid_test.png");
    AssetManager::load_asset<components::Texture>("normal_tiles_1k.png");
    AssetManager::load_asset<components::Texture>("particles/particle.png");
    AssetManager::load_asset<components::Texture>("skybox/cmft_skybox.hdr");
    AssetManager::load_asset<components::Texture>("skybox/cmtr_radiance.hdr");
    AssetManager::load_asset<components::Texture>("skybox/cmtr_irradiance.hdr");

    AssetManager::load_asset<components::Mesh>("primitive_plane.obj");
    AssetManager::load_asset<components::Mesh>("default_cube.obj");
}

void AssetManager::load_assets_serialize() {}

}  // namespace beet