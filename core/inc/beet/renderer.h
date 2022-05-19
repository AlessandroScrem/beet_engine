#pragma once

#include <beet/camera.h>
#include <beet/components.h>
#include <beet/framebuffer.h>
#include <beet/log.h>
#include <beet/mesh.h>
#include <beet/shader_program.h>
#include <beet/subsystem.h>
#include <beet/texture.h>
#include <beet/transform.h>
#include <beet/universal_buffer_data.h>
#include <entt/entt.hpp>

namespace beet {
class Engine;
}  // namespace beet

namespace beet {

class Renderer : public Subsystem {
   public:
    explicit Renderer(Engine& engine);
    ~Renderer();

    void on_awake() override;
    void on_update(double deltaTime) override;
    void on_late_update() override;
    void on_destroy() override;

    void resize_all_framebuffers(const vec2i& size);

   private:
    // 1) SYSTEM : Shadow pass
    // 2) SYSTEM : Update Active Camera (Runtime / Editor)
    // 3) SYSTEM : Depth Pass (No Transparent Objects)
    // 4) SYSTEM : Color Render Pass
    // 5) SYSTEM : Skybox Render
    // 6) SYSTEM : Sorted Transparent Render Pass
    // 7) SYSTEM : Post Processing Stack

    void shadow_pass(uint16_t id);
    void depth_pass(uint16_t id);
    void color_pass(uint16_t id);
    void gui_pass(uint16_t id);
    void transparent_pass(uint16_t id);
    void post_process_pass(uint16_t id);

   private:
    entt::registry test_registry;

    Engine& m_engine;
    UniversalBufferData m_universalBufferData;

    vec4 m_clearCol{1.0f, 0.4f, 0.4f, 1.0f};
    float m_timePassed{0.0f};

    std::shared_ptr<components::Mesh> m_tempPostProcessMesh;
    components::ShaderProgram m_tempScreenShader;

    Framebuffer m_tempFramebufferColor;
};

}  // namespace beet
