#pragma once

#include "../IMeshBuilder.hpp"
#include "Builders/MeshBuilder/MeshBuilderResult.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Material/IMaterial.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace RTC {
using MaterialsMap =
    std::unordered_map<std::string, std::shared_ptr<IMaterial>>;

using TriangleBuffer = std::vector<std::unique_ptr<IHittable>>;
using AreaLightBuffer = std::vector<std::unique_ptr<ILight>>;

class ObjMeshBuilder : public IMeshBuilder {
private:
    std::shared_ptr<ILogger> logger_;

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
        TriangleBuffer& triangleBuffer,
        AreaLightBuffer& areaLightBuffer,
        const std::shared_ptr<IMaterial>& material,
        const std::vector<Point3<float>>& vertexBuffer,
        std::stringstream& lineStream
    ) const;

    [[nodiscard]] MeshBuilderResult parseMesh(
        const std::filesystem::path& path,
        const Vector3<float>& position
    ) const;

public:
    ObjMeshBuilder(std::shared_ptr<ILogger> logger);

    [[nodiscard]] MeshBuilderResult buildFromFile(
        const std::filesystem::path& path,
        const Vector3<float>& position
    ) const override;
};
}