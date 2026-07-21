#include "tgaimage.h"
#include <cmath>
#include <vector>

#include <fstream>
#include <iostream>
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
} // namespace Canvas

class Vertex {
  int x_{};
  int y_{};
  double z_{};

public:
  Vertex(int x, int y, double z)
      : x_{x}
      , y_{y}
      , z_{z}
  {
  }

  Vertex() = default;

  int x() const { return x_; }

  int y() const { return y_; }

  double z() const { return z_; }

  void x(int x) { x_ = x; }

  void y(int y) { y_ = y; }

  void z(double z) { z_ = z; }
};

class Face {
  Vertex v1_{};
  Vertex v2_{};
  Vertex v3_{};

public:
  Face(Vertex v1, Vertex v2, Vertex v3)
      : v1_{v1}
      , v2_{v2}
      , v3_{v3}
  {
  }

  Vertex v1() const { return v1_; }

  Vertex v2() const { return v2_; }

  Vertex v3() const { return v3_; }

  void v1(const Vertex &v1) { v1_ = v1; }

  void v2(const Vertex &v2) { v2_ = v2; }

  void v3(const Vertex &v3) { v3_ = v3; }
};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color)
{
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

void convertModelVertexToWorld(Vertex &vertex)
{
  using namespace Canvas;
  vertex.x(std::round(vertex.x() * width + midWidth));
  vertex.y(std::round(vertex.y() * height + midHeight));
}

int main(int argc, char **argv)
{
  TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);

  std::ifstream objFile("obj/diablo3_pose/diablo3_pose.obj");

  std::vector<Vertex> vertices{};

  std::string curLine{};
  std::istringstream stream{};

  while (objFile.good()) {
    std::getline(objFile, curLine);
    stream.str(curLine);

    std::string lineStart{};
    std::getline(stream, lineStart, ' ');

    if (lineStart == "v") {
    } else if (lineStart == "f") {
    }
  }

  objFile.close();

  framebuffer.write_tga_file("framebuffer.tga");
  return 0;
}