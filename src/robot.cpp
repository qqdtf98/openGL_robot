//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;

GLuint pvmMatrixID;

float rotAngle = 0.0f;
float armRotAngle = -0.08f;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 1.0, 1.0, 1.0)  // white
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d];  Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	pvmMatrixID = glGetUniformLocation(program, "mPVM");

	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	viewMat = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

void drawRobot(glm::mat4 robotMat) {
	glm::mat4 modelMat, pvmMat;
	glm::vec3 legPos[4];
	glm::vec3 armPos[4];

	legPos[0] = glm::vec3(0, -0.2, -0.1); // robot left upper leg
	legPos[1] = glm::vec3(0, -0.4, -0.1); // robot left lower leg
	legPos[2] = glm::vec3(0, -0.2, 0.1); // robot right lower leg
	legPos[3] = glm::vec3(0, -0.4, 0.1); // robot right lower leg

	armPos[0] = glm::vec3(0, 0.3, -0.3); // robot left upper arm
	armPos[1] = glm::vec3(0, 0.13, -0.45); // robot left lower arm
	armPos[2] = glm::vec3(0, 0.3, 0.3); // robot right upper arm
	armPos[3] = glm::vec3(0, 0.13, 0.45); // robot right lower arm

	// robot body
	modelMat = glm::translate(robotMat, glm::vec3(0, 0.2, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.35, 0.6, 0.4));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// robot head
	modelMat = glm::translate(robotMat, glm::vec3(0, 0.6, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.25, 0.25, 0.2));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// robot legs
	for (int i = 0; i < 4; i++) {
		if (i < 2) {
			modelMat = glm::rotate(robotMat, armRotAngle * 10.0f, glm::vec3(0, 1, -1));
			modelMat = glm::translate(modelMat, legPos[i]);
			modelMat = glm::scale(modelMat, glm::vec3(0.17, 0.2, 0.12));
		}
		else {
			modelMat = glm::rotate(robotMat, -armRotAngle * 10.0f, glm::vec3(0, 1, -1));
			modelMat = glm::translate(modelMat, legPos[i]);
			modelMat = glm::scale(modelMat, glm::vec3(0.17, 0.2, 0.12));
		}
		
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// robot arms
	for (int i = 0; i < 4; i++) {
		if (i < 2) {
			modelMat = glm::rotate(robotMat, -armRotAngle * 10.0f, glm::vec3(0, 1, -1));
			modelMat = glm::translate(modelMat, armPos[i]);
			modelMat = glm::rotate(modelMat, 0.7f, glm::vec3(1.0f, 0, 0));
		}
		else {
			modelMat = glm::rotate(robotMat, -armRotAngle * 10.0f, glm::vec3(0, 1, 1));
			modelMat = glm::translate(modelMat, armPos[i]);
			modelMat = glm::rotate(modelMat, -0.7f, glm::vec3(1.0f, 0, 0));
		}
		
		modelMat = glm::scale(modelMat, glm::vec3(0.14, 0.22, 0.12));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	
}

void display(void) // matrix를 계산해서 넘겨줌
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldMat = glm::rotate(glm::mat4(1.0f), rotAngle, glm::vec3(0, 1.0f, 0)); // 이전 transformation(identitiy matrix), rotation angle, 회전축
	worldMat = glm::mat4(1.0f);

	drawRobot(worldMat);


	glutSwapBuffers();
}

//----------------------------------------------------------------------------
int state = 0;

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	

	if (abs(currTime - prevTime) >= 20)
	{
		float t = abs(currTime - prevTime);
		rotAngle += glm::radians(t * 360.0f / 10000.0f);
		if (state == 0 && armRotAngle > 0.08f) {
			state = 1;
		}
		else if (state == 1 && armRotAngle < -0.088f) {
			state = 0;
		}
		state == 1 ? armRotAngle -= 0.003f : armRotAngle += 0.003f;
		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Car");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
