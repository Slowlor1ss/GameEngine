#pragma once
#include <chrono>
#include <numeric>
#include <algorithm>
#include <vector>
#include "imgui.h"
#include "imgui_plot.h"

/**
 * \brief ImGui and plot code
 */

 //a global function just because this is code for the exercise
void Exercise1(bool* p_open = NULL);
void Exercise2(bool* p_open = NULL);

constexpr size_t buf_size = 11;
static float x_data[buf_size];
static float y_data1[buf_size];

static float x_data2[buf_size];
static float y_data2[buf_size];
static float y_data3[buf_size];

inline void TrashTheCacheWInts(unsigned int sampleSize)
{
	std::vector<std::vector<float>> data(11);
	constexpr int size{ 64 * 1024 * 1024 };
	//std::vector<int> v(64 * 1024 * 1024, 1);
	int* v = new int[size];
	std::fill_n(v, size, 0);

	for (size_t i{ 0 }; i < sampleSize; ++i)
	{
		int iteration{ 0 };
		for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
		{
			auto start = std::chrono::high_resolution_clock::now();

			for (int j = 0; j < size; j += stepsize) v[j] *= 2;

			auto end = std::chrono::high_resolution_clock::now();
			const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();
			data[iteration].push_back(elapsedTime / 1000.f);
			++iteration;
		}
	}
	delete[] v;

	//sort all data
	for (size_t i{ 0 }; i < data.size(); ++i)
	{
		if (sampleSize == 1)
		{
			y_data1[i] = data[i][0];
			x_data[i] = static_cast<float>(i);
			continue;
		}
		auto& vector = data[i];

		std::ranges::sort(vector);

		//delete first and last
		vector.erase(vector.begin());
		vector.pop_back();

		//save average
		y_data1[i] = std::reduce(vector.begin(), vector.end()) / vector.size();

		x_data[i] = static_cast<float>(i);
	}
}

struct Transform
{
	float matrix[16] = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 };
};

class GameObject3DAlt
{
public:
	Transform* transform{ nullptr };
	int ID{};
};
class GameObject3D
{
public:
	Transform transform{};
	int ID{};
};

inline void TrashTheCacheWObj3D(unsigned int sampleSize)
{
	std::vector<std::vector<float>> data(11);
	constexpr int size{ 24 * 1024 * 1024 };
	//std::vector<GameObject3D> v(64 * 1024 * 1024);
	//sizeof(GameObject3D);
	auto* v = new GameObject3D[size];

	for (size_t i{ 0 }; i < sampleSize; ++i)
	{
		int iteration{ 0 };
		for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
		{
			auto start = std::chrono::high_resolution_clock::now();

			for (int j = 0; j < size; j += stepsize) v[j].ID *= 2;

			auto end = std::chrono::high_resolution_clock::now();
			const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();
			data[iteration].push_back(elapsedTime / 1000.f);
			++iteration;
		}
	}
	delete[] v;

	//sort all data
	for (size_t i{ 0 }; i < data.size(); ++i)
	{
		if (sampleSize == 1)
		{
			y_data2[i] = data[i][0];
			x_data2[i] = static_cast<float>(i);
			continue;
		}
		auto& vector = data[i];

		std::ranges::sort(vector);

		//delete first and last
		vector.erase(vector.begin());
		vector.pop_back();

		//save average
		y_data2[i] = std::reduce(vector.begin(), vector.end()) / vector.size();

		x_data2[i] = static_cast<float>(i);
	}
}

inline void TrashTheCacheWObj3DAlt(unsigned int sampleSize)
{
	std::vector<std::vector<float>> data(11);
	constexpr int size{ 24 * 1024 * 1024 };
	auto* v = new GameObject3DAlt[size];

	for (size_t i{ 0 }; i < sampleSize; ++i)
	{
		int iteration{ 0 };
		for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
		{
			auto start = std::chrono::high_resolution_clock::now();

			for (int j = 0; j < size; j += stepsize)
				v[j].ID *= 2;

			auto end = std::chrono::high_resolution_clock::now();
			const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();
			data[iteration].push_back(elapsedTime / 1000.f);
			++iteration;

			//prevent compiler from optimizing operation away


		}
	}

	delete[] v;


	//sort all data
	for (size_t i{ 0 }; i < data.size(); ++i)
	{
		if (sampleSize == 1)
		{
			y_data3[i] = data[i][0];
			x_data2[i] = static_cast<float>(i);
			continue;
		}
		auto& vector = data[i];

		std::ranges::sort(vector);

		//delete first and last
		vector.erase(vector.begin());
		vector.pop_back();

		//save average
		y_data3[i] = std::reduce(vector.begin(), vector.end()) / vector.size();

		x_data2[i] = static_cast<float>(i);
	}
}

inline void Exercise1(bool* p_open)
{
	static ImU32 color = ImColor(213, 100, 15);

	ImGui::SetNextWindowSize(ImVec2(400, 450));
	if (!ImGui::Begin("Exercise 1", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	static int samples = 10;
	ImGui::InputInt("# samples", &samples);
	static int clicked = 0;
	if (ImGui::Button("Trash the cache"))
		clicked++;
	if (clicked & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Wait for it...");
		TrashTheCacheWInts(samples);

		++clicked;
	}

	if (clicked > 0)
	{
		ImGui::PlotConfig conf;
		conf.values.xs = x_data;
		conf.values.count = buf_size;
		conf.values.ys = y_data1;
		conf.values.color = color;
		conf.scale.min = -1;
		conf.scale.max = y_data1[0] + 10;
		conf.tooltip.show = true;
		conf.grid_x.show = false;
		conf.grid_y.show = true;
		conf.grid_y.size = 10;
		conf.grid_y.subticks = 5;
		conf.selection.show = false;
		conf.frame_size = ImVec2(300, 150);
		ImGui::Plot("plot1", conf);
	}

	ImGui::End();
}

inline void Exercise2(bool* p_open)
{
	static const float* y_data[] = { y_data2, y_data3 };
	static ImU32 colors[3] = { ImColor(0, 255, 0), ImColor(255, 0, 0) };

	//if (!ImGui::Begin("Exercise 3", p_open, ImGuiWindowFlags_AlwaysAutoResize))
	ImGui::SetNextWindowSize(ImVec2(500, 450));
	if (!ImGui::Begin("Exercise 2", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	static int samples = 10;
	ImGui::InputInt("# samples", &samples);
	static int clickedGameObjects = 0;
	if (ImGui::Button("Trash the cache with GameObjects3D"))
		clickedGameObjects++;
	if (clickedGameObjects & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Wait for it...");
		TrashTheCacheWObj3D(samples);

		++clickedGameObjects;
	}

	if (clickedGameObjects > 0)
	{
		ImGui::PlotConfig conf;
		conf.values.xs = x_data2;
		conf.values.count = buf_size;
		conf.values.ys = y_data[0];
		conf.values.color = colors[0];
		conf.scale.min = -1;
		conf.scale.max = y_data[0][0] + 10;
		conf.tooltip.show = true;
		conf.grid_x.show = false;
		conf.grid_y.show = true;
		conf.grid_y.size = 10;
		conf.grid_y.subticks = 5;
		conf.selection.show = false;
		conf.frame_size = ImVec2(250, 100);
		ImGui::Plot("Plot GameObjects3D", conf);
	}

	static int clickedGameObjectsAlt = 0;
	if (ImGui::Button("Trash the cache with GameObjects3DAlt"))
		clickedGameObjectsAlt++;
	if (clickedGameObjectsAlt & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Wait for it...");
		TrashTheCacheWObj3DAlt(samples);

		++clickedGameObjectsAlt;
	}

	if (clickedGameObjectsAlt > 0)
	{
		ImGui::PlotConfig conf;
		conf.values.xs = x_data2;
		conf.values.count = buf_size;
		conf.values.ys = y_data[1];
		conf.values.color = colors[0];
		conf.scale.min = -1;
		conf.scale.max = y_data[1][0] + 10;
		conf.tooltip.show = true;
		conf.grid_x.show = false;
		conf.grid_y.show = true;
		conf.grid_y.size = 10;
		conf.grid_y.subticks = 5;
		conf.selection.show = false;
		conf.frame_size = ImVec2(250, 100);
		ImGui::Plot("Plot GameObjects3DAlt", conf);
	}

	if (clickedGameObjects > 0 && clickedGameObjectsAlt > 0)
	{
		ImGui::Text("Combined:");

		ImGui::PlotConfig conf;
		conf.values.xs = x_data;
		conf.values.count = buf_size;
		conf.values.ys_list = y_data;
		conf.values.ys_count = 2;
		conf.values.colors = colors;
		conf.scale.min = -1;
		conf.scale.max = std::max(y_data[1][0], y_data[0][0]) + 10;
		conf.tooltip.show = true;
		conf.grid_x.show = false;
		conf.grid_y.show = true;
		conf.grid_y.size = 10;
		conf.grid_y.subticks = 5;
		conf.selection.show = false;
		conf.frame_size = ImVec2(250, 100);
		ImGui::Plot("plot1", conf);
	}

	ImGui::End();
}