#pragma once

#include "aliases.hpp"
#include <array>
#include <cstdint>

struct Texture
{
    TEXTURE id;
    const char** name;
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t num_tiles_width;
    std::uint32_t num_tiles_height;
    float offset_x_weird_math;
    float offset_y_weird_math;
    float tile_width_fraction;
    float tile_height_fraction;
    float tile_width_minus_one_fraction;
    float tile_height_minus_one_fraction;
    float one_over_width;
    float one_over_height;
};

struct Textures
{
    std::uint32_t num_textures;
    std::array<Texture, 0x192> textures;
    std::array<Texture*, 0x192> texture_map;
};

Textures* get_textures();
