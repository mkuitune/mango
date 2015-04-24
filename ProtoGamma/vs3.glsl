#version 330
layout(location = 0) in Vec4 position;
layout(location = 1) in Vec4 color;
smooth out Vec4 theColor;
smooth out Vec4 thePosition;
void main()
{
    gl_Position = position;
    thePosition = position;
    theColor = color;
}
