#include <array>
#include <cmath>
#include <vector>

#include <charconv>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <random>
#include <numbers>

#include "tgaimage.h"

#include "canvas.h"
#include "model.h"
#include "our_gl.h"
#include "shaders.h"

int main(int argc, char** argv) {
	using namespace Canvas;

	std::vector<Model> scene_models;
	scene_models.emplace_back("diablo3_pose");
	scene_models.emplace_back("floor");


	// main camera
	RenderContext rContext{};
	rContext.lookat(eye, center, up);
	rContext.init_perspective(norm(eye - center));
	rContext.init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
	rContext.init_zbuffer();
	TGAImage finalbuffer(width, height, TGAImage::RGB);

	// one render to freeze zbuffer from camera pov (zbuffer will never change for that camera pov)
	for (const auto& model : scene_models) {
		ShadowShader firstPassShader(model, rContext);
		for (int f{ 0 }; f < model.nfaces(); ++f) {
			Triangle triang_vertices{
				firstPassShader.vertex(f, 0),
				firstPassShader.vertex(f, 1),
				firstPassShader.vertex(f, 2)
			};

			rContext.rasterize(triang_vertices, firstPassShader, finalbuffer);
		}
	}

	// SSAO
	constexpr double radius{ 0.1 };
	std::mt19937 mt{ 10 };
	std::uniform_real_distribution<double> getDist{ -radius, radius };
	constexpr int samples{ 128 };

	const matrix<4, 4> eye_to_screen{ rContext.Viewport * rContext.Perspective };
	const matrix<4, 4> screen_to_eye{ (eye_to_screen).inverse() };
	for (int x{ 0 }; x < width; ++x) {
		for (int y{ 0 }; y < height; ++y) {
			const double z = rContext.zbuffer[y + x * width];
			if (z < 1e-5) continue; // open sky / background, no occlusion

			vec4 coords_eye_space{ screen_to_eye * vec4{
				static_cast<double>(x), static_cast<double>(y), z, 1.
			} };

			// sampling
			int voters{}; // valid, in-bound samples
			int votes{}; // occluded samples
			for (int i{ 0 }; i < samples; ++i) {
				double dist{ getDist(mt) };
				vec4 rand_coord{ x + dist, y + dist, z + dist, 1. };

				// out of bounds check
				if (rand_coord.x < 0 || rand_coord.x > width
					|| rand_coord.y < 0 || rand_coord.y > height)
				{
					continue;
				}

				vec4 rand_coord_screen{ eye_to_screen * rand_coord };
				if (rand_coord_screen.z < 1e-5) continue; // open sky / background, no occlusion
				if (rand_coord_screen.z < rContext.zbuffer[rand_coord_screen.y + rand_coord_screen.x + width]) {
					++votes; // this sample is occluded
				}

				++voters;
			}

			// scale down occlusion contribution
			const double occlusion_factor{ 0.4 * votes / voters };
			const double ambient{ 1. - occlusion_factor }; // how lit this point is (1 = max bright)
			TGAColor screen_color{ finalbuffer.get(x, y) };
			finalbuffer.set(x, y, TGAColor{
				static_cast<std::uint8_t>(screen_color[0] * ambient),
				static_cast<std::uint8_t>(screen_color[1] * ambient),
				static_cast<std::uint8_t>(screen_color[2] * ambient),
				screen_color[3]
				});
		}
	}


	finalbuffer.write_tga_file("framebuffer.tga");
	return 0;
}
