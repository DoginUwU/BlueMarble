#include <iostream>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

void Constructors() {
	std::cout << std::endl;

	glm::vec2 Point0{ 10.0f, 10.0f };
	glm::vec3 Point1{ 10.f, 0.0f, 0.0f };
	glm::vec4 Point2{ 10.f, 0.0f, 0.0f, 0.0f };
	// inteiros
	glm::ivec2 Point3{ 3, 4 };
	// booleanos
	glm::bvec3 Point4{ false, true, false };
	// doubles
	glm::dvec2 Point5{ 10.0, 10.0 };
	// using vec2 to make vec3
	glm::vec3 Point6{ Point0, 1.0f };

	std::cout << "Point0: " << glm::to_string(Point0) << std::endl;
	std::cout << "Point1: " << glm::to_string(Point1) << std::endl;
	std::cout << "Point2: " << glm::to_string(Point2) << std::endl;
	std::cout << "Point3: " << glm::to_string(Point3) << std::endl;
	std::cout << "Point4: " << glm::to_string(Point4) << std::endl;
	std::cout << "Point5: " << glm::to_string(Point5) << std::endl;
	std::cout << "Point6: " << glm::to_string(Point6) << std::endl;
}

void Components() {
	std::cout << std::endl;

	glm::vec3 P{ 1, 2, 3 };
	std::cout << "X: " << P.x << " Y: " << P.y << " Z: " << P.z << std::endl;
	std::cout << "R: " << P.r << " G: " << P.g << " B: " << P.b << std::endl;
	std::cout << "0: " << P[0] << " 1: " << P[1] << " 2: " << P[2] << std::endl;
}

void Swizzle() {
	std::cout << std::endl;

	glm::vec4 P{ 1, 2, 3, 4 };
	glm::vec3 Q = P.xxx;
	glm::vec3 R = P.yxy;
	glm::vec4 T = P.xyzw;

	std::cout << glm::to_string(T) << std::endl;
}

void Operations() {
	std::cout << std::endl;

	glm::vec3 P0{ 10.0f, 10.0f, 0.0f };
	glm::vec3 P1{ 10.0f, 10.0f, 10.0f };

	glm::vec3 R;

	// Soma
	R = P0 + P1;
	std::cout << glm::to_string(R) << std::endl;

	// Subtrãção
	R = P0 - P1;
	std::cout << glm::to_string(R) << std::endl;

	// Multiplicação
	R = P0 * P1;
	std::cout << glm::to_string(R) << std::endl;

	// Divisão
	R = P0 / P1;
	std::cout << glm::to_string(R) << std::endl;

	// Scala
	R = P0 * 5.0f;
	std::cout << glm::to_string(R) << std::endl;

	// Scala de Divisão
	R = P0 / 5.0f;
	std::cout << glm::to_string(R) << std::endl;

	// Tamanho do vetor
	float L = glm::length(P0);
	std::cout << L << std::endl;

	// Normalizar um vetor
	glm::vec3 Norm = glm::normalize(P0);
	// P0.xyz / Length
	std::cout << glm::to_string(Norm) << std::endl;

	// Produto Escalar (Dot Product)
	float Dot = glm::dot(P0, P1);
	std::cout << Dot << std::endl;

	// Produto Vetorial (Cross Product)
	glm::vec3 Cross = glm::cross(P0, P1);
	std::cout << glm::to_string(Cross) << std::endl;

	// Distância
	float Distance = glm::distance(P0, P1);
	std::cout << Distance << std::endl;

	// Refração
	glm::vec3 Refract = glm::refract(P0, Norm, 1.0f);
	std::cout << glm::to_string(Refract) << std::endl;

	// Reflect
	glm::vec3 Reflect = glm::reflect(P1, Norm);
	std::cout << glm::to_string(Reflect) << std::endl;
}

int main() {
	Constructors();
	Components();
	Swizzle();
	Operations();

	return 0;
}