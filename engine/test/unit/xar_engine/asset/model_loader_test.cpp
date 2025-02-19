#include <gtest/gtest.h>

#include <xar_engine/asset/assimp_model_loader.hpp>


namespace
{
    TEST(model_loader_factory, make_loader__returns_existing_object)
    {
        const auto model_loader = xar_engine::asset::ModelLoaderFactory::make_loader();
        EXPECT_NE(model_loader, nullptr);
    }

    TEST(assimp_model_loader, assimp_model_loader)
    {
        xar_engine::asset::AssimpModelLoader model_loader;
        const auto model = model_loader.load_model_from_file("resources/crate.obj");
    }
}