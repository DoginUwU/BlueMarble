#include <iostream>
#include <stdexcept>
#include <cassert>

#include <GLFW/glfw3.h>

const int Width = 800;
const int Height = 600;

int main() {
	// glfwInit = Inicializa o GLFW
	if (glfwInit() == GLFW_FALSE)
		throw std::runtime_error("[ERROR] GLFW NOT INITIALIZED CORRECTLY");
	
	// Criar uma janela
	GLFWwindow* Window = glfwCreateWindow(Width, Height, "BlueMarble", nullptr, nullptr);
	assert(Window);

	// Loop de eventos
	while (!glfwWindowShouldClose(Window)) {
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