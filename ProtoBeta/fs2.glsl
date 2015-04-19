#version 330
smooth in vec4 theColor;
smooth in vec4 thePosition;
out vec4 outputColor;

uniform vec4 uniform_color;

void main()
{
    
    vec4 origin = vec4(0,0,0,1.0);
    vec4 diff = thePosition - origin;

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
