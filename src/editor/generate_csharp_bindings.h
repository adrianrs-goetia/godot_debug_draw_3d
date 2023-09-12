#pragma once
#include "utils/compiler.h"

#include <map>

GODOT_WARNING_DISABLE()
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/builtin_types.hpp>
#include <godot_cpp/variant/typed_array.hpp>
GODOT_WARNING_RESTORE()

using namespace godot;

class GenerateCSharpBindingsPlugin {
	class IndentGuard {
		GenerateCSharpBindingsPlugin *owner;

	public:
		IndentGuard(GenerateCSharpBindingsPlugin *_owner);
		~IndentGuard();
	};

	struct PropertyMethods {
		String type_name;
		String set;
		String get;
	};

	struct ArgumentData {
		String name;
		String type_name;
		Variant::Type type;
		bool is_void;
		bool is_enum;

		ArgumentData(const String &_name, Variant::Type _type, const String &_type_name, bool _enum = false) {
			name = _name;
			type_name = _type_name;
			type = _type;
			is_void = _type_name == "void";
			is_enum = _enum;
		}
	};

	struct DefaultData {
		String name;
		String arg_string;
		bool is_need_remap;
		bool is_nullable;
		DefaultData(const String &_name, bool need_remap, const String &_arg_str, bool nullable = false) {
			name = _name;
			is_need_remap = need_remap;
			arg_string = _arg_str;
			is_nullable = nullable;
		}
	};

	String indent;
	Ref<FileAccess> opened_file;
	Ref<FileAccess> opened_log_file;
	TypedArray<StringName> generate_for_classes;

	std::map<Variant::Type, String> types_map = {
		{ Variant::NIL, "NILL" },
		{ Variant::BOOL, "bool" },
		{ Variant::INT, "int" },
		{ Variant::FLOAT, "float" },
		{ Variant::VECTOR2, "Vector2" },
		{ Variant::RECT2, "Rect2" },
		{ Variant::VECTOR3, "Vector3" },
		{ Variant::TRANSFORM2D, "Transform2D" },
		{ Variant::VECTOR4, "Vector4" },
		{ Variant::PLANE, "Plane" },
		{ Variant::QUATERNION, "Quaternion" },
		{ Variant::AABB, "Aabb" },
		{ Variant::BASIS, "Basis" },
		{ Variant::COLOR, "Color" },
		{ Variant::PROJECTION, "Projection" },
		{ Variant::TRANSFORM3D, "Transform3D" },
		{ Variant::STRING_NAME, "StringName" },
		{ Variant::NODE_PATH, "NodePath" },
		{ Variant::RID, "Rid" },
		{ Variant::CALLABLE, "Callable" },
		{ Variant::SIGNAL, "Signal" },

		// not matched
		{ Variant::STRING, "string" },
		{ Variant::VECTOR2I, "Vector2I" },
		{ Variant::RECT2I, "Rect2I" },
		{ Variant::VECTOR3I, "Vector3I" },
		{ Variant::VECTOR4I, "Vector4I" },
		{ Variant::OBJECT, "GodotObject" },
		{ Variant::DICTIONARY, "Godot.Collections.Dictionary" },
		{ Variant::ARRAY, "Godot.Collections.Array" },
		{ Variant::PACKED_BYTE_ARRAY, "byte[]" },
		{ Variant::PACKED_INT32_ARRAY, "int[]" },
		{ Variant::PACKED_INT64_ARRAY, "long[]" },
		{ Variant::PACKED_FLOAT32_ARRAY, "float[]" },
		{ Variant::PACKED_FLOAT64_ARRAY, "double[]" },
		{ Variant::PACKED_STRING_ARRAY, "string[]" },
		{ Variant::PACKED_VECTOR2_ARRAY, "Vector2[]" },
		{ Variant::PACKED_VECTOR3_ARRAY, "Vector3[]" },
		{ Variant::PACKED_COLOR_ARRAY, "Color[]" },
	};

public:
	void generate();

private:
	void generate_class(const StringName &cls);

	void generate_wrapper(const StringName &cls, bool is_static, bool inheritance = false);
	void generate_constants(const StringName &cls);
	void generate_enum(const StringName &cls, const StringName &enm);
	void generate_method(const StringName &cls, const Dictionary &method, bool is_static, bool is_property);
	void generate_properties(const StringName &cls, const TypedArray<Dictionary> &props, std::map<String, PropertyMethods> setget_map, bool is_static);
	ArgumentData argument_parse(const Dictionary &arg, bool is_return = false);
	ArgumentData argument_parse(const StringName &class_name, const String& name, const Variant::Type type);
	std::vector<DefaultData> arguments_parse_values(const TypedArray<Dictionary> &args, const Array &def_args);
	DefaultData arguments_get_formatted_value(const ArgumentData &arg_data, const Variant &def_val);
	String arguments_string_decl(const TypedArray<Dictionary> &args, bool with_defaults, std::vector<DefaultData> def_args_data = {});
	String arguments_string_call(const TypedArray<Dictionary> &args, const std::vector<DefaultData> &def_remap);
	void line(const String &str = "");
	void log(const String &str = "", const int &indent = 0);
	void log_warning(const String &str = "", const int &indent = 0);
	IndentGuard tab();
};