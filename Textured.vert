
uniform int unif_Mag;
uniform vec2 unif_TextureTiling;
varying vec2 var_TexCoord;
attribute vec3 deformation;
attribute vec3 coloring;
varying vec3 def;

varying vec3 var_Normal;
varying vec3 var_EyeVector;

///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{
vec4 eyeSpacePosition = gl_ModelViewMatrix * gl_Vertex;
var_TexCoord = gl_MultiTexCoord0.xy * unif_TextureTiling;
var_EyeVector = -eyeSpacePosition;
var_Normal = gl_NormalMatrix * gl_Normal;
vec4 newVertexPos;
newVertexPos = vec4(deformation.x*unif_Mag,deformation.y*unif_Mag,deformation.z*unif_Mag,1) + gl_Vertex;
gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
gl_FrontColor = gl_Color;
def = coloring;
}