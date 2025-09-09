#version 460 core

in vec2 fTexCoord;
in vec3 fNormal;
out vec4 FragColor;

uniform sampler2D texture1;

void main(){
	
	// FragColor = texture(texture1, fTexCoord);

    vec4 texColor = texture(texture1, fTexCoord);
    FragColor = texColor; // includes alpha
}
