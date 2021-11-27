#include <iostream>
#include <stdexcept>
#include <cassert>
#include <array>
#include <fstream>
#include <vector>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int Width = 800;
int Height = 600;

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

		GLint InfoLogLength = 0;
		glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		std::string ProgramInfoLog(InfoLogLength, '\0');
		glGetProgramInfoLog(ProgramId, InfoLogLength, nullptr, &ProgramInfoLog[0]);

		std::cout << "[ERROR][LINK] " << ProgramInfoLog << std::endl;

		assert(false);
	}

	// Deletar Shaders já utilizados
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	return ProgramId;
}

GLuint LoadTexture(const char* TextureFile) {
	std::cout << "[TEXTURE] " << TextureFile << std::endl;

	stbi_set_flip_vertically_on_load(true);

	int TextureWidth = 0, TextureHeight = 0;
	int NumberOfCompoents = 0;	
	unsigned char* TextureData = stbi_load(TextureFile, &TextureWidth, &TextureHeight, &NumberOfCompoents, 3);

	assert(TextureData);

	// Gerar o identificador da textura
	GLuint TextureId;
	glGenTextures(1, &TextureId);

	// Habilitar a textura para ser modificada
	glBindTexture(GL_TEXTURE_2D, TextureId);

	// Copiar a textura para a memória de vídeo (GPU)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureWidth, TextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);

	// Adicionar filtros (Magnificação e Minificação)
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Configurar o Texture Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S = U
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T = V

	// Gerar o mipmap a partir da textura
	glGenerateMipmap(GL_TEXTURE_2D);

	// Desligar a textura, pois ela já foi copiada para a GPU
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(TextureData);

	return TextureId;
}

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 UV;
};

struct DirectionalLight {
	glm::vec3 Direction;
	GLfloat Intensity;
};

// Unloaded
GLuint LoadGeometry() {
	// Definir um triângulo em coordenadas normalizadas
	std::array<Vertex, 4> Quad = {
		Vertex{ glm::vec3{ -1, -1, 0 }, glm::vec3{ 0, 0, 1 }, glm::vec3{ 1, 0, 0 }, glm::vec2{ 0, 0 } },
		Vertex{ glm::vec3{ 1, -1, 0 }, glm::vec3{ 0, 0, 1 }, glm::vec3{ 0, 1, 0 }, glm::vec2{ 1, 0 } },
		Vertex{ glm::vec3{ 1, 1, 0 }, glm::vec3{ 0, 0, 1 }, glm::vec3{ 1, 0, 0 }, glm::vec2{ 1, 1 } },
		Vertex{ glm::vec3{ -1, 1, 0 }, glm::vec3{ 0, 0, 1 }, glm::vec3{ 0, 0, 1 }, glm::vec2{ 0, 1 } }
	};

	// Definir o Array de elementos que formam os Triângulos
	std::array<glm::ivec3, 2> Indices = {
		glm::ivec3{ 0, 1, 3 },
		glm::ivec3{ 3, 1, 2 },
	};
	

	// Copiar os vértices do trinagulo para a memória da GPU
	GLuint VertexBuffer;

	// Pedir para o OpenGL gerar o identificador do VBO
	glGenBuffers(1, &VertexBuffer);

	// Pedir para o OpenGL gerar o identificador do EBO
	GLuint ElementBuffer = 0;
	glGenBuffers(1, &ElementBuffer);

	// Ativar o VertexBuffer atual como sendo o buffer para onde será copiado os dados do triangulo
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	// Copiar os dados para a memória de vídeo
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad.data(), GL_STATIC_DRAW);

	// Copiar os dados do Element Buffer para a GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices.data(), GL_STATIC_DRAW);

	// Gerar Vertex Array Object (VAO)
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	// Habilitar o VAO
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	// Fala para o OpenGL que o VertexBuffer irá ser o buffer ativo no momento
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	// Informa ao OpenGL onde, dentro do VertexBuffer, os vértices estão
	// Array Triangles é contido em memória, logo falaremos quantos vértices vamos usar para desenhar o triângulo
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

void GenerateSphereMesh(GLuint Resolution, std::vector<Vertex>& Vertices, std::vector<glm::ivec3>& Indices) {
	Vertices.clear();
	Indices.clear();

	constexpr float Pi = glm::pi<float>();
	constexpr float TwoPi = glm::two_pi<float>();
	const float InvResolution = 1.0f / static_cast<float>(Resolution - 1);

	for (GLuint UIndex = 0; UIndex < Resolution; UIndex++)
	{
		const float U = UIndex * InvResolution;
		const float Theta = glm::mix(0.0f, Pi, U);

		for (GLuint VIndex = 0; VIndex < Resolution; VIndex++)
		{
			const float V = VIndex * InvResolution;
			const float Phi = glm::mix(0.0f, TwoPi, V);

			glm::vec3 VertexPosition{
				glm::sin(Theta) * glm::cos(Phi),
				glm::sin(Theta) * glm::sin(Phi),
				glm::cos(Theta)
			};

			Vertex Vertex{
				VertexPosition,
				glm::normalize(VertexPosition),
				glm::vec3{ 1.0f, 1.0f, 1.0f },
				glm::vec2{ U, 1 - V }
			};

			Vertices.push_back(Vertex);
		}
	}

	for (GLuint U = 0; U < Resolution - 1; U++)
	{
		for (GLuint V = 0; V < Resolution - 1; V++)
		{
			GLuint P0 = U + V * Resolution;
			GLuint P1 = (U + 1) + V * Resolution;
			GLuint P2 = (U + 1) + (V + 1) * Resolution;
			GLuint P3 = U + (V + 1) * Resolution;

			Indices.push_back(glm::ivec3{ P0, P1, P3 });
			Indices.push_back(glm::ivec3{ P3, P1, P2 });
		}
	}
}

GLuint LoadSphere(GLuint& NumVertices, GLuint& NumIndices) {
	std::vector<Vertex> Vertices;
	std::vector<glm::ivec3> Triangles;
	GenerateSphereMesh(50, Vertices, Triangles);

	NumVertices = Vertices.size();
	NumIndices = Triangles.size() * 3;

	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	GLuint ElementBuffer;
	glGenBuffers(1, &ElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndices * sizeof(GLuint), Triangles.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

class FlyCamera {

public:
	// Definição da Matriz de View
	glm::vec3 Location{ 0, 0, 5 };
	glm::vec3 Direction{ 0, 0, -1 };
	glm::vec3 Up{ 0, 1, 0 };

	// Definição da Matriz Projection
	float FieldOfView = glm::radians(45.0f);
	float AspectRatio = Width / Height;
	float Near = 0.01f;
	float Far = 1000.0f;

	// Parâmetros de Interatividade
	float Speed = 10.0f;
	float Sensitivity = 0.1f;

	glm::mat4 GetViewProjection() const {
		glm::mat4 Projection = glm::perspective(FieldOfView, AspectRatio, Near, Far);

		return Projection * GetView();
	}

	void MoveForward(float Amount) {
		Location += glm::normalize(Direction) * Amount * Speed;
	}

	void MoveRight(float Amount) {
		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		Location += Right * Amount * Speed;
	}

	void Look(float Yaw, float Pitch) {
		Yaw *= Sensitivity;
		Pitch *= Sensitivity;

		const glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		const glm::mat4 I = glm::identity<glm::mat4>();

		glm::mat4 YawRotation = glm::rotate(I, glm::radians(Yaw), Up);
		glm::mat4 PitchRotation = glm::rotate(I, glm::radians(Pitch), Right);
	
		// Up = PitchRotation * glm::vec4{ Up, 0 };
		Direction = YawRotation * PitchRotation * glm::vec4{ Direction, 0 };
	}

	glm::mat4 GetView() const {
		return glm::lookAt(Location, Location + Direction, Up);
	}
};

FlyCamera Camera;
bool bEnableMouseMovement = false;
glm::vec2 PreviousCursor{ 0, 0 };

void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers) {
	if (Button == GLFW_MOUSE_BUTTON_LEFT) {
		if (Action == GLFW_PRESS) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double X, Y;
			glfwGetCursorPos(Window, &X, &Y);

			PreviousCursor = glm::vec2{ X, Y };

			bEnableMouseMovement = true;
		}

		if (Action == GLFW_RELEASE) {
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			bEnableMouseMovement = false;
		}
	}
 }

void MouseMotionCallback(GLFWwindow* Window, double X, double Y) {
	if (!bEnableMouseMovement) return;

	glm::vec2 CurrentCursor{ X, Y };
	glm::vec2 DeltaCursor = CurrentCursor - PreviousCursor;

	// std::cout << "[DELTA] " << glm::to_string(DeltaCursor) << std::endl;

	Camera.Look(-DeltaCursor.x, -DeltaCursor.y);

	PreviousCursor = CurrentCursor;
}

void ResizeCallback(GLFWwindow* Window, int NewWidth, int NewHeight) {
	Width = NewWidth;
	Height = NewHeight;

	Camera.AspectRatio = static_cast<float>(Width) / Height;
	glViewport(0, 0, Width, Height);
}

int main() {
	// Inicializar o GLFW
	assert(glfwInit() == GLFW_TRUE);

	// Criar uma janela
	GLFWwindow* Window = glfwCreateWindow(Width, Height, "Blue Marble", nullptr, nullptr);
	assert(Window);

	// Cadastrar as callbacks no GLFW
	glfwSetMouseButtonCallback(Window, MouseButtonCallback);
	glfwSetCursorPosCallback(Window, MouseMotionCallback);
	glfwSetFramebufferSizeCallback(Window, ResizeCallback);

	// Manter a janela atual como o contexto ativo para o resto do OpenGL
	glfwMakeContextCurrent(Window);
	// Desligar o VSync
	glfwSwapInterval(0);

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
	std::cout << std::endl;

	ResizeCallback(Window, Width, Height);

	GLuint ProgramId = LoadShaders("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

	GLuint TextureId = LoadTexture("textures/earth_2k.jpg");
	GLuint CloudTextureId = LoadTexture("textures/earth_clouds_2k.jpg");

	GLuint ShepereNumVertices = 0;
	GLuint ShepereNumIndices = 0;
	GLuint SphereVAO = LoadSphere(ShepereNumVertices, ShepereNumIndices);

	glm::mat4 I = glm::identity<glm::mat4>();
	glm::mat4 ModelMatrix = glm::rotate(I, glm::radians(90.0f), glm::vec3{ 0, 1, 0 });
	glm::mat4 ModelMatrix2 = glm::translate(I, glm::vec3{ 10, 0, 0 });

	// Definir a cor de fundo da janela
	glClearColor(0, 0, 0, 0);

	// Guardar o tempo do frame anterior 
	double PreviousTime = glfwGetTime();

	// Habilitar o Backface culling
	glEnable(GL_CULL_FACE);
	// Especificar qual face será cortada
	glCullFace(GL_FRONT);

	// Habilitar o teste de profundidade (Z-Buffer)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Criar uma fonte de luz direcional
	DirectionalLight Light{};
	Light.Direction = glm::vec3{ 0, 0, -1.0f };
	Light.Intensity = 1;

	// Loop de eventos
	while (!glfwWindowShouldClose(Window)) {
		double CurrentTime = glfwGetTime();
		double DeltaTime = CurrentTime - PreviousTime;

		if (DeltaTime > 0.0) {
			PreviousTime = CurrentTime;
		}

		// Limpar o framebuffer
		// GL_COLOR_BUFFER_BIT limpa o buffer de cor, para que ele possa preencher com a cor que foi configurada no glClearColor()
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Ativar o programa de shader
		glUseProgram(ProgramId);

		glm::mat4 NormalMatrix = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix));
		glm::mat4 NormalMatrix2 = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix2));

		glm::mat4 ViewProjection = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = ViewProjection * ModelMatrix;

		GLint TimeLoc = glGetUniformLocation(ProgramId, "Time");
		glUniform1f(TimeLoc, CurrentTime);

		GLint ModelViewProjectionLoc = glGetUniformLocation(ProgramId, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

		GLint NormalTrixLoc = glGetUniformLocation(ProgramId, "NormalMatrix");
		glUniformMatrix4fv(NormalTrixLoc, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, CloudTextureId);

		GLint TextureSamplerLoc = glGetUniformLocation(ProgramId, "TextureSampler");
		glUniform1i(TextureSamplerLoc, 0);

		GLint CloudTextureSamplerLoc = glGetUniformLocation(ProgramId, "CloudTexture");
		glUniform1i(CloudTextureSamplerLoc, 1);

		GLint LightDirectionLoc = glGetUniformLocation(ProgramId, "LightDirection");
		glUniform3fv(LightDirectionLoc, 1, glm::value_ptr(Camera.GetView() * glm::vec4{ Light.Direction, 0 }));
		GLint LightIntensityLoc = glGetUniformLocation(ProgramId, "LightIntensity");
		glUniform1f(LightIntensityLoc, Light.Intensity);

		// glBindVertexArray(QuadVAO);
		glBindVertexArray(SphereVAO);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Informa ao OpenGL desenhar o triângulo com os dados que estão armazenados no VertexBuffer
		// glDrawArrays(GL_TRIANGLES, 0, Quad.size());
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		// glDrawArrays(GL_POINTS, 0, ShepereNumVertices);
		glDrawElements(GL_TRIANGLES, ShepereNumIndices, GL_UNSIGNED_INT, nullptr);

		// glUniformMatrix4fv(ModelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(ModelViewProjection * ModelMatrix2));
		// glUniformMatrix4fv(NormalTrixLoc, 1, GL_FALSE, glm::value_ptr(NormalMatrix2));
		// glDrawElements(GL_TRIANGLES, ShepereNumIndices, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		// Desabilitar o programa ativo
		glUseProgram(0);

		// Processar todos os eventos da fila de eventos do GLFW
		// Podendo ser eventos do teclado, mouse, GamePad
		glfwPollEvents();

		// Envia o conteúdo do frameBuffer da janela para ser desenhado na tela
		// Enviando a memória do backbuffer para o frontbuffer (placa de vídeo > monitor)
		glfwSwapBuffers(Window);

		// Processar os Inputs do Teclado
		if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
			Camera.MoveForward(1 * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
			Camera.MoveForward(-1 * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
			Camera.MoveRight(-1 * DeltaTime);
		}

		if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
			Camera.MoveRight(1 * DeltaTime);
		}
	}

	// Desalocar o VertexBuffer
	glDeleteVertexArrays(1, &SphereVAO);

	// Encerra o GLFW
	glfwTerminate();

	return 0;
}