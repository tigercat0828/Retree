#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}

/*
void main() {
	vec4 color = texture2D(texture, uv);
	gl_FragColor = color * vec4(1.0, 1.0 - abs(sin(time)), 1.0, 1.0); // º“¿¿√C¶‚≈‹§∆
}
*/