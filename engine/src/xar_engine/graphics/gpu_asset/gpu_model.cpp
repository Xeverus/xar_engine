#include <xar_engine/graphics/gpu_asset/gpu_model.hpp>


namespace xar_engine::graphics::gpu_asset
{
    namespace
    {
        constexpr auto PositionType = sizeof(asset::Mesh{}.position_list[0]);
        constexpr auto NormalType = sizeof(asset::Mesh{}.normal_list[0]);
        constexpr auto TextureCoordType = sizeof(asset::Mesh{}.texture_coord_list[0]);
        constexpr auto IndexType = sizeof(asset::Mesh{}.index_list[0]);

        void fill_vertex_and_index_offset_values(
            GpuModelListBufferStructure& gpu_model_list_buffer_structure,
            const std::vector<asset::Model>& model_list)
        {
            auto vertex_offset = std::size_t{0};
            auto index_offset = std::size_t{0};

            gpu_model_list_buffer_structure.gpu_model_buffer_structure_list.reserve(model_list.size());
            for (const auto& model: model_list)
            {
                auto& gpu_model_offset = gpu_model_list_buffer_structure.gpu_model_buffer_structure_list.emplace_back();
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

                for (const auto& gpu_mesh: gpu_model_offset.gpu_mesh_buffer_structure_list)
                {
                    gpu_model_offset.vertex_counts += gpu_mesh.vertex_counts;
                    gpu_model_offset.index_counts += gpu_mesh.index_counts;
                }
            }

            for (const auto& gpu_model: gpu_model_list_buffer_structure.gpu_model_buffer_structure_list)
            {
                gpu_model_list_buffer_structure.vertex_counts += gpu_model.vertex_counts;
                gpu_model_list_buffer_structure.index_counts += gpu_model.index_counts;
            }
        }

        void fill_byte_size_values(
            GpuModelListBufferStructure& gpu_model_list_buffer_structure,
            const std::vector<asset::Model>& model_list)
        {
            gpu_model_list_buffer_structure.position_list_byte_size =
                gpu_model_list_buffer_structure.vertex_counts * sizeof(PositionType);
            gpu_model_list_buffer_structure.normal_list_byte_size =
                gpu_model_list_buffer_structure.vertex_counts * sizeof(NormalType);
            gpu_model_list_buffer_structure.texture_coord_list_byte_size =
                gpu_model_list_buffer_structure.vertex_counts * sizeof(TextureCoordType);
            gpu_model_list_buffer_structure.index_list_byte_size =
                gpu_model_list_buffer_structure.index_counts * sizeof(IndexType);
        }
    }

    GpuModelListBufferStructure make_gpu_model_list_buffer_structure(const std::vector<asset::Model>& model_list)
    {
        auto gpu_model_list_buffer_structure = GpuModelListBufferStructure{};
        fill_vertex_and_index_offset_values(
            gpu_model_list_buffer_structure,
            model_list);
        fill_byte_size_values(
            gpu_model_list_buffer_structure,
            model_list);

        return gpu_model_list_buffer_structure;
    }
}