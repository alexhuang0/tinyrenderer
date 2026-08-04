#include "model.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include "canvas.h"

namespace fs = std::filesystem;

vec4 Model::vert(const std::size_t i) const {
	// 0 <= i < nverts()
	return verts[i];
}
vec4 Model::vert(const std::size_t iface, const std::size_t nthvert) const {
	// 0 <= iface < nfaces(), 0 <= nthvert < 3
	return verts[facet_vrt[iface * 3 + nthvert]];
}

vec4 Model::normal(const std::size_t iface, const std::size_t nthnorm) const {
	return norms[facet_nrm[iface * 3 + nthnorm]];
}

vec2 Model::uv_coords(const std::size_t iface, const std::size_t nthvert) const {
	return tex[facet_tex[iface * 3 + nthvert]];
}
vec4 Model::normal_tex(const vec2& uv) const {
	TGAColor c{ normalmap.get(uv.x * normalmap.width(), uv.y * normalmap.height()) };
	// map [0, 255] -> [-1, 1]
	return (vec4{
			static_cast<double>(c[2]),
			static_cast<double>(c[1]),
			static_cast<double>(c[0]),
			0.,
		}) * 2. / 255. - vec4{ 1, 1, 1, 0 };
	;
}

TGAColor Model::diff(const vec2& uv) const {
	return diffmap.get(uv.x * diffmap.width(), uv.y * diffmap.height());
}
double Model::spec(const vec2& uv) const {
	return specmap.get(uv.x * specmap.width(), uv.y * specmap.height())[0];
}

Model::Model(const std::string& modelName) {
	fs::path dir = fs::path("./obj") / modelName;

	fs::path obj_path = dir / (modelName + ".obj");
	std::ifstream objFile(obj_path);
	if (objFile.fail()) {
		// print the exact reason (eg "No such file or directory")
		std::perror("File open failed");
	}

	std::string curLine{};
	std::string lineStart{};

	int i{ 0 }; // debugging
	while (std::getline(objFile, curLine)) {
		if (curLine.empty()) {
			continue;
		}

		if (curLine.compare(0, 2, "v ") == 0) {
			// v 0.608654 -0.568839 -0.416318
			double x{};
			double y{};
			double z{};

			if (std::sscanf(curLine.c_str(), "v %lf %lf %lf", &x, &y, &z) == 3) {
				Model::verts.push_back({ x, y, z });
			}
		}
		else if (curLine.compare(0, 2, "vn") == 0) {
			double x{}, y{}, z{};

			if (std::sscanf(curLine.c_str(), "vn %lf %lf %lf", &x, &y, &z) == 3) {
				Model::norms.push_back({ x, y, z });
			}
		}
		else if (curLine.compare(0, 2, "vt") == 0) {
			double x{}, y{}, tmp{};

			if (std::sscanf(curLine.c_str(), "vt %lf %lf %lf", &x, &y, &tmp) == 3) {
				Model::tex.push_back({ x, 1 - y });
			}
		}
		else if (curLine.compare(0, 2, "f ") == 0) {
			// f 1193/1240/1193 1180/1227/1180 1179/1226/1179
			std::string vCluster{}; // "v/vt/vn"

			int v1{}, v2{}, v3{};
			int vn1{}, vn2{}, vn3{};
			int vt1{}, vt2{}, vt3{};
			if (std::sscanf(curLine.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d"
				, &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3) == 9) {
				Model::facet_vrt.push_back(static_cast<size_t>(v1) - 1);
				Model::facet_vrt.push_back(static_cast<size_t>(v2) - 1);
				Model::facet_vrt.push_back(static_cast<size_t>(v3) - 1);

				Model::facet_nrm.push_back(static_cast<size_t>(vn1) - 1);
				Model::facet_nrm.push_back(static_cast<size_t>(vn2) - 1);
				Model::facet_nrm.push_back(static_cast<size_t>(vn3) - 1);

				Model::facet_tex.push_back(static_cast<size_t>(vt1) - 1);
				Model::facet_tex.push_back(static_cast<size_t>(vt2) - 1);
				Model::facet_tex.push_back(static_cast<size_t>(vt3) - 1);
			}
		}
	}

	// TGA texture loading
	auto load_texture = [&modelName, &dir](const std::string& type, TGAImage& img) {
		fs::path tex_path = dir / (modelName + "_" + type + ".tga");
		if (!img.read_tga_file(tex_path.string())) {
			std::cerr << "Error reading " << type << "texture";
		}
		};

	load_texture("nm", normalmap);
	load_texture("diffuse", diffmap);
	load_texture("spec", specmap);

	std::cout << "read " << nverts() << " vertices, "
		<< nnorms() << " vertex normals, "
		<< ntex() << " texture coords, and "
		<< nfaces() << " faces\n";

	objFile.close();
}
