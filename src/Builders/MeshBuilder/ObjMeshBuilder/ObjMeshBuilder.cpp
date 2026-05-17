#include "ObjMeshBuilder.hpp"

#include "Builders/MeshBuilder/MeshBuilderResult.hpp"
#include "Core/Color/Color.hpp"
#include "Geometry/Hittable/Triangle/Triangle.hpp"
#include "Geometry/Light/TriangleAreaLight/TriangleAreaLight.hpp"
#include "Rendering/Material/DiffuseMaterial/DiffuseMaterial.hpp"
#include "Rendering/Material/GlossyMaterial/GlossyMaterial.hpp"
#include "Rendering/Material/MtlParameters.hpp"
#include "Rendering/Material/StandardMaterial/StandardMaterial.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace RTC {
static constexpr DiffuseParameters DEFAULT_MATERIAL_PARAMETERS {
    .baseColor {.red = 0.50F, .green = 0.10F, .blue = 0.40F},
    .emission = LinearColor::black()
};

std::shared_ptr<IMaterial> ObjMeshBuilder::createMaterial(
    const MtlParameters& parameters
) const {
    const auto diffuseMaterial =
        std::make_shared<DiffuseMaterial>(parameters);

    const auto glossyMaterial =
        std::make_shared<GlossyMaterial>(parameters);

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

        } else if (dataType == "Tr") {
            lineStream >> mtlParameters.transparency;
        } else if (dataType == "Ns") {
            lineStream >> mtlParameters.shininess;
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
    std::vector<Point3<float>>& vertexBuffer,
    const Vector3<float>& offset,
    std::stringstream& lineStream
) const {
    float x = NAN;
    float y = NAN;
    float z = NAN;

    lineStream >> x >> y >> z;

    vertexBuffer.push_back(Point3<float> {x, y, z});
    vertexBuffer.back() += offset;
}

uint32_t ObjMeshBuilder::parseVertexIndex(
    const std::string& entry,
    uint32_t verticesAmount
) const {
    const auto slashIndex = entry.find_first_of('/');
    int vertexIndex = 0;

    if (slashIndex != std::string::npos) {
        vertexIndex = std::stoi(entry.substr(0, slashIndex));
    } else {
        vertexIndex = std::stoi(entry);
    }

    if (vertexIndex < 0) {
        return verticesAmount + vertexIndex;
    }

    return vertexIndex - 1;
}

void ObjMeshBuilder::parseFace(
    TriangleBuffer& triangleBuffer,
    AreaLightBuffer& areaLightBuffer,
    const std::shared_ptr<IMaterial>& material,
    const std::vector<Point3<float>>& vertexBuffer,
    std::stringstream& lineStream
) const {
    std::vector<uint32_t> indices {};

    std::string vertexIndex;
    while (lineStream >> vertexIndex) {
        const uint32_t index =
            parseVertexIndex(vertexIndex, vertexBuffer.size());

        indices.push_back(index);
    }

    for (size_t i = 1; i + 1 < indices.size(); i++) {
        const uint32_t fanBaseIndex = indices[0];
        const uint32_t fanIndexA = indices[i];
        const uint32_t fanIndexB = indices[i + 1];

        auto triangle = std::make_unique<Triangle>(
            vertexBuffer[fanBaseIndex],
            vertexBuffer[fanIndexA],
            vertexBuffer[fanIndexB],
            material
        );

        triangleBuffer.push_back(std::move(triangle));

        if (material && not material->getEmission().isBlack()) {
            auto light = std::make_unique<TriangleAreaLight>(
                vertexBuffer[fanBaseIndex],
                vertexBuffer[fanIndexA],
                vertexBuffer[fanIndexB],
                material->getEmission()
            );

            areaLightBuffer.push_back(std::move(light));
        }
    }
}

ObjMeshBuilder::ObjMeshBuilder(std::shared_ptr<ILogger> logger) :
    logger_(std::move(logger)) {}

MeshBuilderResult ObjMeshBuilder::parseMesh(
    const std::filesystem::path& path,
    const Vector3<float>& position
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
    std::vector<Point3<float>> vertexBuffer {};

    std::unordered_map<std::string, std::shared_ptr<IMaterial>> materials;
    std::shared_ptr<IMaterial> currentMaterial =
        std::make_shared<DiffuseMaterial>(DEFAULT_MATERIAL_PARAMETERS);

    while (std::getline(file, line)) {
        std::stringstream lineStream {line};

        std::string dataType;
        lineStream >> dataType;

        if (dataType == "v") {
            parseVertex(vertexBuffer, position, lineStream);
        } else if (dataType == "f") {
            parseFace(
                triangleBuffer,
                areaLightBuffer,
                currentMaterial,
                vertexBuffer,
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
    const Vector3<float>& position
) const {
    logger_->log(LogLevel::Info, "Building mesh from .obj file");

    const auto startTime = std::chrono::high_resolution_clock::now();
    MeshBuilderResult result = parseMesh(path, position);
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