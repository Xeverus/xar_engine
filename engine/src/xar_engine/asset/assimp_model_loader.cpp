#include <xar_engine/asset/assimp_model_loader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::asset
{
    namespace
    {
        std::tuple<std::unique_ptr<Assimp::Importer>, const aiScene*> load_data_from_file(const std::filesystem::path& path)
        {
            constexpr auto aiPostProcessSteps = aiProcess_Triangulate |
                                                aiProcess_FlipUVs |
                                                aiProcess_JoinIdenticalVertices;

            auto ai_importer = std::make_unique<Assimp::Importer>();
            const auto* const ai_scene = ai_importer->ReadFile(
                path.string(),
                aiPostProcessSteps);
            XAR_THROW_IF(
                !ai_scene || (ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !ai_scene->mRootNode,
                error::XarException,
                "Failed to load model from file '{}'",
                path.string());

            return {std::move(ai_importer), ai_scene};
        }

        Model parse_ai_scene(const aiScene& ai_scene)
        {
            auto model = Model{};

            model.meshes.resize(ai_scene.mNumMeshes);
            for (auto ai_mesh_index = 0; ai_mesh_index < ai_scene.mNumMeshes; ++ai_mesh_index)
            {
                auto& mesh = model.meshes[ai_mesh_index];
                auto* const ai_mesh = ai_scene.mMeshes[ai_mesh_index];

                mesh.vertices.resize(ai_mesh->mNumVertices);
                for (auto ai_vertex_index = 0; ai_vertex_index < ai_mesh->mNumVertices; ++ai_vertex_index)
                {
                    auto& vertex = mesh.vertices[ai_vertex_index];
                    const auto& ai_vertex = ai_mesh->mVertices[ai_vertex_index];

                    vertex.x = ai_vertex.x;
                    vertex.y = ai_vertex.y;
                    vertex.z = ai_vertex.z;
                }

                mesh.indices.resize(ai_mesh->mNumFaces * 3);
                for (auto ai_face_index = 0; ai_face_index < ai_mesh->mNumFaces; ++ai_face_index)
                {
                    const auto& ai_face = ai_mesh->mFaces[ai_face_index];

                    mesh.indices[ai_face_index * 3 + 0] = ai_face.mIndices[0];
                    mesh.indices[ai_face_index * 3 + 1] = ai_face.mIndices[1];
                    mesh.indices[ai_face_index * 3 + 2] = ai_face.mIndices[2];
                }

                for (auto ai_tex_coords_index = 0;
                     ai_tex_coords_index < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++ai_tex_coords_index)
                {
                    const auto* const ai_tex_coords_list = ai_mesh->mTextureCoords[ai_tex_coords_index];
                    if (ai_tex_coords_list == nullptr)
                    {
                        break;
                    }

                    mesh.tex_coords_channel_count.push_back(ai_mesh->mNumUVComponents[ai_tex_coords_index]);
                    mesh.tex_coords.emplace_back(ai_mesh->mNumVertices * mesh.tex_coords_channel_count.back());
                    for (auto ai_vertex_index = 0; ai_vertex_index < ai_mesh->mNumVertices; ++ai_vertex_index)
                    {
                        auto& tex_coords = mesh.tex_coords.back();
                        const auto& ai_tex_coords = ai_tex_coords_list[ai_vertex_index];

                        for (auto i = 0; i < mesh.tex_coords_channel_count.back(); ++i)
                        {
                            tex_coords[ai_vertex_index * mesh.tex_coords_channel_count.back() + i] = ai_tex_coords[i];
                        }
                    }
                }
            }

            return model;
        }
    }

    Model AssimpModelLoader::load_model_from_file(const std::filesystem::path& path) const
    {
        auto [ai_importer, ai_scene] = load_data_from_file(path);
        return parse_ai_scene(*ai_scene);
    }
}
