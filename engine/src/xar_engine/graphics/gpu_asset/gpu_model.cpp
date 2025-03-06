#include <xar_engine/graphics/gpu_asset/gpu_model.hpp>


namespace xar_engine::graphics::gpu_asset
{
    GpuModelListBufferStructure make_gpu_model_list_buffer_structure(const std::vector<asset::Model>& model_list)
    {
        auto gpu_model_offsets = GpuModelListBufferStructure{};

        auto vertex_offset = std::size_t{0};
        auto index_offset = std::size_t{0};

        gpu_model_offsets.gpu_model_buffer_structure_list.reserve(model_list.size());
        for (const auto& model: model_list)
        {
            auto& gpu_model_offset = gpu_model_offsets.gpu_model_buffer_structure_list.emplace_back();
            gpu_model_offset.first_vertex = vertex_offset;
            gpu_model_offset.first_index = index_offset;

            gpu_model_offset.gpu_mesh_buffer_structure_list.reserve(model.mesh_list.size());
            for (const auto& mesh: model.mesh_list)
            {
                auto& gpu_mesh_offset = gpu_model_offset.gpu_mesh_buffer_structure_list.emplace_back();
                gpu_mesh_offset.first_vertex = vertex_offset;
                gpu_mesh_offset.first_index = index_offset;
                gpu_mesh_offset.vertex_counts = mesh.position_list.size();
                gpu_mesh_offset.index_counts = mesh.index_list.size();

                vertex_offset += mesh.position_list.size();
                index_offset += mesh.index_list.size();
            }

            for (const auto& gpu_mesh : gpu_model_offset.gpu_mesh_buffer_structure_list)
            {
                gpu_model_offset.vertex_counts += gpu_mesh.vertex_counts;
                gpu_model_offset.index_counts += gpu_mesh.index_counts;
            }
        }

        for (const auto& gpu_model : gpu_model_offsets.gpu_model_buffer_structure_list)
        {
            gpu_model_offsets.vertex_counts += gpu_model.vertex_counts;
            gpu_model_offsets.index_counts += gpu_model.index_counts;
        }

        return gpu_model_offsets;
    }
}