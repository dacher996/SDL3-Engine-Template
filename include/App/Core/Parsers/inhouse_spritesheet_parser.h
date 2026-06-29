#ifndef APP_INHOUSE_SPRITESHEET_PARSER_H
#define APP_INHOUSE_SPRITESHEET_PARSER_H

#include "Engine/Core/texture_region_parser.h"
#include <string>
#include <vector>
#include <utility>
#include "SDL3/SDL_stdinc.h"

namespace YourProject {
    /// Parses an in-house JSON format to extract texture regions.
    class InhouseSpritesheetParser : public Engine::TextureRegionParser {
    public:
        /// Parser that takes in the path to the json file containing the texture region data and the textureId
        /// which represents the id of the texture registered inside the TextureManager
        InhouseSpritesheetParser(std::string jsonPath, Uint16 textureId);

        ~InhouseSpritesheetParser() override = default;

        /// Parses the file and returns the regions
        std::vector<std::pair<Uint32, Engine::TextureRegion> > Parse() override;

    private:
        std::string m_jsonPath;
        Uint16 m_textureId;
    };
} // namespace Engine

#endif // APP_INHOUSE_SPRITESHEET_PARSER_H
