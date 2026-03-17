#pragma once

#include "../IMeshBuilder.hpp"
#include "Geometry/Material/IMaterial.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace RTC {
using MaterialsMap =
    std::unordered_map<std::string, std::shared_ptr<IMaterial>>;

class ObjMeshBuilder : public IMeshBuilder {
private:
    [[nodiscard]] MaterialsMap extractMaterials(
        const std::filesystem::path& path
    ) const;

    void parseMtlLib(
        MaterialsMap& materials,
        const std::filesystem::path& objPath,
        std::stringstream& lineStream
    ) const;

    void updateUsedMaterial(
        std::shared_ptr<IMaterial>& currentMaterial,
        const MaterialsMap& materials,
        std::stringstream& lineStream
    ) const;

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
        const std::shared_ptr<IMaterial>& material,
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