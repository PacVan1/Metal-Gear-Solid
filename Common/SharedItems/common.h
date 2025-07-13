#pragma once

// standard
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ostream>
#include <filesystem>
#include <cstdint> 
#include <vector>
#include <algorithm>
#include <string.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <deque>
#include <map>
#include <thread>		// multi-threading
#include <future>		// returning values from thread
#include <type_traits>	// SFINAE
#include <memory>
#include <random>

using std::cout;
using std::endl;
using std::make_shared;
using std::make_unique;
using std::unordered_map;
using std::string_view;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;

// external
#include "ImGui-master/imgui.h"
#include "ImGui-master/backends/imgui_impl_opengl3.h"

// additional
//#include "time.h"
#include "types.h"
#include "debug_log.h"
#include "settings.h"
#include "maths.h"
#include "directions2.h"
#include "transform.h"
#include "sgnode.h"
#include "physics.h"
#include "texture.h"
#include "ogl.h"
#include "light.h"
#include "perlin.h"
#include "IInput.h"
#include "IWindow.h"
#include "controls.h"
#include "camera.h"
#include "static_model.h"
#include "fog.h"
#include "skeletal_model.h"
#include "gui_element_3d.h"
#include "colliders.h"
#include "ray_caster.h"
#include "health.h"
#include "inventory.h"
#include "firearm.h"
#include "path.h"
#include "actors.h"
#include "scene_graph.h"
#include "object_pool.h"
#include "scene.h"
#include "skybox.h"
#include "resources.h"
#include "gui.h"
#include "gui2.h"
#include "editor.h"
#include "state_machine.h"
#include "Game.h"

// source Tmpl8: https://github.com/jbikker/tmpl8
class Timer
{
public:
	Timer() { Reset(); }
	float Elapsed() const
	{
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - mStart);
		return (float)time_span.count();
	}
	void Reset() { mStart = std::chrono::high_resolution_clock::now(); }
	std::chrono::high_resolution_clock::time_point mStart;
};
