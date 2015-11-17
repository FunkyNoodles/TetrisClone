#version 330 core
in vec3 ourColor;

out vec4 color;
//uniform vec4 ourColor;
void main()
{
    color = vec4(ourColor, 1.0f);
	//color = ourColor;
}