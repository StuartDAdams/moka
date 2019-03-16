
#include <application/application.hpp>

using namespace moka;

class texture_application final : public application
{
    float vertices_[18] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f};

    const char* vertex_source_ =
        "    #version 330 core                               \n"
        "    layout (location = 0) in vec3 position;         \n"
        "    layout (location = 1) in vec4 color0;           \n"
        "    out vec3 out_color0;                            \n"
        "    void main()                                     \n"
        "    {                                               \n"
        "        gl_Position = vec4(position, 1.0);          \n"
        "        out_color0 = color0.xyz;                    \n"
        "    }                                               \0";

    const char* fragment_source_ =
        "    #version 330 core                               \n"
        "    out vec4 FragColor;                             \n"
        "    in vec3 out_color0;                             \n"
        "    void main()                                     \n"
        "    {                                               \n"
        "        FragColor = vec4(out_color0, 1.0f);         \n"
        "    }                                               \0";

    vertex_buffer_handle vertex_buffer_;
    index_buffer_handle index_buffer_;
    material_handle material_;

public:
    texture_application(const app_settings& app_settings) : application(app_settings)
    {
        vertex_layout layout = {{0, attribute_type::float32, 2, false, 6 * sizeof(float), 0},
                                {1, attribute_type::float32, 3, false, 6 * sizeof(float), 3 * sizeof(float)}};

        vertex_buffer_ = graphics_.make_vertex_buffer(
            vertices_, sizeof vertices_, std::move(layout), buffer_usage::static_draw);

        material_ = graphics_.build_material()
                        .set_vertex_shader(vertex_source_)
                        .set_fragment_shader(fragment_source_)
                        .build();
    }

    ~texture_application()
    {
        // todo: destroy graphics resources
        timer_.stop();
    }

    void draw(const game_time delta_time) override
    {
        command_list list;

        list.clear().set_color(1.0f, 0.0f, 0.0f, 1.0f).set_clear_color(true).set_clear_depth(true);

        list.draw().set_vertex_buffer(vertex_buffer_).set_material(material_).set_vertex_count(3);

        graphics_.submit_and_swap(std::move(list));
    }

    void update(const game_time delta_time) override
    {
    }

    std::filesystem::path data_path() override
    {
        // cmake-defined macro points to example project asset folder relative
        // to source. A real application could point this wherever it wanted.
        std::filesystem::path result{MOKA_ASSET_PATH};
        return result.lexically_normal();
    }
};

int main(const int argc, char* argv[])
{
    app_settings settings{};
    settings.window_settings.resolution = {1600, 900};
    settings.window_settings.fullscreen = false;

    return texture_application{settings}.run();
}