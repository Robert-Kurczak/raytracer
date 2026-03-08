#pragma once

#include "MeshBuilder/IMeshBuilder.hpp"

#include <memory>

namespace RTC {
class ObjMeshBuilder : public IMeshBuilder {
private:
    uint32_t parseFaceEntry(const std::string& entry) const;

public:
    [[nodiscard]] std::unique_ptr<Mesh> buildFromFile(
        const std::filesystem::path& path
    ) const override;
};
}