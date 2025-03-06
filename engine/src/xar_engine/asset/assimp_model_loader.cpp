#include <xar_engine/asset/assimp_model_loader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::asset
{
    namespace
    {
        class MeshParser
        {
        public:
            explicit MeshParser(const aiMesh& ai_mesh)
                : _ai_mesh(ai_mesh)
            {
            }

            [[nodiscard]]
            Mesh parse_mesh() const
            {
                return {
                    parse_positions(),
                    parse_normals(),
                    parse_texture_coords(),
                    parse_indices()
                };
            }

        private:
            [[nodiscard]]
            std::vector<math::Vector3f> parse_positions() const
            {
                if (!_ai_mesh.HasPositions())
                {
                    return {};
                }

                auto positions = std::vector<math::Vector3f>(_ai_mesh.mNumVertices);
                for (auto ai_vertex_index = 0; ai_vertex_index < _ai_mesh.mNumVertices; ++ai_vertex_index)
                {
                    auto& vertex = positions[ai_vertex_index];
                    const auto& ai_vertex = _ai_mesh.mVertices[ai_vertex_index];

                    vertex.x = ai_vertex.x;
                    vertex.y = ai_vertex.y;
                    vertex.z = ai_vertex.z;
                }

                return positions;
            }

            [[nodiscard]]
            std::vector<math::Vector3f> parse_normals() const
            {
                if (!_ai_mesh.HasNormals())
                {
                    return {};
                }

                auto normals = std::vector<math::Vector3f>(_ai_mesh.mNumVertices);
                for (auto ai_vertex_index = 0; ai_vertex_index < _ai_mesh.mNumVertices; ++ai_vertex_index)
                {
                    auto& vertex = normals[ai_vertex_index];
                    const auto& ai_vertex = _ai_mesh.mNormals[ai_vertex_index];

                    vertex.x = ai_vertex.x;
                    vertex.y = ai_vertex.y;
                    vertex.z = ai_vertex.z;
                }

                return normals;
            }

            [[nodiscard]]
            std::vector<math::Vector2f> parse_texture_coords() const
            {
                if (!_ai_mesh.HasTextureCoords(0) || _ai_mesh.mNumUVComponents[0] != 2)
                {
                    return {};
                }

                auto texture_coords = std::vector<math::Vector2f>(_ai_mesh.mNumVertices);
                for (auto ai_vertex_index = 0; ai_vertex_index < _ai_mesh.mNumVertices; ++ai_vertex_index)
                {
                    auto& tex_coords = texture_coords[ai_vertex_index];
                    const auto& ai_tex_coords = _ai_mesh.mTextureCoords[0][ai_vertex_index];

                    tex_coords.x = ai_tex_coords.x;
                    tex_coords.y = ai_tex_coords.y;
                }

                return texture_coords;
            }

            [[nodiscard]]
            std::vector<std::uint32_t> parse_indices() const
            {
                if (!_ai_mesh.HasFaces())
                {
                    return {};
                }

                auto indices = std::vector<std::uint32_t>(_ai_mesh.mNumFaces * 3);

                for (auto ai_face_index = 0; ai_face_index < _ai_mesh.mNumFaces; ++ai_face_index)
                {
                    const auto index = ai_face_index * 3;
                    const auto& ai_face = _ai_mesh.mFaces[ai_face_index];

                    indices[index + 0] = ai_face.mIndices[0];
                    indices[index + 1] = ai_face.mIndices[1];
                    indices[index + 2] = ai_face.mIndices[2];
                }

                return indices;
            }

        private:
            const aiMesh& _ai_mesh;
        };

        class SceneParser
        {
        public:
            explicit SceneParser(std::filesystem::path path)
                : _scene(nullptr)
                , _path(std::move(path))
            {
            }

            [[nodiscard]]
            Model parse_scene() const
            {
                constexpr auto aiPostProcessSteps = aiProcess_Triangulate |
                                                    aiProcess_FlipUVs |
                                                    aiProcess_JoinIdenticalVertices;

                auto importer = Assimp::Importer{};
                _scene = importer.ReadFile(
                    _path.string(),
                    aiPostProcessSteps);
                XAR_THROW_IF(
                    !_scene || (_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !_scene->mRootNode,
                    error::XarException,
                    "Failed to load model from file '{}'",
                    _path.string());

                return {
                    parse_metadata(),
                    parse_meshes(),
                };
            }

        private:
            ModelMetadata parse_metadata() const
            {
                return {
                    std::string{_scene->mName.C_Str()
                }};
            }

            std::vector<Mesh> parse_meshes() const
            {
                std::vector<Mesh> meshes;
                meshes.reserve(_scene->mNumMeshes);

                for (auto ai_mesh_index = 0; ai_mesh_index < _scene->mNumMeshes; ++ai_mesh_index)
                {
                    auto mesh_parser = MeshParser{*_scene->mMeshes[ai_mesh_index]};
                    meshes.push_back(mesh_parser.parse_mesh());
                }

                return meshes;
            }

        private:
            mutable const aiScene* _scene;
            std::filesystem::path _path;
        };
    }

    Model AssimpModelLoader::load_model_from_file(const std::filesystem::path& path) const
    {
        return SceneParser{path}.parse_scene();
    }
}
