#version 150

in  vec4 vPosition; // in은 vertex리스트의 엘리먼트를 가지고와서 하나씩 처리해주는것. vertex마다 계속 바뀜.
in  vec4 vColor;
out vec4 color;

uniform mat4 mPVM; // matrix를 uniform variable 형태로 받아옴. projection, view, model transformation을 다 곱한 4*4 matrix
// 모든 vertex에 대해서 동일한 하나의 variable

void main() 
{
  gl_Position = mPVM * vPosition;  // transform된 position 구함
  color = vColor;
} 
