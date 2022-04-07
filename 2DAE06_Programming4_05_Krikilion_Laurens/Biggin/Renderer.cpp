#include "BigginPCH.h"
#include "Renderer.h"
#include <chrono>
#include "Box2dManager.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl.h"
#include "SceneManager.h"
#include "Texture2D.h"

int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}

void biggin::Renderer::Init(SDL_Window * window)
{
	m_Window = window;
	m_Renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_Renderer == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL2_Init();
}

void biggin::Renderer::Render() const
{
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_Renderer);

	SceneManager::GetInstance().Render();
	Box2dManager::GetInstance().RenderDebug();

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	SceneManager::GetInstance().RenderUi();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	SDL_RenderPresent(m_Renderer);
}

void biggin::Renderer::Destroy()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (m_Renderer != nullptr)
	{
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;
	}
}

void biggin::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	SDL_RenderCopy(m_Renderer, texture.GetSDLTexture(), nullptr, &dst);
}

void biggin::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width);
	dst.h = static_cast<int>(height);
	SDL_RenderCopy(m_Renderer, texture.GetSDLTexture(), nullptr, &dst);
}

void biggin::Renderer::RenderPolygon(glm::vec2* points, int vertexCount, const SDL_Color& color) const
{
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);

	glm::vec2& point1{ points[vertexCount - 1] };
	for (size_t i{ 0 }; i < vertexCount; i += 2)
	{
		const glm::vec2& point2{ points[i + 1] };
		//SDL_RenderDrawLine(m_Renderer, point1.x, point1.y, point2.x, point2.y);
		SDL_RenderDrawLineF(m_Renderer, point1.x, point1.y, point2.x, point2.y);
		point1 = point2;
	}
}

void biggin::Renderer::RenderPolygon(std::vector<glm::vec2> points, const SDL_Color& color) const
{
	RenderPolygon(points.data(), static_cast<int>(points.size()), color);
}

