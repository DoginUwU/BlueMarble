#include <iostream>
#include <stdexcept>
#include <cassert>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

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

	// Definir a cor de fundo da janela
	glClearColor(0.43f, 0.65f, 1.0f, 1.0f);

	// Loop de eventos
	while (!glfwWindowShouldClose(Window)) {
		// glClear vai limpar o framebuffer
		// GL_COLOR_BUFFER_BIT limpa o buffer de cor, para que ele possa preencher com a cor que foi configurada no glClearColor()
		glClear(GL_COLOR_BUFFER_BIT);

		// Processar todos os eventos da fila de eventos do GLFW
		// Podendo ser eventos do teclado, mouse, GamePad
		glfwPollEvents();

		// Envia o conteúdo do frameBuffer da janela para ser desenhado na tela
		// Enviando a memória do backbuffer para o frontbuffer (placa de vídeo > monitor)
		glfwSwapBuffers(Window);
	}

	// glfwTerminate = Encerra o GLFW
	glfwTerminate();

	return 0;
}