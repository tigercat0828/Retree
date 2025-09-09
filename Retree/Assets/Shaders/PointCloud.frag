#version 460 core
//uniform vec3 uColor;

in vec3 fColor;
out vec4 FragColor;

void main(){
	FragColor = vec4(fColor, 1.0f);
}