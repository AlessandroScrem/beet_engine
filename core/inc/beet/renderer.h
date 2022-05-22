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

    void update_universal_buffer_data();
    void shadow_pass();
    void depth_pass();
    void color_pass();
    void gui_pass();
    void transparent_pass();
    void post_process_pass();

   private:
    entt::registry test_registry;

    Engine& m_engine;
    UniversalBufferData m_universalBufferData;

    vec4 m_clearCol{1.0f, 0.4f, 0.4f, 1.0f};
    float m_timePassed{0.0f};

    components::ShaderProgram m_depthProgram;
    GLuint m_modelUniform{0};

    mat4 m_tmpInverseProj{0};
};

}  // namespace beet
