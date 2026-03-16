#pragma once

#include "../IMeshBuilder.hpp"

#include <memory>

namespace RTC {
class ObjMeshBuilder : public IMeshBuilder {
private:
    void parseVertex(
        std::vector<Point3<float>>& vertexBuffer,
        const Vector3<float>& offset,
        std::stringstream& lineStream
    ) const;

    [[nodiscard]] uint32_t parseVertexIndex(
        const std::string& entry,
        uint32_t verticesAmount
    ) const;

    void parseFace(
        Mesh& mesh,
        const std::vector<Point3<float>>& vertexBuffer,
        std::stringstream& lineStream
    ) const;

public:
    [[nodiscard]] std::unique_ptr<Mesh> buildFromFile(
        const std::filesystem::path& path,
        const Vector3<float>& position
    ) const override;
};
}