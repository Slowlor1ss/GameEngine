#include "BigginPCH.h"
#include "TrashTheCache.h"
#include <chrono>
#include <numeric>
#include "imgui_plot.h"

using namespace biggin;

void TrashTheCache::RenderUi()
{
	RenderExercise1();
	RenderExercise2();
}

void TrashTheCache::RenderExercise1()
{
	static ImU32 color = ImColor(213, 100, 15);

	ImGui::SetNextWindowSize(ImVec2(400, 450));
	if (!ImGui::Begin("Exercise 1", m_pOpenEx1))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::InputInt("# samples", &m_Ex1Samples);
	if (ImGui::Button("Trash the cache"))
		m_ClickedEx1++;

	if (m_ClickedEx1 & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Wait for it...");
		TrashTheCacheWInts(m_Ex1Samples);

		++m_ClickedEx1;
	}

	if (m_ClickedEx1 > 0)
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
		Plot("plot1", conf);
	}

	ImGui::End();
}

void TrashTheCache::RenderExercise2()
{
	static const float* y_data[] = { y_data2, y_data3 };
	static ImU32 colors[3] = { ImColor(0, 255, 0), ImColor(255, 0, 0) };

	ImGui::SetNextWindowSize(ImVec2(500, 450));
	if (!ImGui::Begin("Exercise 2", m_pOpenEx2))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::InputInt("# samples", &m_Ex2Samples);
	if (ImGui::Button("Trash the cache with GameObjects3D"))
		m_ClickedEx2++;
	if (m_ClickedEx2 & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Wait for it...");
		TrashTheCacheWObj3D(m_Ex2Samples);

		++m_ClickedEx2;
	}

	if (m_ClickedEx2 > 0)
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
		Plot("Plot GameObjects3D", conf);
	}

	if (ImGui::Button("Trash the cache with GameObjects3DAlt"))
		m_ClickedEx2Alt++;
	if (m_ClickedEx2Alt & 1)
	{
		ImGui::SameLine();
		ImGui::Text("Wait for it...");
		TrashTheCacheWObj3DAlt(m_Ex2Samples);

		++m_ClickedEx2Alt;
	}

	if (m_ClickedEx2Alt > 0)
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
		Plot("Plot GameObjects3DAlt", conf);
	}

	if (m_ClickedEx2 > 0 && m_ClickedEx2Alt > 0)
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
		Plot("plot1", conf);
	}

	ImGui::End();
}


void TrashTheCache::TrashTheCacheWInts(unsigned int sampleSize)
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

void TrashTheCache::TrashTheCacheWObj3D(unsigned int sampleSize)
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

void TrashTheCache::TrashTheCacheWObj3DAlt(unsigned int sampleSize)
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

