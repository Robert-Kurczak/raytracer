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

    const uint32_t indexA = indices[0] - 1;
    const uint32_t indexB = indices[1] - 1;
    const uint32_t indexC = indices[2] - 1;

    if (indices.size() == 3) {
        auto triangle = std::make_unique<Triangle>(
            vertexBuffer[indexA],
            vertexBuffer[indexB],
            vertexBuffer[indexC]
        );

        mesh.addTriangle(std::move(triangle));
    } else if (indices.size() == 4) {
        // A-----B
        // |   / |
        // | /   |
        // C-----D
        //
        // Counter-clockwise
        // ACB, BCD

        const uint32_t indexD = indices[3] - 1;

        auto triangle1 = std::make_unique<Triangle>(
            vertexBuffer[indexA],
            vertexBuffer[indexC],
            vertexBuffer[indexB]
        );
        auto triangle2 = std::make_unique<Triangle>(
            vertexBuffer[indexB],
            vertexBuffer[indexC],
            vertexBuffer[indexD]
        );

        mesh.addTriangle(std::move(triangle1));
        mesh.addTriangle(std::move(triangle2));
    } else {
        throw std::runtime_error(
            ".obj parser does not support more than 4 indices in face "
            "the entry"
        );
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