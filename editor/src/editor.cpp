#include "editor.h"
#include <beet/components.h>
#include <beet/log.h>

namespace beet {
Editor::Editor() {
    m_scene = std::make_unique<Scene>();
    Scene::set_active_scene(*m_scene);
    log::Logger::setup();
    m_engine = std::make_unique<Engine>();

    {
        auto editorCamera = m_scene->create_game_object("camera");
        auto& cam = editorCamera.add_component<components::Camera>();
        editorCamera.get_component<components::Transform>().set_position(glm::vec3(0.0f, 0.0f, -5.5));
    }

    {
        auto cubeObj = m_scene->create_game_object("loaded_cube");
        cubeObj.get_component<components::Transform>().set_position(glm::vec3(0, 0, 0));
        cubeObj.get_component<components::Transform>().set_scale(glm::vec3(1, 1, 1));
        cubeObj.get_component<components::Transform>().set_rotation_euler(glm::vec3(0, 45, 0));

        cubeObj.add_component<components::InstanceMesh>("default_cube.obj");

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "darkOceanTiles07/DarkOceanTiles07_1K_Albedo.png");
        material.set_texture_slot_path(TextureType::Normal, "darkOceanTiles07/DarkOceanTiles07_1K_Normal.png");
        material.set_texture_slot_path(TextureType::Metallic, "darkOceanTiles07/DarkOceanTiles07_1K_Height.png");
        material.set_texture_slot_path(TextureType::Roughness, "darkOceanTiles07/DarkOceanTiles07_1K_Roughness.png");
        material.set_texture_slot_path(TextureType::Occlusion, "darkOceanTiles07/DarkOceanTiles07_1K_AO.png");
        cubeObj.add_component<components::Material>(material);
    }

    {
        auto light = m_scene->create_game_object("pLight0");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(-2, -1, 1));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(0, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        light.add_component<components::Material>(material);

        light.add_component<components::InstanceMesh>("sphere.obj");
    }
    {
        auto light = m_scene->create_game_object("pLight1");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(-1.5f, 0, -1.5f));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));

        pointLight.set_color(vec3(0, 1, 1));
        pointLight.set_intensity(3);
        pointLight.set_range(10);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        light.add_component<components::Material>(material);

        light.add_component<components::InstanceMesh>("sphere.obj");
    }
    {
        auto light = m_scene->create_game_object("pLight2");
        auto& pointLight = light.add_component<components::PointLight>();
        light.get_component<components::Transform>().set_position(glm::vec3(1.5f));
        light.get_component<components::Transform>().set_scale(glm::vec3(0.2f));
        pointLight.set_color(vec3(1, 1, 0));
        pointLight.set_intensity(3);
        pointLight.set_range(10);

        auto material = components::Material();
        material.set_texture_slot_path(TextureType::Albedo, "whiteTexture");
        material.set_texture_slot_path(TextureType::Normal, "whiteTexture");
        material.set_texture_slot_path(TextureType::Metallic, "whiteTexture");
        material.set_texture_slot_path(TextureType::Roughness, "whiteTexture");
        material.set_texture_slot_path(TextureType::Occlusion, "whiteTexture");
        light.add_component<components::Material>(material);

        light.add_component<components::InstanceMesh>("sphere.obj");
    }
}

void Editor::run() {
    while (m_engine->is_open()) {
        m_engine->update_modules();
    }
}

}  // namespace beet
