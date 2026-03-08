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
uint32_t ObjMeshBuilder::parseFaceEntry(const std::string& entry) const {
    const uint32_t slashIndex = entry.find_first_of('/');
    return std::stoi(entry.substr(0, slashIndex));
}

std::unique_ptr<Mesh> ObjMeshBuilder::buildFromFile(
    const std::filesystem::path& path
) const {
    std::ifstream file {path};

    if (!file) {
        throw std::runtime_error(
            std::format("Cannot open {}", path.c_str())
        );
    }

    std::string line;

    auto mesh = std::make_unique<Mesh>();

    std::vector<Point3<float>> vertices {};

    while (std::getline(file, line)) {
        std::stringstream lineStream {line};
        std::string dataType;

        lineStream >> dataType;

        if (dataType == "v") {
            float x = NAN;
            float y = NAN;
            float z = NAN;

            lineStream >> x >> y >> z;

            vertices.push_back(Point3<float> {x, y, z});
        } else if (dataType == "f") {
            std::string vertexIndex;

            lineStream >> vertexIndex;
            const uint32_t indexI = parseFaceEntry(vertexIndex);

            lineStream >> vertexIndex;
            const uint32_t indexJ = parseFaceEntry(vertexIndex);

            lineStream >> vertexIndex;
            const uint32_t indexK = parseFaceEntry(vertexIndex);

            auto triangle = std::make_unique<Triangle>(
                vertices[indexI - 1],
                vertices[indexJ - 1],
                vertices[indexK - 1]
            );

            mesh->addTriangle(std::move(triangle));
        }
    }

    return mesh;
}
}