#pragma once

#include "../IMeshBuilder.hpp"
#include "Builders/MeshBuilder/MeshBuilderResult.hpp"
#include "Geometry/Hittable/IHittable.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Geometry/Vertex.hpp"
#include "Rendering/Material/IMaterial.hpp"
#include "Rendering/Material/MtlParameters.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace RTC {
using MaterialsMap =
    std::unordered_map<std::string, std::shared_ptr<IMaterial>>;

using TriangleBuffer = std::vector<std::unique_ptr<IHittable>>;
using AreaLightBuffer = std::vector<std::shared_ptr<ILight>>;

class ObjMeshBuilder : public IMeshBuilder {
private:
    struct FaceIndices {
        uint32_t vertexIndex = 0;
        std::optional<uint32_t> textureCoordIndex;
        std::optional<uint32_t> normalIndex;
    };

    std::shared_ptr<ILogger> logger_;

    [[nodiscard]] bool isMaterialTransparent(
        const MtlParameters& parameters
    ) const;

    [[nodiscard]] std::shared_ptr<IMaterial> createMaterial(
        const MtlParameters& parameters
    ) const;

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
        std::vector<Point3f>& vertexBuffer,
        const Vector3f& offset,
        std::stringstream& lineStream
    ) const;

    void parseTextureCoord(
        std::vector<Point2f>& textureCoordsBuffer,
        std::stringstream& lineStream
    ) const;

    void parseVertexNormal(
        std::vector<Vector3f>& normalBuffer,
        std::stringstream& lineStream
    ) const;

    [[nodiscard]] uint32_t convertObjIndexToBufferIndex(
        int objIndex,
        uint32_t bufferSize
    ) const;

    [[nodiscard]] FaceIndices parseFaceIndices(
        const std::string& entry,
        uint32_t verticesAmount,
        uint32_t textureCoordsAmount,
        uint32_t normalsAmount
    ) const;

    [[nodiscard]] Vertex convertFaceIndicesToVertex(
        const FaceIndices& faceIndices,
        const std::vector<Point3f>& vertexBuffer,
        const std::vector<Point2f>& textureCoordsBuffer,
        const std::vector<Vector3f>& normalsBuffer
    ) const;

    void parseFace(
        TriangleBuffer& triangleBuffer,
        AreaLightBuffer& areaLightBuffer,
        const std::vector<Point3f>& vertexBuffer,
        const std::vector<Point2f>& textureCoordsBuffer,
        const std::vector<Vector3f>& normalsBuffer,
        const std::shared_ptr<IMaterial>& material,
        std::stringstream& lineStream
    ) const;

    [[nodiscard]] MeshBuilderResult parseMesh(
        const std::filesystem::path& path,
        const Vector3f& position
    ) const;

public:
    ObjMeshBuilder(std::shared_ptr<ILogger> logger);

    [[nodiscard]] MeshBuilderResult buildFromFile(
        const std::filesystem::path& path,
        const Vector3f& position
    ) const override;
};
}