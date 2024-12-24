#pragma once

#include <string>
#include <vector>

#include "SDL3/SDL.h"
#include "utils/vectors.h"

/**
 * @brief Represents a shader asset for rendering images using SDL.
 */
struct ShaderAsset {

    SDL_GPUShader *shader;

    std::string shaderFilename;

    Uint32 samplerCount;
    Uint32 uniformBufferCount;
    Uint32 storageBufferCount;
    Uint32 storageTextureCount;

    std::vector<uint8_t> fileData;

};

using Shader = ShaderAsset &;

