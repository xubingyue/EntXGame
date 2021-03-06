#version 420 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNorm;

out vec3 FragPos;
out vec3 FragNorm;
out vec2 FragUV;

uniform mat4 camView;
uniform mat4 model;

void main() {
   gl_Position = camView * model * vec4(vertexPos, 1.0f);

   //Accounts for any non-uniform stretching in model
   FragNorm = normalize(mat3(transpose(inverse(model))) * vertexNorm);
   FragPos = vec3(model * vec4(vertexPos, 1.0f));
   FragUV = vertexUV;
}
