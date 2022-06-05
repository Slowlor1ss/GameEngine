#include "BigginPCH.h"
#include "Renderer.h"
#include <chrono>
#include "Box2dManager.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_sdl.h"
#include "MenuRenderer.h"
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
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL2_Init();
}

void biggin::Renderer::Render() const
{
	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_Renderer);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);
	ImGui::NewFrame();

	SceneManager::GetInstance().Render();
	//ImGui::ShowDemoWindow();
	MenuRenderer::GetInstance().RenderMenu();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//for some reason this had to be rendered after the imgui otherwise text break as soon as box 2d renders its debug
	SceneManager::GetInstance().RenderDebug();

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

void biggin::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, SDL_RendererFlip flip) const
{
	SDL_Rect dst{};
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);
	dst.w = static_cast<int>(width);
	dst.h = static_cast<int>(height);
	SDL_RenderCopyEx(m_Renderer, texture.GetSDLTexture(), nullptr, &dst, 0, nullptr, flip);
}

void biggin::Renderer::RenderTexture(const Texture2D& texture, const glm::vec2& pos, const glm::vec2& sizeCell, SDL_RendererFlip flip) const
{
	RenderTexture(texture, pos.x, pos.y, sizeCell.x, sizeCell.y, flip);
}

void biggin::Renderer::RenderTexture(const Texture2D& texture, const SDL_Rect* dst, const SDL_Rect* src, SDL_RendererFlip flip) const
{
	SDL_RenderCopyEx(m_Renderer, texture.GetSDLTexture(), src, dst, 0, nullptr, flip);
}

void biggin::Renderer::RenderPolygon(const glm::vec2* points, int vertexCount, SDL_Color color, bool closed) const
{
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);

	//close polygon
	if (closed)
	{
		const glm::vec2& firstPoint{ points[0] };
		const glm::vec2& lastPoint{ points[vertexCount - 1] };
		SDL_RenderDrawLineF(m_Renderer, firstPoint.x, firstPoint.y, lastPoint.x, lastPoint.y);
	}

	for (int i{ 0 }; i < vertexCount - 1; i += 1)
	{
		const glm::vec2& point1{ points[i] };
		const glm::vec2& point2{ points[i + 1] };
		SDL_RenderDrawLineF(m_Renderer, point1.x, point1.y, point2.x, point2.y);
		//point1 = point2;
	}
}

void biggin::Renderer::RenderPolygon(std::vector<glm::vec2> points, SDL_Color color, bool closed) const
{
	RenderPolygon(points.data(), static_cast<int>(points.size()), color, closed);
}

void biggin::Renderer::RenderPoint(glm::vec2 point, SDL_Color color) const
{
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawPointF(m_Renderer, point.x, point.y);
}

//https://stackoverflow.com/questions/38334081/howto-draw-circles-arcs-and-vector-graphics-in-sdl
void biggin::Renderer::RenderCircle(glm::vec2 point, int32_t radius, SDL_Color color) const
{
	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);

	const int32 centreX{ static_cast<int32>(point.x) };
	const int32 centreY{ static_cast<int32>(point.y) };

	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(m_Renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(m_Renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(m_Renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(m_Renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(m_Renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(m_Renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(m_Renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(m_Renderer, centreX - y, centreY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
