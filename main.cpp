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
	TGAImage normalbuffer(width, height, TGAImage::RGB); // stores normal vector of the closest obj at given pixel
	for (const auto& model : scene_models) {
		SSAOShader firstPassShader(model, rContext);
		for (int f{ 0 }; f < model.nfaces(); ++f) {
			Triangle triang_vertices{
				firstPassShader.vertex(f, 0),
				firstPassShader.vertex(f, 1),
				firstPassShader.vertex(f, 2)
			};

			rContext.rasterize(triang_vertices, firstPassShader, normalbuffer);
		}
	}

	// SSAO
	constexpr double delta{ 0.1 };
	std::mt19937 mt{ 10 };
	std::uniform_real_distribution<double> getPhi{ 0, 2 * std::numbers::pi };
	std::uniform_real_distribution<double> getDist{ -1, 1. }; // full sphere, dot product check will converge to hemisphere
	std::uniform_real_distribution<double> getRandRadius{ 0, delta };
	constexpr int samples{ 512 };

	const matrix<4, 4> eye_to_screen{ rContext.Viewport * rContext.Perspective };
	const matrix<4, 4> screen_to_eye{ (eye_to_screen).inverse() };
#pragma omp parallel for collapse(2)
	for (int x{ 0 }; x < width; ++x) {
		for (int y{ 0 }; y < height; ++y) {
			const double z = rContext.zbuffer[x + y * width];
			if (z < -1e5) continue; // open sky / background, no occlusion

			vec4 coords_eye_space{ screen_to_eye * vec4{static_cast<double>(x), static_cast<double>(y), z, 1.} };
			coords_eye_space /= coords_eye_space.w;

			// get normal vector at this point
			TGAColor normal_color{ normalbuffer.get(x, y) };
			vec4 normal_eye_space{
				normalize(vec4{
				2 * (normal_color[0] / 255. - 0.5) ,
				2 * (normal_color[1] / 255. - 0.5) ,
				2 * (normal_color[2] / 255. - 0.5) ,
				0. // directional vector
					})
			};

			// sampling
			int voters{}; // valid, in-bound samples
			int votes{}; // occluded samples
			for (int i{ 0 }; i < samples; ++i) {
				double phi{ getPhi(mt) };
				double sampleY{ getDist(mt) };

				// radius of 2D circle slice
				const double r{ std::sqrt(1. - sampleY * sampleY) };
				double sampleX{ std::cos(phi) * r };
				double sampleZ{ std::sin(phi) * r };

				// dir vector based on perfect unit sphere
				vec4 dir_vector{ sampleX, sampleY, sampleZ, 0. };
				// flip dir vector if not in same general dir as normal
				if (dot(normal_eye_space, dir_vector) < 0) {
					dir_vector *= -1;
				}

				// mult dir vector by rand radius
				const double rand_radius{ getRandRadius(mt) };
				dir_vector *= rand_radius;

				// add dir to fragment base pos
				vec4 sample_coord{ coords_eye_space.x + dir_vector.x, coords_eye_space.y + dir_vector.y, coords_eye_space.z + dir_vector.z, 1. };
				vec4 sample_coord_screen{ eye_to_screen * sample_coord };

				sample_coord_screen /= sample_coord_screen.w;
				int sx = static_cast<int>(sample_coord_screen.x);
				int sy = static_cast<int>(sample_coord_screen.y);
				// out of bounds check
				if (sx < 0 || sx >= width
					|| sy < 0 || sy >= height)
				{
					continue;
				}

				// the rendered on-screen zbuffer at the sample's x, y coords
				double scene_z{ rContext.zbuffer[sx + sy * width] };
				if (scene_z < -1e5) continue; // open sky / background, no occlusion


				// range check to prevent dark halo
				if (std::abs(z - scene_z) > 5. * delta) continue;

				// rand_coord_screen.z is a rand generated z val in the surrounding cube
				// if this rand z is 
				if (sample_coord_screen.z < scene_z) {
					++votes; // this sample is occluded
				}

				++voters;
			}

			// scale down occlusion contribution
			const double occlusion_factor{ (voters > 0) ? (1. * votes / voters) : 0. };
			const double ambient{ 1. - occlusion_factor }; // how lit this point is (1 = max bright)
			finalbuffer.set(x, y, TGAColor{
				static_cast<std::uint8_t>(255 * ambient),
				static_cast<std::uint8_t>(255 * ambient),
				static_cast<std::uint8_t>(255 * ambient),
				255
				});
		}
	}


	finalbuffer.write_tga_file("framebuffer.tga");
	return 0;
}
