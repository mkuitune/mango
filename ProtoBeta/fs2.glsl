#version 330
smooth in Vec4 theColor;
smooth in Vec4 thePosition;
out Vec4 outputColor;

uniform Vec4 uniform_color;

void main()
{
    
    Vec4 origin = Vec4(0,0,0,1.0);
    Vec4 diff = thePosition - origin;

    if(length(diff) < 0.1)
    {
        //outputColor = Vec4(1.0, 1.0, 1.0, 1.0);
        outputColor = uniform_color;
    }
    else
    {
        outputColor = theColor;
    }

    

}
