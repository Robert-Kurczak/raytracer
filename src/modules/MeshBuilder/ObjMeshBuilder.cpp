#include "ObjMeshBuilder.hpp"

#include "Hittable/Mesh/Mesh.hpp"
#include "Hittable/Triangle/Triangle.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace RTC {

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

    return vertexIndex;
}

void ObjMeshBuilder::parseFace(
    Mesh& mesh,
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

    for (size_t i = 1; i + 1 < indices.size(); ++i) {
        const uint32_t fanBaseIndex = indices[0] - 1;
        const uint32_t fanIndexA = indices[i] - 1;
        const uint32_t fanIndexB = indices[i + 1] - 1;

        auto triangle = std::make_unique<Triangle>(
            vertexBuffer[fanBaseIndex],
            vertexBuffer[fanIndexA],
            vertexBuffer[fanIndexB]
        );

        mesh.addTriangle(std::move(triangle));
    }
}

std::unique_ptr<Mesh> ObjMeshBuilder::buildFromFile(
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

    auto mesh = std::make_unique<Mesh>();

    std::vector<Point3<float>> vertexBuffer {};

    while (std::getline(file, line)) {
        std::stringstream lineStream {line};
        std::string dataType;

        lineStream >> dataType;

        if (dataType == "v") {
            parseVertex(vertexBuffer, position, lineStream);
        } else if (dataType == "f") {
            parseFace(*mesh, vertexBuffer, lineStream);
        }
    }

    return mesh;
}
}