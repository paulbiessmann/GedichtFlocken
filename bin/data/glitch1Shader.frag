#version 150

//Shader for visualizing a depth texture as a focal distance map
//paired with dofblur for debugging purposes



in vec4 vColor;
in vec2 vTexCoord;

out vec4 vFragColor;


void main()
{
    vFragColor = vec4( 0.5, vColor.g, vColor.b, vColor.a  );
     //   vFragColor = vColor;
}
