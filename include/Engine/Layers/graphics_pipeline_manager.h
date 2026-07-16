#ifndef ENGINE_GRAPHICS_PIPELINE_MANAGER_H
#define ENGINE_GRAPHICS_PIPELINE_MANAGER_H

#include <string>
#include <unordered_map>
#include <utility>

#include "Engine/Rendering/graphics_pipeline_creation_info.h"
#include "SDL3/SDL_gpu.h"

namespace Engine {
    /// Stores and manages the lifecycle of GPU Graphics Pipelines
    class GraphicsPipelineManager {
    public :
        /// Registers a graphics pipeline under the specific id.
        ///
        /// Returns success if the graphics pipeline was registered. If the id was already taken, the pipeline won't be
        /// registered and the function will return with `false`.
        bool RegisterPipeline(Uint16 id, SDL_GPUGraphicsPipeline *pipeline);

        /// Frees a graphics pipeline with a specific id from memory.
        void UnloadPipeline(Uint16 id);

        /// Returns the pipeline previously registered under the provided id or nullptr if the id is invalid.
        SDL_GPUGraphicsPipeline *GetPipeline(Uint16 id);

        /// Frees all pipelines from memory
        void Cleanup();

        /// Sets the default (single-texture) and default-multilayered (texture-array) pipeline ids. These are
        /// tracked as a pair so Default2DRenderPass can pick whichever of the two actually matches a draw's
        /// bound texture, correcting a mismatched material in either direction.
        void SetDefaultPipeline(Uint16 defaultId, Uint16 multilayeredId);

        /// Retrieves the {default, multilayered} pipeline id pair set via SetDefaultPipeline
        [[nodiscard]] std::pair<Uint16, Uint16> GetDefaultPipeline() const;

        /// Loads a shader from file and returns a pointer to it.
        static SDL_GPUShader *LoadShader(const std::string &shaderFilename,
                                         Uint32 samplerCount = 0,
                                         Uint32 uniformBufferCount = 0,
                                         Uint32 storageBufferCount = 0,
                                         Uint32 storageTextureCount = 0);

        /// Frees the shader from memory
        static void UnloadShader(SDL_GPUShader *shader);

        /// Creates a graphics pipeline from provided vertex/fragment shader and returns a pointer to it.
        static SDL_GPUGraphicsPipeline *CreateGraphicsPipeline(SDL_GPUShader *vertShader, SDL_GPUShader *fragShader,
                                                               const GraphicsPipelineCreationInfo &creationInfo);

    private:
        std::unordered_map<Uint16, SDL_GPUGraphicsPipeline *> m_pipelines;
        Uint16 m_defaultPipelineId{0};
        Uint16 m_multilayeredPipelineId{0};
    };
}

#endif //ENGINE_GRAPHICS_PIPELINE_MANAGER_H
