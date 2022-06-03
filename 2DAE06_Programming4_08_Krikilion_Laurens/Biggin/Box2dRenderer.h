#pragma once
#pragma warning(push, 0)
#include <Box2D/Common/b2Draw.h>
#pragma warning(pop)

class Box2dRenderer final : public b2Draw
{
public:
	explicit Box2dRenderer() = default;
	~Box2dRenderer() override = default;

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
	void DrawSolidPolygon(const b2Vec2* /*vertices*/, int32 /*vertexCount*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented"); };
	void DrawCircle(const b2Vec2& /*center*/, glm::float32 /*radius*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented"); };
	void DrawSolidCircle(const b2Vec2& /*center*/, glm::float32 /*radius*/, const b2Vec2& /*axis*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented"); };
	void DrawSegment(const b2Vec2& /*p1*/, const b2Vec2& /*p2*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented"); };
	void DrawTransform(const b2Transform& /*xf*/) override { throw std::runtime_error("Function not implemented"); };
	void DrawPoint(const b2Vec2& /*p*/, glm::float32 /*size*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented"); };
};

