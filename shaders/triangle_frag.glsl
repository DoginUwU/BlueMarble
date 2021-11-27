#version 330 core

uniform sampler2D TextureSampler;
uniform sampler2D CloudTexture;

uniform float Time;

uniform vec2 CloudsRotationSpeed = vec2(0.01, 0.005);

in vec3 Normal;
in vec3 Color;
in vec2 UV;

uniform vec3 LightDirection;
uniform float LightIntensity;

out vec4 OutColor;

void main() {
	// Renormalizar a normal para evitar problemas com a interpolação linear
	vec3 N = normalize(Normal);

	// Inverter a direção da luz para calcular o vetor L
	vec3 L = -normalize(LightDirection);

	float Lambertian = max(dot(N, L), 0);

	// Vetor V (View)
	vec3 ViewDirection = vec3(0, 0, -1);
	vec3 V = -ViewDirection;

	// Vetor R (Reflection)
	vec3 R = reflect(-L, N);

	// Termo Especular (R . V) ^ Alpha
	float Alpha = 50.0f;
	float Specular = pow(max(dot(R, V), 0), Alpha);
	Specular = max(Specular, 0.0);

	// OutColor = vec4(Color, 1.0);

	vec3 EarthColor = texture(TextureSampler, UV).rgb;
	vec3 CloudColor = texture(CloudTexture, UV + Time * CloudsRotationSpeed).rgb;
	vec3 FinalColor = (EarthColor + CloudColor) * LightIntensity * Lambertian + Specular;
	// FinalColor = vec3(0.0);

	OutColor = vec4(FinalColor, 1.0);
}