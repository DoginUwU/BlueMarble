#include <iostream>
#include <stdexcept>
#include <cassert>
#include <array>
#include <fstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

const int Width = 800;
const int Height = 600;

std::string ReadFile(const char* FilePath) {
	std::string FileContents;
	std::ifstream FileStream{ FilePath, std::ios::in };

	if(FileStream) {
		// Ler dentro do FileStream o conteudo do arquivo e adicionar dentro do FileContents
		FileContents.assign(std::istreambuf_iterator<char>(FileStream), std::istreambuf_iterator<char>());
	}

	return FileContents;
}

void CheckShader(GLuint ShaderId) {
	// ShaderId é um identificador de um shader já compilado

	GLint Result = GL_TRUE;
	glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE) {
		// Houve um erro ao compilar o shader

		// Obter o tamanho do log
		GLint InfoLogLength = 0;
		glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength == 0)
			return;

		std::string ShaderInfoLog(InfoLogLength, '\0');
		glGetShaderInfoLog(ShaderId, InfoLogLength, nullptr, &ShaderInfoLog[0]);

		std::cout << "[ERROR][SHADER] " << ShaderInfoLog << std::endl;
		assert(false);
	}
}

GLuint LoadShaders(const char* VertexShaderFile, const char* FragmentShaderFile) {
	std::string VertexShaderSource = ReadFile(VertexShaderFile);
	std::string FragmentShaderSource = ReadFile(FragmentShaderFile);

	assert(!VertexShaderSource.empty());
	assert(!FragmentShaderSource.empty());

	// Criar os identificadoes do Vertex e Fragment Shaders
	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	std::cout << "[COMPILE] " << VertexShaderFile << std::endl;
	const char* VertexShaderSourcePtr = VertexShaderSource.c_str();

	// Indica para o Shader com id 'tal' o código fonte dele 
	glShaderSource(VertexShaderId, 1, &VertexShaderSourcePtr, nullptr);

	// Compila o Shader para o OpenGL
	glCompileShader(VertexShaderId);

	// Verifica se a compilação do shaders retornou um erro
	CheckShader(VertexShaderId);

	// ===============================================================

	std::cout << "[COMPILE] " << FragmentShaderFile << std::endl;
	const char* FragmentShaderSourcePtr = FragmentShaderSource.c_str();

	// Indica para o Shader com id 'tal' o código fonte dele 
	glShaderSource(FragmentShaderId, 1, &FragmentShaderSourcePtr, nullptr);

	// Compila o Shader para o OpenGL
	glCompileShader(FragmentShaderId);

	// Verifica se a compilação do shaders retornou um erro
	CheckShader(FragmentShaderId);

	std::cout << "[LINK] Shader" << std::endl;
	GLuint ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);

	// Verificar se o programa foi linkado corretamente 
	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE) {
		// Pegar log para saber o problema
		std::cout << "[ERROR][LINK] Undefined" << std::endl;

		assert(false);
	}

	// Deletar Shaders já utilizados
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	return ProgramId;
}

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
		return -1;
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

	GLuint ProgramId = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

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