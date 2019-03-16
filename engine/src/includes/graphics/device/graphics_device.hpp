#pragma once

#include "graphics/buffer/frame_buffer_handle.hpp"
#include "graphics/material/material_builder.hpp"
#include <application/window.hpp>
#include <graphics/api/graphics_api.hpp>
#include <graphics/command/command_list.hpp>
#include <memory>
#include <queue>

namespace moka
{
    enum class graphics_backend
    {
        direct3d_9,
        direct3d_11,
        direct3d_12,
        gnm,
        metal,
        opengl_es,
        opengl,
        vulkan,
        null
    };

    /* string containing unique identifier of the texture
     */
    using texture_id = std::string;

    class texture_cache
    {
        graphics_device& device_;

        std::vector<texture_handle> textures_;
        std::unordered_map<texture_id, int> texture_lookup_;

    public:
        explicit texture_cache(graphics_device& device, const size_t initial_capacity = 0)
            : device_(device)
        {
            textures_.reserve(initial_capacity);
        }

        void add_texture(texture_handle handle, const texture_id& id)
        {
            const auto index = textures_.size();
            textures_.emplace_back(handle);
            texture_lookup_[id] = index;
        }

        bool exists(const texture_id& id) const
        {
            return texture_lookup_.find(id) != texture_lookup_.end();
        }

        texture_handle get_texture(const texture_id& id) const
        {
            return textures_[texture_lookup_.at(id)];
        }
    };

    /* string containing unique identifier of shader source code + all preprocessor definitions
     * the preprocessor definitions are necessary to differentiate different configurations of the same shader source
     */
    using program_id = std::string;

    class program_cache
    {
        graphics_device& device_;

        std::vector<program_handle> shaders_;
        std::unordered_map<program_id, int> shader_lookup_;

    public:
        explicit program_cache(graphics_device& device, const size_t initial_capacity = 0)
            : device_(device)
        {
            shaders_.reserve(initial_capacity);
        }

        void add_program(program_handle handle, const program_id& id)
        {
            shaders_.emplace_back(handle);
        }

        bool exists(const program_id& id) const
        {
            return shader_lookup_.find(id) != shader_lookup_.end();
        }

        program_handle get_program(const program_id& id) const
        {
            return shaders_[shader_lookup_.at(id)];
        }
    };

    class material_cache
    {
        graphics_device& device_;

        std::vector<material> materials_;

    public:
        explicit material_cache(graphics_device& device, const size_t initial_capacity = 0)
            : device_(device)
        {
            materials_.reserve(initial_capacity);
        }

        material_handle add_material(material&& t)
        {
            const auto index = materials_.size();
            materials_.emplace_back(std::move(t));
            return material_handle(index);
        }

        material* get_material(const material_handle handle)
        {
            if (handle == std::numeric_limits<material_handle>::max())
            {
                return nullptr;
            }
            return &materials_[handle];
        }

        const material* get_material(const material_handle handle) const
        {
            if (handle == std::numeric_limits<material_handle>::max())
            {
                return nullptr;
            }
            return &materials_[handle];
        }
    };

    class graphics_device
    {
        std::unique_ptr<graphics_api> graphics_api_;

        texture_cache textures_;

        program_cache shaders_;

        material_cache materials_;

    public:
        texture_cache& get_texture_cache();

        const texture_cache& get_texture_cache() const;

        program_cache& get_shader_cache();

        const program_cache& get_shader_cache() const;

        material_cache& get_material_cache();

        const material_cache& get_material_cache() const;

        explicit graphics_device(window& window, graphics_backend graphics_backend = graphics_backend::opengl);

        vertex_buffer_handle make_vertex_buffer(
            const void* cube_vertices, size_t size, vertex_layout&& layout, buffer_usage use) const;

        index_buffer_handle make_index_buffer(const void* indices, size_t size, index_type type, buffer_usage use) const;

        shader_handle make_shader(shader_type type, const std::string& source) const;

        program_handle make_program(shader_handle vertex_handle, shader_handle fragment_handle) const;

        texture_handle make_texture(void** data, texture_metadata&& metadata, bool free_host_data) const;

        texture_builder build_texture();

        frame_buffer_handle make_frame_buffer(render_texture_data* render_textures, size_t render_texture_count) const;

        frame_buffer_builder build_frame_buffer();

        material_builder build_material();

        void destroy(program_handle handle);

        void destroy(shader_handle handle);

        void destroy(vertex_buffer_handle handle);

        void destroy(index_buffer_handle handle);

        void submit(command_list&& command_list, bool sort = true) const;

        void submit_and_swap(command_list&& command_list, bool sort = true) const;
    };
} // namespace moka