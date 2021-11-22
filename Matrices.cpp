#include <iostream>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

void PrintMatrix(const glm::mat4& M) {
	for (int i = 0; i < M.length(); i++) {
		for (int j = 0; j < M.length(); j++) {
			std::cout << std::setw(10) << std::setprecision(4) << std::fixed << M[j][i] << " ";
		}

		std::cout << std::endl;
	}
}

void TranslationMatrix() {
	std::cout << std::endl;

	std::cout << "==================" << std::endl;
	std::cout << "Translation Matrix" << std::endl;
	std::cout << "==================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();
	glm::vec3 T{ 10, 10, 10 };
	glm::mat4 Translation = glm::translate(I, T);

	PrintMatrix(Translation);

	glm::vec4 Position{ 10, 10, 10, 1 };
	glm::vec4 Direction { 10, 10, 10, 0 };

	std::cout << std::endl;

	Position = Translation * Position;
	Direction = Translation * Direction;

	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
	
}

void ScaleMatrix() {
	std::cout << std::endl;

	std::cout << "==================" << std::endl;
	std::cout << "Scale Matrix" << std::endl;
	std::cout << "==================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();
	glm::vec3 ScaleAmount{ 2, 2, 2 };
	glm::mat4 Scale = glm::scale(I, ScaleAmount);

	PrintMatrix(Scale);

	glm::vec4 Position{ 100, 100, 0, 1 };
	glm::vec4 Direction{ 100, 100, 0, 0 };

	Position = Scale * Position;
	Direction = Scale * Direction;

	std::cout << std::endl;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

void RotationMatrix() {
	std::cout << std::endl;

	std::cout << "==================" << std::endl;
	std::cout << "Rotation Matrix" << std::endl;
	std::cout << "==================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();
	constexpr float Angle = glm::radians(90.0f);
	glm::vec3 Axis{ 0, 0, 1 };
	glm::mat4 Rotation = glm::rotate(I, Angle, Axis);

	PrintMatrix(Rotation);

	glm::vec4 Position{ 100, 0, 0, 1 };
	glm::vec4 Direction{ 100, 0, 0, 0 };

	Position = Rotation * Position;
	Direction = Rotation * Direction;

	std::cout << std::endl;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

void ComposedMatrix() {
	std::cout << std::endl;

	std::cout << "==================" << std::endl;
	std::cout << "Composed Matrix" << std::endl;
	std::cout << "==================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();

	glm::vec3 T{ 0, 10, 0 };
	glm::mat4 Translation = glm::translate(I, T);

	constexpr float Angle = glm::radians(45.0f);
	glm::vec3 Axis{ 0, 0, 1 };
	glm::mat4 Rotation = glm::rotate(I, Angle, Axis);

	glm::vec3 ScaleAmount{ 2, 2, 0 };
	glm::mat4 Scale = glm::scale(I, ScaleAmount);

	glm::vec4 Position{ 1, 1, 0, 1 };
	glm::vec4 Direction{ 1, 1, 0, 0 };

	// A * B !== B * A
	// ModelMatrix
	glm::mat4 Transform = Translation * Rotation * Scale;

	PrintMatrix(Transform);

	Position = Transform * Position;
	Direction = Transform * Direction;

	std::cout << std::endl;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

int main() {
	TranslationMatrix();
	ScaleMatrix();
	RotationMatrix();
	ComposedMatrix();

	return 0;
}