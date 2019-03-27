#pragma once

namespace moka
{
    class clear_command;
    class draw_command;
    class scissor_command;
    class viewport_command;
    class fill_vertex_buffer_command;
    class fill_index_buffer_command;
    class frame_buffer_command;
    class frame_buffer_texture_command;
    class generate_mipmaps_command;
    class set_material_parameters_command;

    /**
     * \brief Used to define visitor-pattern functionality for graphics_commands
     */
    class graphics_visitor
    {
    public:
        virtual ~graphics_visitor() = default;
        virtual void visit(clear_command& cmd) = 0;
        virtual void visit(draw_command& cmd) = 0;
        virtual void visit(viewport_command& cmd) = 0;
        virtual void visit(scissor_command& cmd) = 0;
        virtual void visit(fill_vertex_buffer_command& cmd) = 0;
        virtual void visit(fill_index_buffer_command& cmd) = 0;
        virtual void visit(frame_buffer_command& cmd) = 0;
        virtual void visit(frame_buffer_texture_command& cmd) = 0;
        virtual void visit(generate_mipmaps_command& cmd) = 0;
        virtual void visit(set_material_parameters_command& cmd) = 0;
    };
} // namespace moka