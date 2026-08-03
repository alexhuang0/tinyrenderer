#include "model.h"
#include <fstream>
#include <sstream>

vec3 Model::vert(const std::size_t i) const {
	// 0 <= i < nverts()
	return verts[i];
}
vec3 Model::vert(const std::size_t iface, const std::size_t nthvert) const {
	// 0 <= iface < nfaces(), 0 <= nthvert < 3
	return verts[facet_vrt[iface * 3 + nthvert]];
}

vec3 Model::normal(const std::size_t iface, const std::size_t nthnorm) const {
	return norms[facet_nrm[iface * 3 + nthnorm]];
}

Model::Model(const std::string& filename) {
	std::ifstream objFile(filename);

	if (objFile.fail()) {
		// This will print the exact reason (e.g., "No such file or directory")
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
		else if (curLine.compare(0, 2, "f ") == 0) {
			// f 1193/1240/1193 1180/1227/1180 1179/1226/1179
			std::string vCluster{}; // "v/vt/vn"

			int i1{};
			int i2{};
			int i3{};
			int tmp{};
			if (std::sscanf(curLine.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d"
				, &i1, &tmp, &tmp, &i2, &tmp, &tmp, &i3, &tmp, &tmp) == 9) {
				Model::facet_vrt.push_back(static_cast<size_t>(i1) - 1);
				Model::facet_vrt.push_back(static_cast<size_t>(i2) - 1);
				Model::facet_vrt.push_back(static_cast<size_t>(i3) - 1);
			}
		}
	}

	std::cout << "read " << nverts() << " vertices and " << nfaces() << " faces\n";
	objFile.close();
}
