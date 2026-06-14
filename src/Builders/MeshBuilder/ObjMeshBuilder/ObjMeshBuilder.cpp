#include "ObjMeshBuilder.hpp"

#include "Builders/MeshBuilder/MeshBuilderResult.hpp"
#include "Builders/MeshBuilder/ObjMeshBuilder/MtlParameters.hpp"
#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/Triangle/Triangle.hpp"
#include "Geometry/Light/TriangleAreaLight/TriangleAreaLight.hpp"
#include "Geometry/Vertex.hpp"
#include "Rendering/Material/DiffuseMaterial/DiffuseMaterial.hpp"
#include "Rendering/Material/DiffuseMaterial/DiffuseParameters.hpp"
#include "Rendering/Material/GlossyMaterial/GlossyMaterial.hpp"
#include "Rendering/Material/GlossyMaterial/GlossyParameters.hpp"
#include "Rendering/Material/StandardMaterial/StandardMaterial.hpp"
#include "Rendering/Material/TransparentMaterial/TransparentMaterial.hpp"
#include "Rendering/Material/TransparentMaterial/TransparentParameters.hpp"
#include "Rendering/Texture/ColorTexture/ColorTexture.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Utils/Logger/ILogger.hpp"
#include "Utils/StringUtils.hpp"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace RTC {
bool ObjMeshBuilder::isMaterialTransparent(
    const MtlParameters& parameters
) const {
    const bool isTransparentMode = parameters.illuminationModel == 7 or
                                   parameters.illuminationModel == 8 or
                                   parameters.illuminationModel == 9;

    if (isTransparentMode) {
        return true;
    }

    const bool containsTransparentPrams =
        not parameters.transmisionFilter.isBlack() and
        parameters.refractionIndex > 1.0F;

    return containsTransparentPrams;
}

std::shared_ptr<IMaterial> ObjMeshBuilder::createMaterial(
    const MtlParameters& parameters
) const {
    if (isMaterialTransparent(parameters)) {
        const TransparentParameters transparentParameters {
            .refractionIndex = parameters.refractionIndex,
            .emission = parameters.emission / float(std::numbers::pi),
            .transmissionColor =
                UNIT_LINEAR_COLOR - parameters.transmisionFilter
        };

        return std::make_shared<TransparentMaterial>(
            transparentParameters
        );
    }

    std::shared_ptr<ITexture> diffuseTexture;

    if (not parameters.diffuseTexturePath.empty()) {
        diffuseTexture = imageTextureBuilder_->buildFromFile(
            parameters.diffuseTexturePath
        );
    } else {
        diffuseTexture =
            std::make_shared<ColorTexture>(parameters.diffuse);
    }

    const DiffuseParameters diffuseParameters {
        .colorTexture = diffuseTexture,
        .emission = parameters.emission / float(std::numbers::pi)
    };

    const auto diffuseMaterial =
        std::make_shared<DiffuseMaterial>(diffuseParameters);

    const GlossyParameters glossyParameters {
        .roughness = std::sqrt(2.0F / (parameters.shininess + 2.0F)),
        .fresnelBaseTerm =
            LinearColor {.red = 0.80F, .green = 0.80F, .blue = 0.80F}
    };

    const auto glossyMaterial =
        std::make_shared<GlossyMaterial>(glossyParameters);

    const float glossyBlendFactor =
        parameters.specular.getLargestComponent();

    return std::make_unique<StandardMaterial>(
        diffuseMaterial, glossyMaterial, glossyBlendFactor
    );
}

MaterialsMap ObjMeshBuilder::extractMaterials(
    const std::filesystem::path& path
) const {
    std::ifstream file {path};

    if (!file) {
        throw std::runtime_error(
            std::format("Cannot open {}", path.c_str())
        );
    }

    std::unordered_map<std::string, std::shared_ptr<IMaterial>>
        materials_;

    std::string mtlName;
    MtlParameters mtlParameters;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream lineStream {line};
        std::string dataType;

        lineStream >> dataType;

        if (dataType == "newmtl") {
            if (!mtlName.empty()) {
                materials_[mtlName] = createMaterial(mtlParameters);
            }

            lineStream >> mtlName;
            mtlParameters = MtlParameters {};
        } else if (dataType == "Ka") {
            lineStream >> mtlParameters.ambient.red;
            lineStream >> mtlParameters.ambient.green;
            lineStream >> mtlParameters.ambient.blue;
        } else if (dataType == "Kd") {
            lineStream >> mtlParameters.diffuse.red;
            lineStream >> mtlParameters.diffuse.green;
            lineStream >> mtlParameters.diffuse.blue;
        } else if (dataType == "Ks") {
            lineStream >> mtlParameters.specular.red;
            lineStream >> mtlParameters.specular.green;
            lineStream >> mtlParameters.specular.blue;
        } else if (dataType == "Ke") {
            lineStream >> mtlParameters.emission.red;
            lineStream >> mtlParameters.emission.green;
            lineStream >> mtlParameters.emission.blue;
        } else if (dataType == "Tf") {
            lineStream >> mtlParameters.transmisionFilter.red;
            lineStream >> mtlParameters.transmisionFilter.green;
            lineStream >> mtlParameters.transmisionFilter.blue;
        } else if (dataType == "Tr") {
            lineStream >> mtlParameters.transparency;
        } else if (dataType == "Ns") {
            lineStream >> mtlParameters.shininess;
        } else if (dataType == "Ni") {
            lineStream >> mtlParameters.refractionIndex;
        } else if (dataType == "illum") {
            int illum = 0;
            lineStream >> illum;
            mtlParameters.illuminationModel = uint8_t(illum);
        } else if (dataType == "map_Kd") {
            lineStream >> mtlParameters.diffuseTexturePath;
            mtlParameters.diffuseTexturePath =
                path.parent_path() / mtlParameters.diffuseTexturePath;
        }
    }

    if (!mtlName.empty()) {
        materials_[mtlName] = createMaterial(mtlParameters);
    }

    return materials_;
}

void ObjMeshBuilder::parseMtlLib(
    MaterialsMap& materials,
    const std::filesystem::path& objPath,
    std::stringstream& lineStream
) const {
    std::string libName;
    lineStream >> libName;
    const std::filesystem::path mtlPath = objPath.parent_path() / libName;

    const MaterialsMap newMaterials = extractMaterials(mtlPath);
    materials.insert(newMaterials.begin(), newMaterials.end());
}

void ObjMeshBuilder::updateUsedMaterial(
    std::shared_ptr<IMaterial>& currentMaterial,
    const MaterialsMap& materials,
    std::stringstream& lineStream
) const {
    std::string name;
    lineStream >> name;

    if (!materials.contains(name)) {
        std::cerr << "Ignoring undefined material: '" << name << "'\n";
    } else {
        currentMaterial = materials.at(name);
    }
}

void ObjMeshBuilder::parseVertex(
    std::vector<Point3f>& vertexBuffer,
    const Vector3f& offset,
    std::stringstream& lineStream
) const {
    float x = NAN;
    float y = NAN;
    float z = NAN;

    lineStream >> x >> y >> z;

    vertexBuffer.push_back(Point3f {x, y, z});
    vertexBuffer.back() += offset;
}

void ObjMeshBuilder::parseTextureCoord(
    std::vector<Point2f>& textureCoordsBuffer,
    std::stringstream& lineStream
) const {
    float u = NAN;
    float v = NAN;

    lineStream >> u >> v;

    textureCoordsBuffer.push_back(Point2f {u, v});
}

void ObjMeshBuilder::parseVertexNormal(
    std::vector<Vector3f>& normalBuffer,
    std::stringstream& lineStream
) const {
    float x = NAN;
    float y = NAN;
    float z = NAN;

    lineStream >> x >> y >> z;

    normalBuffer.push_back(Point3f {x, y, z});
}

uint32_t ObjMeshBuilder::convertObjIndexToBufferIndex(
    int objIndex,
    uint32_t bufferSize
) const {
    assert(objIndex != 0);

    if (objIndex < 0) {
        return bufferSize + objIndex;
    }

    return objIndex - 1;
}

ObjMeshBuilder::FaceIndices ObjMeshBuilder::parseFaceIndices(
    const std::string& entry,
    uint32_t verticesAmount,
    uint32_t textureCoordsAmount,
    uint32_t normalsAmount
) const {
    const std::vector<std::string> entries = split(entry, '/');

    FaceIndices result {};

    result.vertexIndex = convertObjIndexToBufferIndex(
        std::stoi(entries[0]), verticesAmount
    );

    if (entries.size() > 1 and not entries[1].empty()) {
        result.textureCoordIndex = convertObjIndexToBufferIndex(
            std::stoi(entries[1]), textureCoordsAmount
        );
    }

    if (entries.size() > 2 and not entries[2].empty()) {
        result.normalIndex = convertObjIndexToBufferIndex(
            std::stoi(entries[2]), normalsAmount
        );
    }

    return result;
}

Vertex ObjMeshBuilder::convertFaceIndicesToVertex(
    const FaceIndices& faceIndices,
    const std::vector<Point3f>& vertexBuffer,
    const std::vector<Point2f>& textureCoordsBuffer,
    const std::vector<Vector3f>& normalsBuffer
) const {
    std::optional<Point2f> texturePosition;
    std::optional<Vector3f> normal;

    if (faceIndices.textureCoordIndex) {
        texturePosition =
            textureCoordsBuffer[faceIndices.textureCoordIndex.value()];
    }

    if (faceIndices.normalIndex) {
        normal = normalsBuffer[faceIndices.normalIndex.value()];
    }

    return Vertex {
        .position = vertexBuffer[faceIndices.vertexIndex],
        .texturePosition = texturePosition,
        .normal = normal
    };
}

void ObjMeshBuilder::parseFace(
    TriangleBuffer& triangleBuffer,
    AreaLightBuffer& areaLightBuffer,
    const std::vector<Point3f>& vertexBuffer,
    const std::vector<Point2f>& textureCoordsBuffer,
    const std::vector<Point3f>& normalsBuffer,
    const std::shared_ptr<IMaterial>& material,
    std::stringstream& lineStream
) const {
    std::vector<FaceIndices> indices {};

    std::string indicesEntry;
    while (lineStream >> indicesEntry) {
        const FaceIndices faceIndices = parseFaceIndices(
            indicesEntry,
            vertexBuffer.size(),
            textureCoordsBuffer.size(),
            normalsBuffer.size()
        );

        indices.push_back(faceIndices);
    }

    for (size_t i = 1; i + 1 < indices.size(); i++) {
        const FaceIndices fanBaseIndex {indices[0]};
        const FaceIndices fanIndexA = indices[i];
        const FaceIndices fanIndexB = indices[i + 1];

        const Vertex baseVertex = convertFaceIndicesToVertex(
            fanBaseIndex, vertexBuffer, textureCoordsBuffer, normalsBuffer
        );

        const Vertex vertexA = convertFaceIndicesToVertex(
            fanIndexA, vertexBuffer, textureCoordsBuffer, normalsBuffer
        );

        const Vertex vertexB = convertFaceIndicesToVertex(
            fanIndexB, vertexBuffer, textureCoordsBuffer, normalsBuffer
        );

        auto triangle = std::make_unique<Triangle>(
            baseVertex, vertexA, vertexB, material
        );

        triangleBuffer.push_back(std::move(triangle));

        if (material && not material->getEmission().isBlack()) {
            auto light = std::make_shared<TriangleAreaLight>(
                baseVertex.position,
                vertexA.position,
                vertexB.position,
                material->getEmission()
            );

            areaLightBuffer.push_back(std::move(light));
        }
    }
}

ObjMeshBuilder::ObjMeshBuilder(
    std::shared_ptr<ILogger> logger,
    std::unique_ptr<IImageTextureBuilder> imageTextureBuilder
) :
    logger_(std::move(logger)),
    imageTextureBuilder_(std::move(imageTextureBuilder)) {}

MeshBuilderResult ObjMeshBuilder::parseMesh(
    const std::filesystem::path& path,
    const Vector3f& position,
    std::shared_ptr<IMaterial> defaultMaterial
) const {
    std::ifstream file {path};

    if (!file) {
        throw std::runtime_error(
            std::format("Cannot open {}", path.c_str())
        );
    }

    std::string line;

    TriangleBuffer triangleBuffer {};
    AreaLightBuffer areaLightBuffer {};
    std::vector<Point3f> vertexBuffer {};
    std::vector<Point2f> textureCoordsBuffer {};
    std::vector<Vector3f> normalsBuffer {};

    std::unordered_map<std::string, std::shared_ptr<IMaterial>> materials;
    std::shared_ptr<IMaterial> currentMaterial =
        std::move(defaultMaterial);

    while (std::getline(file, line)) {
        std::stringstream lineStream {line};

        std::string dataType;
        lineStream >> dataType;

        if (dataType == "v") {
            parseVertex(vertexBuffer, position, lineStream);
        } else if (dataType == "vn") {
            parseVertexNormal(normalsBuffer, lineStream);
        } else if (dataType == "vt") {
            parseTextureCoord(textureCoordsBuffer, lineStream);
        } else if (dataType == "f") {
            parseFace(
                triangleBuffer,
                areaLightBuffer,
                vertexBuffer,
                textureCoordsBuffer,
                normalsBuffer,
                currentMaterial,
                lineStream
            );
        } else if (dataType == "mtllib") {
            parseMtlLib(materials, path, lineStream);
        } else if (dataType == "usemtl") {
            updateUsedMaterial(currentMaterial, materials, lineStream);
        }
    }

    return MeshBuilderResult {
        .triangles = std::move(triangleBuffer),
        .areaLights = std::move(areaLightBuffer)
    };
}

MeshBuilderResult ObjMeshBuilder::buildFromFile(
    const std::filesystem::path& path,
    const Vector3f& position,
    std::shared_ptr<IMaterial> defaultMaterial
) const {
    logger_->log(LogLevel::Info, "Building mesh from .obj file");

    const auto startTime = std::chrono::high_resolution_clock::now();
    MeshBuilderResult result = parseMesh(path, position, defaultMaterial);
    const auto endTime = std::chrono::high_resolution_clock::now();

    const auto executionTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime
        );

    logger_->log(
        LogLevel::Info,
        std::format("Triangles in mesh: {}", result.triangles.size())
    );
    logger_->log(
        LogLevel::Info,
        std::format("Area lights in mesh: {}", result.areaLights.size())
    );
    logger_->log(
        LogLevel::Info, std::format("Mesh built in {}", executionTime)
    );

    return result;
}
}