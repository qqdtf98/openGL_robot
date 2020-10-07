#version 150

in  vec4 vPosition; // in�� vertex����Ʈ�� ������Ʈ�� ������ͼ� �ϳ��� ó�����ִ°�. vertex���� ��� �ٲ�.
in  vec4 vColor;
out vec4 color;

uniform mat4 mPVM; // matrix�� uniform variable ���·� �޾ƿ�. projection, view, model transformation�� �� ���� 4*4 matrix
// ��� vertex�� ���ؼ� ������ �ϳ��� variable

void main() 
{
  gl_Position = mPVM * vPosition;  // transform�� position ����
  color = vColor;
} 
