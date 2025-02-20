#include <gtest/gtest.h>

#include <xar_engine/asset/stb_image_loader.hpp>


namespace
{
    TEST(image_loader_factory,
         make_loader__returns_existing_object)
    {
        const auto loader = xar_engine::asset::ImageLoaderFactory::make_loader();
        EXPECT_NE(loader,
                  nullptr);
    }

    TEST(stb_model_loader,
         load_small_quad_png__loaded_png_is_correct)
    {
        xar_engine::asset::StbImageLoader loader;
        const auto model = loader.load_image_from_file("resources/colorful_quad.png");

        const auto expected_bytes = std::vector<std::uint8_t>{
            237, 28, 36, 255,
            34, 177, 76, 255,
            0, 162, 232, 255,
            255, 242, 0, 255,
            163, 73, 164, 255,
            237, 28, 36, 255,
            255, 242, 0, 255,
            181, 230, 29, 255,
            185, 122, 87, 255,
            255, 242, 0, 255,
            237, 28, 36, 255,
            63, 72, 204, 255,
            255, 242, 0, 255,
            239, 228, 176, 255,
            136, 0, 21, 255,
            237, 28, 36, 255
        };

        EXPECT_EQ(model.pixel_width,
                  4);
        EXPECT_EQ(model.pixel_height,
                  4);
        EXPECT_EQ(model.channel_count,
                  4);
        EXPECT_EQ(model.bytes,
                  expected_bytes);
    }
}