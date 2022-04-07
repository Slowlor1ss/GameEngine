#pragma once
#include <Box2D/Common/b2Draw.h>

class Box2dRenderer final : public b2Draw
{
public:
	explicit Box2dRenderer() = default;
	~Box2dRenderer() override = default;

	void DrawPolygon(const b2Vec2* /*vertices*/, int32 /*vertexCount*/, const b2Color& /*color*/) override {};
	void DrawSolidPolygon(const b2Vec2* /*vertices*/, int32 /*vertexCount*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented yet"); };
	void DrawCircle(const b2Vec2& /*center*/, float32 /*radius*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented yet"); };
	void DrawSolidCircle(const b2Vec2& /*center*/, float32 /*radius*/, const b2Vec2& /*axis*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented yet"); };
	void DrawSegment(const b2Vec2& /*p1*/, const b2Vec2& /*p2*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented yet"); };
	void DrawTransform(const b2Transform& /*xf*/) override { throw std::runtime_error("Function not implemented yet"); };
	void DrawPoint(const b2Vec2& /*p*/, float32 /*size*/, const b2Color& /*color*/) override { throw std::runtime_error("Function not implemented yet"); };
};

