#include "data_graphs.h"
#include "debug_draw.h"
#include "math_utils.h"
#include "utils.h"

#include <limits.h>

using namespace godot;

void GraphParameters::_bind_methods() {
#define REG_CLASS_NAME GraphParameters

	REG_PROP_BOOL(enabled);
	REG_PROP_BOOL(show_title);
	REG_PROP_BOOL(frame_time_mode);

	REG_PROP(show_text_flags, Variant::INT);
	REG_PROP(size, Variant::VECTOR2I);
	REG_PROP(buffer_size, Variant::INT);
	REG_PROP(offset, Variant::VECTOR2I);
	REG_PROP(position, Variant::INT);
	REG_PROP(line_width, Variant::FLOAT);
	REG_PROP(line_color, Variant::COLOR);
	REG_PROP(line_position, Variant::INT);
	REG_PROP(background_color, Variant::COLOR);
	REG_PROP(border_color, Variant::COLOR);
	REG_PROP(text_suffix, Variant::STRING);

	REG_PROP(custom_font, Variant::OBJECT);
	REG_PROP(title_size, Variant::FLOAT);
	REG_PROP(text_size, Variant::INT);
	REG_PROP(title_color, Variant::COLOR);
	REG_PROP(text_color, Variant::COLOR);

	BIND_ENUM_CONSTANT(LINE_TOP);
	BIND_ENUM_CONSTANT(LINE_CENTER);
	BIND_ENUM_CONSTANT(LINE_BOTTOM);

	BIND_ENUM_CONSTANT(POSITION_LEFT_TOP);
	BIND_ENUM_CONSTANT(POSITION_RIGHT_TOP);
	BIND_ENUM_CONSTANT(POSITION_LEFT_BOTTOM);
	BIND_ENUM_CONSTANT(POSITION_RIGHT_BOTTOM);

	BIND_BITFIELD_FLAG(TEXT_CURRENT);
	BIND_BITFIELD_FLAG(TEXT_AVG);
	BIND_BITFIELD_FLAG(TEXT_MAX);
	BIND_BITFIELD_FLAG(TEXT_MIN);
	BIND_BITFIELD_FLAG(TEXT_ALL);
}

GraphParameters::GraphParameters() {
	if (IS_EDITOR_HINT()) {
		position = GraphPosition::POSITION_LEFT_TOP;
	}
}

void GraphParameters::set_enabled(bool _state) {
	enabled = _state;
}

bool GraphParameters::is_enabled() const {
	return enabled;
}

void GraphParameters::set_show_title(bool _state) {
	show_title = _state;
}

bool GraphParameters::is_show_title() const {
	return show_title;
}

void GraphParameters::set_frame_time_mode(bool _state) {
	frametime_mode = _state;
}

bool GraphParameters::is_frame_time_mode() const {
	return frametime_mode;
}

void GraphParameters::set_line_position(GraphLinePosition _position) {
	line_position = _position;
}

GraphParameters::GraphLinePosition GraphParameters::get_line_position() const {
	return line_position;
}

void GraphParameters::set_show_text_flags(BitField<TextFlags> _flags) {
	show_text_flags = _flags;
}

BitField<GraphParameters::TextFlags> GraphParameters::get_show_text_flags() const {
	return show_text_flags;
}

void GraphParameters::set_size(Vector2i _size) {
	size = _size;
	size.x = Math::clamp(size.x, 1, INT32_MAX);
	size.y = Math::clamp(size.y, 1, INT32_MAX);
}

Vector2i GraphParameters::get_size() const {
	return size;
}

void GraphParameters::set_buffer_size(int _buf_size) {
	buffer_size = Math::clamp(_buf_size, 3, INT32_MAX);
}

int GraphParameters::get_buffer_size() const {
	return buffer_size;
}

void GraphParameters::set_offset(Vector2i _offset) {
	offset = _offset;
}

Vector2i GraphParameters::get_offset() const {
	return offset;
}

void GraphParameters::set_position(GraphPosition _position) {
	position = (GraphPosition)_position;
}

GraphParameters::GraphPosition GraphParameters::get_position() const {
	return position;
}

void GraphParameters::set_line_width(real_t _width) {
	line_width = Math::clamp(_width, 1.0f, 32.0f);
}

real_t GraphParameters::get_line_width() const {
	return line_width;
}

void GraphParameters::set_line_color(Color _new_color) {
	line_color = _new_color;
}

Color GraphParameters::get_line_color() const {
	return line_color;
}

void GraphParameters::set_background_color(Color _new_color) {
	background_color = _new_color;
}

Color GraphParameters::get_background_color() const {
	return background_color;
}

void GraphParameters::set_border_color(Color _new_color) {
	border_color = _new_color;
}

Color GraphParameters::get_border_color() const {
	return border_color;
}

void GraphParameters::set_text_suffix(String _suffix) {
	text_suffix = _suffix;
}

String GraphParameters::get_text_suffix() const {
	return text_suffix;
}

void GraphParameters::set_custom_font(Ref<Font> _custom_font) {
	custom_font = _custom_font;
}

Ref<Font> GraphParameters::get_custom_font() const {
	return custom_font;
}

void GraphParameters::set_title_size(int _size) {
	title_size = Math::clamp(_size, 1, INT_MAX);
}

int GraphParameters::get_title_size() const {
	return title_size;
}

void GraphParameters::set_text_size(int _size) {
	text_size = Math::clamp(_size, 1, INT_MAX);
}

int GraphParameters::get_text_size() const {
	return text_size;
}

void GraphParameters::set_title_color(Color _new_color) {
	title_color = _new_color;
}

Color GraphParameters::get_title_color() const {
	return title_color;
}

void GraphParameters::set_text_color(Color _new_color) {
	text_color = _new_color;
}

Color GraphParameters::get_text_color() const {
	return text_color;
}

////////////////////////////////////
// DataGraph

DataGraph::DataGraph(Ref<GraphParameters> _owner) :
		config(_owner),
		data(std::make_shared<CircularBuffer<double> >(_owner->get_buffer_size())),
		type(Type::Custom) {
}

DataGraph::Type DataGraph::get_type() const {
	return type;
}

Ref<GraphParameters> DataGraph::get_config() const {
	return config;
}

void DataGraph::update(double value) {
	LOCK_GUARD(datalock);

	if (config->get_buffer_size() != data->buffer_size())
		data = std::make_shared<CircularBuffer<double> >(config->get_buffer_size());

	_update_added(value);
}

void DataGraph::_update_added(double value) {
	data->add(value);
}

// TODO sometimes the graphs are drawn in the wrong places
Vector2 DataGraph::draw(CanvasItem *ci, const Ref<Font> &font, const Vector2 &vp_size, const String &title, const Vector2 &base_offset) const {
	if (!config->is_enabled())
		return base_offset;

	LOCK_GUARD(datalock);

	Ref<Font> draw_font = config->get_custom_font().is_null() ? font : config->get_custom_font();

	double min, max, avg;
	data->get_min_max_avg(&min, &max, &avg);

	// Truncate for pixel perfect render
	Vector2 graph_size = config->get_size();
	Vector2 graph_offset = config->get_offset();
	Vector2 pos = graph_offset;
	Vector2 title_size = draw_font->get_string_size(title, HORIZONTAL_ALIGNMENT_LEFT, -1.0, config->get_title_size());

	switch (config->get_position()) {
		case GraphParameters::GraphPosition::POSITION_LEFT_TOP:
			pos.y += base_offset.y;
			pos.x += base_offset.x;
			break;
		case GraphParameters::GraphPosition::POSITION_RIGHT_TOP:
			pos = Vector2(vp_size.x - graph_size.x - graph_offset.x + 1, graph_offset.y + base_offset.y);
			pos.x -= base_offset.x;
			break;
		case GraphParameters::GraphPosition::POSITION_LEFT_BOTTOM:
			pos = Vector2(graph_offset.x, base_offset.y - graph_size.y - graph_offset.y - (config->is_show_title() ? title_size.y - 3 : 0));
			pos.x += base_offset.x;
			break;
		case GraphParameters::GraphPosition::POSITION_RIGHT_BOTTOM:
			pos = Vector2(vp_size.x - graph_size.x - graph_offset.x + 1, base_offset.y - graph_size.y - graph_offset.y - (config->is_show_title() ? title_size.y - 3 : 0));
			pos.x -= base_offset.x;
			break;
	}

	// Draw title
	if (config->is_show_title()) {
		Vector2 title_pos = pos;

		switch (config->get_position()) {
			case GraphParameters::GraphPosition::POSITION_RIGHT_TOP:
			case GraphParameters::GraphPosition::POSITION_RIGHT_BOTTOM:
				title_pos.x = title_pos.x + graph_size.x - title_size.x;
				break;
		}

		// 4 for horizontal padding
		Rect2 border_rect(title_pos, title_size + Vector2(4, 0));
		border_rect.size.height -= 1;
		// Draw background
		ci->draw_rect(border_rect, config->get_background_color(), true);
		ci->draw_string(
				draw_font,
				(title_pos + Vector2(2, (real_t)draw_font->get_ascent(config->get_title_size()))).floor(),
				title,
				godot::HORIZONTAL_ALIGNMENT_LEFT, -1.0, config->get_title_size(), config->get_title_color());

		pos += Vector2(0, title_size.y);
	}

	double height_multiplier = graph_size.y / max;
	double center_offset = 0;
	switch (config->get_line_position()) {
		case GraphParameters::LINE_TOP: {
			center_offset = 0;
			break;
		}
		case GraphParameters::LINE_CENTER: {
			center_offset = (graph_size.y - height_multiplier * (max - min)) * 0.5;
			break;
		}
		case GraphParameters::LINE_BOTTOM: {
			center_offset = graph_size.y - height_multiplier * (max - min) - 1;
			break;
		}
	}

	Rect2 border_size(pos + Vector2_UP, graph_size + Vector2_DOWN);

	// Draw background
	ci->draw_rect(border_size, config->get_background_color(), true);

	// Draw graph line
	if (data->is_filled() || data->size() > 2) {
		PackedVector2Array line_points;

		const int offset = (int)data->is_filled();
		double points_interval = (double)config->get_size().x / ((int64_t)config->get_buffer_size() - 1 - offset);

		line_points.resize((data->size() - offset) * 2);
		{
			auto w = line_points.ptrw();
			for (size_t i = 1; i < data->size() - offset; i++) {
				w[(int)i * 2] = pos + Vector2((real_t)(i * points_interval), graph_size.y - (real_t)(data->get(i) * height_multiplier) + (real_t)center_offset);
				w[(int)i * 2 + 1] = pos + Vector2((real_t)((i - 1) * points_interval), graph_size.y - (real_t)(data->get(i - 1) * height_multiplier) + (real_t)center_offset);
			}
		}
		// ci->draw_polyline(line_points, config->get_line_color(), 1, true);
		ci->draw_multiline(line_points, config->get_line_color(), config->get_line_width());
	}

	// Draw border
	ci->draw_rect(border_size, config->get_border_color(), false);

	auto format_arg = [](const char *format, auto arg) -> String {
		int size_s = std::snprintf(nullptr, 0, format, arg) + 1; // Extra space for '\0'
		if (size_s <= 0) {
			PRINT_ERROR("Error during formatting.");
			return "{FORMAT FAILED}";
		}
		std::unique_ptr<char[]> buf(new char[(size_t)size_s]);
		buf[size_s - 1] = '\0';
		std::snprintf(buf.get(), (size_t)size_s, format, arg);
		return String(buf.get());
	};

	// Draw text
	if (config->get_show_text_flags() & GraphParameters::TextFlags::TEXT_MAX) {
		String text = String("max: {0} {1}").format(Array::make(format_arg("%.2f", max), config->get_text_suffix()));
		real_t height = (real_t)draw_font->get_ascent(config->get_text_size());
		Vector2 text_pos = pos + Vector2(4, height - 1);
		ci->draw_string(draw_font, text_pos.floor(), text, godot::HORIZONTAL_ALIGNMENT_LEFT, -1, config->get_text_size(), config->get_text_color());
	}

	if (config->get_show_text_flags() & GraphParameters::TextFlags::TEXT_AVG) {
		String text = String("avg: {0} {1}").format(Array::make(format_arg("%.2f", avg), config->get_text_suffix()));
		real_t height = (real_t)config->get_text_size();
		Vector2 text_pos = pos + Vector2(4, (graph_size.y * 0.5f + height * 0.5f - 2));
		ci->draw_string(draw_font, text_pos.floor(), text, godot::HORIZONTAL_ALIGNMENT_LEFT, -1, config->get_text_size(), config->get_text_color());
	}

	if (config->get_show_text_flags() & GraphParameters::TextFlags::TEXT_MIN) {
		String text = String("min: {0} {1}").format(Array::make(format_arg("%.2f", min), config->get_text_suffix()));
		Vector2 text_pos = pos + Vector2(4, graph_size.y - 3);
		ci->draw_string(draw_font, text_pos.floor(), text, godot::HORIZONTAL_ALIGNMENT_LEFT, -1, config->get_text_size(), config->get_text_color());
	}

	if (config->get_show_text_flags() & GraphParameters::TextFlags::TEXT_CURRENT) {
		// `space` at the end of line for offset from border
		String text = String("{0} {1}").format(Array::make(format_arg("%.2f", (data->size() > 1 ? data->get(data->size() - 2) : 0)), config->get_text_suffix()));
		Vector2 cur_size = draw_font->get_string_size(text, HORIZONTAL_ALIGNMENT_LEFT, -1.0, config->get_text_size());
		Vector2 text_pos = pos + Vector2(graph_size.x - cur_size.x, graph_size.y * 0.5f + config->get_text_size() * 0.5f - 2);
		ci->draw_string(draw_font, text_pos.floor(), text, godot::HORIZONTAL_ALIGNMENT_LEFT, -1, config->get_text_size(), config->get_text_color());
	}

	switch (config->get_position()) {
		case GraphParameters::GraphPosition::POSITION_LEFT_TOP:
		case GraphParameters::GraphPosition::POSITION_RIGHT_TOP:
			return Vector2(base_offset.x, border_size.position.y + border_size.size.y + 0);
		case GraphParameters::GraphPosition::POSITION_LEFT_BOTTOM:
		case GraphParameters::GraphPosition::POSITION_RIGHT_BOTTOM:
			return Vector2(base_offset.x, border_size.position.y - (config->is_show_title() ? title_size.y : -1));
	}

	return base_offset;
}

////////////////////////////////////
// FPSGraph

void FPSGraph::_update_added(double value) {
	if (is_ms != config->is_frame_time_mode()) {
		data->reset();
		is_ms = config->is_frame_time_mode();
		config->set_text_suffix(is_ms ? "ms" : "fps");
	}

	data->add(is_ms ? value * 1000.f : 1.f / value);
}

////////////////////////////////////
// DataGraphManager

DataGraphManager::DataGraphManager(DebugDraw *root) {
	owner = root;
}

DataGraphManager::~DataGraphManager() {
}

Ref<GraphParameters> DataGraphManager::create_graph(const StringName &title) {
	Ref<GraphParameters> config;
	config.instantiate();

	LOCK_GUARD(datalock);
	graphs[title] = std::make_unique<DataGraph>(config);
	return config;
}

Ref<GraphParameters> DataGraphManager::create_fps_graph(const StringName &title) {
	Ref<GraphParameters> config;
	config.instantiate();

	LOCK_GUARD(datalock);
	graphs[title] = std::make_unique<FPSGraph>(config);
	return config;
}

void DataGraphManager::_update_fps(double delta) {
	for (auto &i : graphs) {
		if (i.second->get_type() == DataGraph::Type::FPS) {
			i.second->update(delta);
			owner->mark_canvas_needs_update();
		}
	}
}

void DataGraphManager::draw(CanvasItem *ci, Ref<Font> font, Vector2 vp_size) const {
	Vector2 base_offset = owner->get_graphs_base_offset();
	Vector2 prev_offset[GraphParameters::POSITION_MAX] = { base_offset, Vector2(base_offset.x, base_offset.y), Vector2(base_offset.x, vp_size.y - base_offset.y), Vector2(base_offset.x, vp_size.y - base_offset.y) };
	for (auto &i : graphs) {
		int pos = i.second->get_config()->get_position();
		prev_offset[pos] = i.second->draw(ci, font, vp_size, i.first, prev_offset[pos]);
	}
}

void DataGraphManager::graph_update_data(const StringName &title, double data) {
	if (graphs.count(title)) {
		if (graphs[title]->get_type() != DataGraph::Type::FPS) {
			graphs[title]->update(data);
			owner->mark_canvas_needs_update();
		} else {
			PRINT_WARNING("Trying to manually update the FPS graph");
		}
	} else {
		PRINT_ERROR("Graph with name '" + title + "' not found");
	}
}

void DataGraphManager::remove_graph(const StringName &title) {
	LOCK_GUARD(datalock);
	if (graphs.count(title)) {
		graphs.erase(title);
	}
}

void DataGraphManager::clear_graphs() {
	LOCK_GUARD(datalock);
	graphs.clear();
}

Ref<GraphParameters> DataGraphManager::get_graph_config(const StringName &title) const {
	if (graphs.count(title)) {
		return graphs.find(title)->second->get_config();
	}
	return Ref<GraphParameters>();
}

PackedStringArray DataGraphManager::get_graph_names() const {
	LOCK_GUARD(datalock);

	PackedStringArray res;
	for (auto &i : graphs) {
		res.append(i.first);
	}
	return res;
}