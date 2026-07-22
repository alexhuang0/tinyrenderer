#include "tgaimage.h"
#include <array>
#include <cmath>
#include <vector>

#include <charconv>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

namespace Canvas {
constexpr int width{64};
constexpr int height{64};
constexpr double midWidth{(width - 1) / 2};
constexpr double midHeight{(width - 1) / 2};
TGAImage framebuffer(width, height, TGAImage::RGB);
} // namespace Canvas

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer,
          TGAColor color) {
  // check for steep vertical
  bool steep{};
  if (std::abs(by - ay) > std::abs(bx - ax)) {
    steep = true;
    std::swap(ax, ay);
    std::swap(bx, by);
  }

  if (ax > bx) {
    std::swap(ax, bx);
    std::swap(ay, by);
  }

  int y{ay};
  int ierror{};
  for (int x = ax; x <= bx; ++x) {
    if (steep) {
      framebuffer.set(y, x, color);
    } else {
      framebuffer.set(x, y, color);
    }

    ierror += 2 * (std::abs(by - ay));
    y += by > ay ? 1 : -1 * (ierror > (bx - ax));
    ierror -= 2 * (bx - ax) * (ierror > (bx - ax));
  }
}

class Vertex {
  int x_{};
  int y_{};
  double z_{};

public:
  Vertex(int x, int y, double z)
      : x_{x}
      , y_{y}
      , z_{z} {}

  Vertex() = default;

  int x() const { return x_; }

  int y() const { return y_; }

  double z() const { return z_; }

  void x(int x) { x_ = x; }

  void y(int y) { y_ = y; }

  void z(double z) { z_ = z; }
};

class Face {
  std::vector<Vertex> vertices_{};

public:
  Face(const std::vector<Vertex> &vertices)
      : vertices_{vertices} {}

  const std::vector<Vertex> &getVertices() const { return vertices_; }

  void setVertices(const std::vector<Vertex> &vertices) {
    vertices_ = vertices;
  }

  void drawFaceBorders() {
    line(vertices_[0].x(), vertices_[0].y(), vertices_[1].x(), vertices_[1].y(),
         Canvas::framebuffer, red);
    line(vertices_[0].x(), vertices_[0].y(), vertices_[2].x(), vertices_[2].y(),
         Canvas::framebuffer, red);
    line(vertices_[2].x(), vertices_[2].y(), vertices_[1].x(), vertices_[1].y(),
         Canvas::framebuffer, red);
  }
};

int convertModelCoordsToWorld(double model, int mid) {
  return std::round(model * 2 * mid + mid);
}

int main(int argc, char **argv) {
  std::ifstream objFile("obj/floor.obj");

  std::vector<Vertex> vertices{};

  std::string curLine{};
  std::istringstream stream{};

  while (objFile.good()) {
    std::getline(objFile, curLine);
    stream.str(curLine);

    std::string lineStart{};
    std::getline(stream, lineStart, ' ');

    if (lineStart == "v") {
      // v 0.608654 -0.568839 -0.416318
      double x{};
      double y{};
      double z{};

      stream >> x >> y >> z;

      vertices.push_back({convertModelCoordsToWorld(x, Canvas::midWidth),
                          convertModelCoordsToWorld(y, Canvas::midHeight), z});

    } else if (lineStart == "f") {
      // f 1193/1240/1193 1180/1227/1180 1179/1226/1179
      std::string vCluster{}; // "v1/v2/v3"
      while (std::getline(stream, vCluster)) {
        std::istringstream vClusterStream(vCluster);
        std::vector<int> vIndices{};
        std::string token;

        while (std::getline(vClusterStream, token)) {
          vIndices.push_back(std::stoi(token) - 1);
        }

        // assumes vertices already read
        Face face({vertices[vIndices[0]], vertices[vIndices[1]],
                   vertices[vIndices[2]]});
        face.drawFaceBorders();
      }
    }
  }

  objFile.close();

  Canvas::framebuffer.write_tga_file("framebuffer.tga");
  return 0;
}