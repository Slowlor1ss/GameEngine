#pragma once
#include "Component.h"
#include "Transform.h"

namespace biggin
{
	class TrashTheCache final : public Component
	{
	public:
		explicit TrashTheCache(GameObject* go) : Component(go) {};

		void Start() override {};
		void Update() override {};
		void FixedUpdate() override {};
		void Render() const override {};
		void RenderUi() override;

	private:
		void RenderExercise1();
		void RenderExercise2();

		bool* m_pOpenEx1{ nullptr };
		bool* m_pOpenEx2{ nullptr };

		static constexpr size_t buf_size = 11;
		float x_data[buf_size]{};
		float y_data1[buf_size]{};

		float x_data2[buf_size]{};
		float y_data2[buf_size]{};
		float y_data3[buf_size]{};
		int m_Ex1Samples{10};
		int m_ClickedEx1{};
		int m_Ex2Samples{10};
		int m_ClickedEx2{};
		int m_ClickedEx2Alt{};

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

		void TrashTheCacheWInts(unsigned int sampleSize);
		void TrashTheCacheWObj3D(unsigned int sampleSize);
		void TrashTheCacheWObj3DAlt(unsigned int sampleSize);

	};
}
