#include <gtest/gtest.h>

#include <xar_engine/asset/assimp_model_loader.hpp>

#include <xar_engine/math/vector.hpp>


namespace
{
    TEST(model_loader_factory,
         make_loader__returns_existing_object)
    {
        const auto loader = xar_engine::asset::ModelLoaderFactory().make();
        EXPECT_NE(loader,
                  nullptr);
    }

    TEST(assimp_model_loader,
         load_simple_obj__loaded_model_is_correct)
    {
        xar_engine::asset::AssimpModelLoader loader;
        const auto model = loader.load_model_from_file("resources/crate.obj");

        ASSERT_EQ(model.meshes.size(),
                  std::size_t{1});

        ASSERT_EQ(model.meshes[0].positions.size(),
                  std::size_t{24});

        const auto expected_normals = std::vector<xar_engine::math::Vector3f>{};
        ASSERT_EQ(model.meshes[0].normals,
                  expected_normals);

        ASSERT_EQ(model.meshes[0].texture_coords.size(),
                  std::size_t{1});
        ASSERT_EQ(model.meshes[0].texture_coords[0].channel_count,
                  std::size_t{2});
        ASSERT_EQ(model.meshes[0].texture_coords[0].coords.size(),
                  std::size_t{48});

        ASSERT_EQ(model.meshes[0].indices.size(),
                  std::size_t{36});
    }
}