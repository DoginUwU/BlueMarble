#include <iostream>
#include <stdexcept>
#include <cassert>
#include <array>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

const int Width = 800;
const int Height = 600;

int main() {
	// glfwInit = Inicializa o GLFW
	assert(glfwInit() == GLFW_TRUE);

	// Criar uma janela
	GLFWwindow* Window = glfwCreateWindow(Width, Height, "Blue Marble", nullptr, nullptr);
	assert(Window);

	// Manter a janela atual como o contexto ativo para o resto do OpenGL
	glfwMakeContextCurrent(Window);

	// Inicializar a biblioteca GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "[ERROR] %s\n", glewGetErrorString(err));
		return 0;
	}

	// Verificar a versão do OpenGL que está sendo usada
	GLint GLMajorVersion = 0, GLMinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &GLMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &GLMinorVersion);
	std::cout << "OpenGL Version : " << GLMajorVersion << "." << GLMinorVersion << std::endl;

	// Usar o glGetString() para obter informações do driver que está sendo usado
	std::cout << "OpenGL Vendor  : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Render  : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GLSL Version   : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Definir um triângulo em coordenadas normalizadas
	std::array<glm::vec3, 3> Triangle = {
		glm::vec3{ -1.0f, -1.0f, 0.0f },
		glm::vec3{ 1.0f, -1.0f, 0.0f },
		glm::vec3{ 0.0f, 1.0f, 0.0f },
	};

	// Model Matrix
	glm::mat4 ModelMatrix = glm::identity<glm::mat4>();

	// View Matrix
	glm::vec3 Eye{ 0, 0, 10 };
	glm::vec3 Center{ 0, 0, 0 };
	glm::vec3 Up{ 0, 1, 0 };
	glm::mat4 ViewMatrix = glm::lookAt(Eye, Center, Up);

	// Projection Matrix
	constexpr float FoV = glm::radians(45.0f);
	const float AspectRatio = Width / Height;
	const float Near = 0.001f;
	const float Far = 1000.0f;
	glm::mat4 ProjectionMatrix = glm::perspective(FoV, AspectRatio, Near, Far);

	// ModelViewProjection
	glm::mat4 ModelViewProjection = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Aplicar a ModelViewProjection nos vértices do triângulo
	for (glm::vec3& Vertex : Triangle) {
		glm::vec4 ProjectedVertex = ModelViewProjection * glm::vec4{ Vertex, 1.0f };

		ProjectedVertex /= ProjectedVertex.w;
		Vertex = ProjectedVertex;
	}

	// Copiar os vértices do trinagulo para a memória da GPU
	GLuint VertexBuffer;

	// Gerar identificador do VertexBuffer
	glGenBuffers(1, &VertexBuffer);

	// Ativar o VertexBuffer atual como sendo o buffer para onde será copiado os dados do triangulo
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	// Copiar os dados para a memória de vídeo
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle.data(), GL_STATIC_DRAW);

	// Definir a cor de fundo da janela
	glClearColor(0.43f, 0.65f, 1.0f, 1.0f);

	// Loop de eventos
	while (!glfwWindowShouldClose(Window)) {
		// glClear vai limpar o framebuffer
		// GL_COLOR_BUFFER_BIT limpa o buffer de cor, para que ele possa preencher com a cor que foi configurada no glClearColor()
		glClear(GL_COLOR_BUFFER_BIT);

		glEnableVertexAttribArray(0);

		// Fala para o OpenGL que o VertexBuffer irá ser o buffer ativo no momento
		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

		// Informa ao OpenGL onde, dentro do VertexBuffer, os vértices estão
		// Array Triangles é contido em memória, logo falaremos quantos vértices vamos usar para desenhar o triângulo
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// Informa ao OpenGL desenhar o triângulo com os dados que estão armazenados no VertexBuffer
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Reverter o estado de buffer criado
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);

		// Processar todos os eventos da fila de eventos do GLFW
		// Podendo ser eventos do teclado, mouse, GamePad
		glfwPollEvents();

		// Envia o conteúdo do frameBuffer da janela para ser desenhado na tela
		// Enviando a memória do backbuffer para o frontbuffer (placa de vídeo > monitor)
		glfwSwapBuffers(Window);
	}

	// Desalocar o VertexBuffer
	glDeleteBuffers(1, &VertexBuffer);

	// glfwTerminate = Encerra o GLFW
	glfwTerminate();

	return 0;
}