#include "our_gl.h"
#include "canvas.h"

void RenderContext::init_zbuffer(int width, int height) {
	zbuffer = std::vector<double>(width * height, -std::numeric_limits<double>::max());
}

// homogeneous coordinates stuff
void RenderContext::init_viewport(int x, int y, int w, int h) {
	Viewport = {
		{
			vec4{w / 2., 0, 0, x + w / 2.},
			{0, h / 2., 0, y + h / 2.},
			{0,0,1,0},
			{0,0,0,1}
		}
	};
}

void RenderContext::init_perspective(double f) {
	Perspective = {
		{
		vec4{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, -1 / f, 1}
		}
	};
}

void RenderContext::lookat(const vec<3>& eye, const vec<3>& center, const vec<3>& up) {
	vec3 n{ normalize(eye - center) };
	vec3 l{ normalize(cross3d(up, n)) };
	vec3 m{ normalize(cross3d(n, l)) };

	matrix<4, 4> translateCenter{
		vec4{1, 0, 0, -center.x},
		{0, 1, 0, -center.y},
		{0, 0, 1, -center.z},
		{0, 0, 0, 1}
	};

	matrix<4, 4> basis_change_matrix_inverted{
		vec4{l.x, l.y, l.z, 0},
		{m.x, m.y, m.z, 0},
		{n.x, n.y, n.z, 0},
		{0, 0, 0, 1}
	};

	ModelView = basis_change_matrix_inverted * translateCenter;
}

void RenderContext::composeTransforms() {
	ComposeTransforms = Perspective * ModelView;
}


// clips is triangle vertices
void RenderContext::rasterize(const Triangle& clip, const IShader& shader, TGAImage& framebuffer) {
	// normalized device coords ([-1, 1] x [-1, 1] bounding box)
	// divide each vertex from clip array by its w (depth, for orthogonal projection)
	vec4 ndc[3]{ clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w };
	// screen coords
	vec2 screen[3]{ (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() };

	matrix<3, 3> ABC{
		vec3{screen[0].x, screen[0].y, 1.},
		{screen[1].x, screen[1].y, 1.},
		{screen[2].x, screen[2].y, 1.}
	};
	if (ABC.determinant() < 1) return; // backface culling + discarding triangles that cover less than a 1/2 pixel

	double bbminX{ std::min(screen[0].x, std::min(screen[1].x, screen[2].x)) };
	double bbmaxX{ std::max(screen[0].x, std::max(screen[1].x, screen[2].x)) };
	double bbminY{ std::min(screen[0].y, std::min(screen[1].y, screen[2].y)) };
	double bbmaxY{ std::max(screen[0].y, std::max(screen[1].y, screen[2].y)) };

	matrix<3, 3> ABC_inv_transp{ ABC.inverse().transpose() };

	// OpenMP requires the canonical loop form (init with '=', not brace-init),
	// and the bounds must be loop-invariant, so hoist them out.
	const int xlo = std::max<int>(bbminX, 0);
	const int xhi = std::min<int>(bbmaxX, framebuffer.width() - 1);
	const int ylo = std::max<int>(bbminY, 0);
	const int yhi = std::min<int>(bbmaxY, framebuffer.height() - 1);

#pragma omp parallel for
	for (int x = xlo; x <= xhi; ++x) {
		for (int y = ylo; y <= yhi; ++y) {
			vec3 bary_coords{ ABC_inv_transp * vec3 { static_cast<double>(x), static_cast<double>(y), 1. } };

			// neg bary coords => pixel outside of triangle
			if (bary_coords.x < 0 || bary_coords.y < 0 || bary_coords.z < 0) continue;

			double z{ dot(bary_coords, vec3{ndc[0].z, ndc[1].z, ndc[2].z}) }; // interpolation for depth
			if (z <= zbuffer[static_cast<size_t>(y * framebuffer.width() + x)]) continue;

			auto [discard, color] = shader.fragment(bary_coords);
			if (discard) continue; // fragment shader can discard curr fragment

			zbuffer[static_cast<size_t>(y * framebuffer.width() + x)] = z;
			framebuffer.set(x, y, color);
		}
	}
}