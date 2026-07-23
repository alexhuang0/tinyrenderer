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

#include "canvas.h"
#include "model.h"
#include "renderer.h"

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255, 0, 255 };
constexpr TGAColor red = { 0, 0, 255, 255 };
constexpr TGAColor blue = { 255, 128, 64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

template <typename T, int N>
std::array<T, N> insertionSort(std::array<T, N> arr) {

	T tmp{};
	for (std::size_t i{ 1 }; i < N; ++i) {
		auto curIdx{ i };
		tmp = arr[i];
		while (curIdx > 0 && arr[curIdx - 1] > tmp) {
			arr[curIdx] = arr[curIdx - 1];
			--curIdx;
		}
		arr[curIdx] = tmp;
	}

	return arr;
}

int main(int argc, char** argv) {
	//Model model(R"(C:\Users\Alex\Documents\Alex Stuff\Programming\c++\tinyrenderer\obj\diablo3_pose\diablo3_pose.obj)");
	std::array arr{ 5, 2, 4, 6, 1 };
	arr = insertionSort(arr);
	for (int element : arr) {
		std::cout << element << " ";
	}


	TGAImage framebuffer(Canvas::width, Canvas::height, TGAImage::RGB);

	Renderer::triangle(7, 45, 35, 100, 45, 60, framebuffer, red);
	Renderer::triangle(120, 35, 90, 5, 45, 110, framebuffer, white);
	Renderer::triangle(115, 83, 80, 90, 85, 120, framebuffer, green);

	framebuffer.write_tga_file("framebuffer.tga");
	return 0;
}
