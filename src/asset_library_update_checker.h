#pragma once

#ifndef DISABLE_DEBUG_RENDERING
#ifdef DEBUG_ENABLED

#include "utils.h"

GODOT_WARNING_DISABLE()
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
GODOT_WARNING_RESTORE()

using namespace godot;

class AssetLibraryUpdateChecker : public RefCounted {
	GDCLASS(AssetLibraryUpdateChecker, RefCounted)

private:
	HTTPRequest *request = nullptr;
	String addon_name;
	String repository_name;
	String root_settings_section;

protected:
	static void _bind_methods();

public:
	void request_completed(int result, int response_code, PackedStringArray headers, PackedByteArray body);
	void init();

	AssetLibraryUpdateChecker();
};

#endif
#endif