#version 330
smooth in vec4 theColor;
smooth in vec3 thePosition;
out vec4 outputColor;

uniform vec4 uniform_color;

void main()
{
    
    vec3 origin = vec3(0,0,0);
    vec3 diff = thePosition - origin;

    if(length(diff) < 0.1)
    {
        //outputColor = vec4(1.0, 1.0, 1.0, 1.0);
        outputColor = uniform_color;
    }
    else
    {
        outputColor = theColor;
    }

    

}
