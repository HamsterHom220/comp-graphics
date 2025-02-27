#pragma once

#include "rasterizer.h"
#include "renderer/rasterizer/rasterizer.h"
#include "renderer/renderer.h"
#include "resource.h"
#include "world/camera.h"
#include "world/model.h"

namespace cg::renderer
{
	class rasterization_renderer : public renderer
	{
	public:
		virtual void init();
		virtual void render();
		virtual void destroy();
		virtual void update();

	protected:
		std::shared_ptr<cg::resource<cg::unsigned_color>> render_target;
		std::shared_ptr<cg::resource<float>> depth_buffer;

		std::shared_ptr<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>> rasterizer;

	private:
		std::shared_ptr<cg::world::model> model;
		std::shared_ptr<cg::world::camera> camera;

		// Grayscale Conversion default parameters
		bool enable_grayscale_conversion = false;
		float grayscale_weight_r = 0.299f;
		float grayscale_weight_g = 0.587f;
		float grayscale_weight_b = 0.114f;

		// Grayscale Conversion
		cg::color convert_to_grayscale(const cg::color& pixel);
	};
}