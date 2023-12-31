#pragma once

#include "common/colors.h"
#include "utils/compiler.h"

GODOT_WARNING_DISABLE()
#include <godot_cpp/classes/ref_counted.hpp>
GODOT_WARNING_RESTORE()
using namespace godot;

/**
 * @brief
 * This is a class for storing part of the DebugDraw3D configuration.
 *
 * You can use DebugDraw3D.get_config to get the current instance of the configuration.
 */
class DebugDraw3DConfig : public RefCounted {
	GDCLASS(DebugDraw3DConfig, RefCounted)

private:
	int32_t geometry_render_layers = 1;
	bool freeze_3d_render = false;
	bool visible_instance_bounds = false;
	bool use_frustum_culling = false;
	real_t cull_by_distance = -1;
	bool force_use_camera_from_scene = false;
	Color line_hit_color = Colors::red;
	Color line_after_hit_color = Colors::green;

protected:
	/// @private
	static void _bind_methods();

public:
	/**
	 * Set whether debug 3D graphics rendering is frozen.
	 * This means that previously created geometry will not be updated until set to false or until DebugDraw3D.clear_all is called.
	 */
	void set_freeze_3d_render(const bool &_state);
	bool is_freeze_3d_render() const;

	/**
	 * Set whether the boundaries of instances are displayed.
	 * Based on these boundaries, instances are culled if set_use_frustum_culling is activated.
	 */
	void set_visible_instance_bounds(const bool &_state);
	bool is_visible_instance_bounds() const;

	/**
	 * Set whether frustum culling is used.
	 * @warning
	 * At this point, activating this parameter is likely to decrease performance.
	 */
	void set_use_frustum_culling(const bool &_state);
	bool is_use_frustum_culling() const;

	/**
	 * Set the culling distance after which instances will stop being displayed to improve performance.
	 *
	 * If set to 0, this parameter will be ignored and instances will be drawn at any distance.
	 */
	void set_culling_distance(const real_t &_distance);
	real_t get_culling_distance() const;

	/**
	 * Set to force the use of the scene's default camera.
	 */
	void set_force_use_camera_from_scene(const bool &_state);
	bool is_force_use_camera_from_scene() const;

	/**
	 * Set the visibility layer on which the 3D geometry will be drawn.
	 * Similar to using VisualInstance3D.layers.
	 */
	void set_geometry_render_layers(const int32_t &_layers);
	int32_t get_geometry_render_layers() const;

	/**
	 * Set the default color for the collision point of DebugDraw3D.draw_line_hit.
	 */
	void set_line_hit_color(const Color &_new_color);
	Color get_line_hit_color() const;

	/**
	 * Set the default color for the line after the collision point of DebugDraw3D.draw_line_hit.
	 */
	void set_line_after_hit_color(const Color &_new_color);
	Color get_line_after_hit_color() const;
};
