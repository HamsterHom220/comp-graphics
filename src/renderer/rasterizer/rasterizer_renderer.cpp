#include "rasterizer_renderer.h"
#include "utils/resource_utils.h"

using namespace cg::renderer;

void rasterization_renderer::init()
{
	rasterizer = std::make_shared<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>>();
	depth_buffer = std::make_shared<cg::resource<float>>(settings->width, settings->height);
	rasterizer->set_viewport(settings->width, settings->height);
	render_target = std::make_shared<resource<cg::unsigned_color>>(settings->width, settings->height);
	rasterizer->set_render_target(render_target, depth_buffer);
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);

	camera = std::make_shared<cg::world::camera>();
	camera->set_height(static_cast<float>(settings->height));
	camera->set_width(static_cast<float>(settings->width));
	camera->set_position(float3{
			settings->camera_position[0],
			settings->camera_position[1],
			settings->camera_position[2]
	});
	camera->set_phi(settings->camera_phi);
	camera->set_theta(settings->camera_theta);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_near(settings->camera_z_near);
	camera->set_z_far(settings->camera_z_far);

	// Grayscale Conversion settings
	enable_grayscale_conversion = true;
	grayscale_weight_r = 0.299f;
	grayscale_weight_g = 0.587f;
	grayscale_weight_b = 0.114f;
}

void rasterization_renderer::render()
{
	auto start = std::chrono::high_resolution_clock::now();
	rasterizer->clear_render_target({ 0, 0, 0 });

	float4x4 matrix = mul(
			camera->get_projection_matrix(),
			camera->get_view_matrix(),
			model->get_world_matrix()
	);

	rasterizer->vertex_shader = [&](float4 vertex, cg::vertex vertex_data) {
		auto processed = mul(matrix, vertex);
		return std::make_pair(processed, vertex_data);
	};

	rasterizer->pixel_shader = [&](const cg::vertex vertex_data, float z) {
		cg::color color = {
				vertex_data.ambient_r,
				vertex_data.ambient_g,
				vertex_data.ambient_b
		};

		if (enable_grayscale_conversion) {
			color = convert_to_grayscale(color);
		}

		return color;
	};

	for (size_t shape_id = 0; shape_id < model->get_index_buffers().size(); shape_id++) {
		rasterizer->set_vertex_buffer(model->get_vertex_buffers()[shape_id]);
		rasterizer->set_index_buffer(model->get_index_buffers()[shape_id]);
		rasterizer->draw(model->get_index_buffers()[shape_id]->get_number_of_elements(), 0);
	}

	auto stop = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = stop - start;
	std::cout << "Rasterization took: " << duration.count() << " ms\n";

	cg::utils::save_resource(*render_target, settings->result_path);
}

void rasterization_renderer::destroy() {}

void rasterization_renderer::update() {}

cg::color rasterization_renderer::convert_to_grayscale(const cg::color& pixel)
{
	float grayscale_value = (
			grayscale_weight_r * pixel.r +
			grayscale_weight_g * pixel.g +
			grayscale_weight_b * pixel.b
	);

	return { grayscale_value, grayscale_value, grayscale_value };
}