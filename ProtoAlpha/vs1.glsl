#version 330
layout(location = 0) in Vec4 position;
layout(location = 1) in Vec4 color;
smooth out Vec4 theColor;
void main()
{
	gl_Position = position;
	theColor = color;
}
