//  ========================================================================
//  COSC363: Computer Graphics (2020).  University of Canterbury.
//
//  FILE NAME: TeapotPatches.cpp
//
//	See Lab10.pdf for details
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

GLuint vaoID;
GLuint matrixLoc;

const float CDR = 3.14159265 / 180.0; 
int level = 8;		//Tessellation level
float* verts;
int nverts;			//Number of patch vertices
float angle = 0;	//Rotation angle
glm::mat4 projnMatrix;
float camDist = 20;	//Camera distance

// ----------- Load patch vertices -------------------------
void loadData()
{
	ifstream infile("PatchVerts_Teapot.txt");
	if (!infile) {
		cout << "There was a problem opening data file " << endl;
		exit(1);
	}

	infile >> nverts;
	verts = new float[nverts * 3];

	for (int i = 0; i < nverts; i++)
		infile >> verts[3 * i] >> verts[3 * i + 1] >> verts[3 * i + 2];

	infile.close();
}

// ----------- Load shaders -------------------------
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}


void initialise()
{
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "TeapotPatch.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "TeapotPatch.frag");
	GLuint shadert = loadShader(GL_TESS_EVALUATION_SHADER, "TeapotPatch.eval");

	GLint status;

	GLuint programBez = glCreateProgram();
	glAttachShader(programBez, shaderv);
	glAttachShader(programBez, shaderf);
	glAttachShader(programBez, shadert);
	glLinkProgram(programBez);

	glGetProgramiv (programBez, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(programBez, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(programBez, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(programBez);

	matrixLoc = glGetUniformLocation(programBez, "mvpMatrix");

	loadData();

	GLuint vboID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);   //Bezier surface
    glGenBuffers(1, &vboID);

	// Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, 3 * nverts * sizeof(float), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); 

    glBindVertexArray(0);
	glPatchParameteri(GL_PATCH_VERTICES, 16);   //Number of vertices in each patch

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//perspective projection matrix
	projnMatrix = glm::perspective(40.0f*CDR, 1.0f, 5.0f, 100.0f);
}

void display()
{
	float outer[4] = { level, level, level, level };	//Outer tessellation levels
	float inner[2] = { level, level };					//Inner tessellation levels
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outer);
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, inner);
	
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 2, camDist),    //view matrix
								glm::vec3(0.0, 0.0, camDist-10), 
								glm::vec3(0.0, 1.0, 0.0)); 
	//identity matrix
	glm::mat4 matrix = glm::mat4(1.0);
	glm::mat4 rotnMatrix = glm::rotate(matrix, angle*CDR, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 mvpMatrix = projnMatrix * viewMatrix * rotnMatrix;	        //model view projection matrix

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

	glBindVertexArray(vaoID);
	glDrawArrays(GL_PATCHES, 0, nverts);

	if (camDist == 50) {
		level = 3;
	}
	else if (camDist < 40 && camDist > 30) {
		level = 5;
	}
	else if (camDist <= 30 && camDist > 20) {
		level = 7;
	}
	else if (camDist <= 20 && camDist > 10) {
		level = 9;
	}
	else if (camDist <= 10) {
		level = 13;
	}

	glFlush();
}

void update(int value)
{
	angle++;
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}


void special(int key, int x, int y) 
{
	if (key == GLUT_KEY_UP) camDist--;
	else if (key == GLUT_KEY_DOWN) camDist++;
	if (camDist < 10) camDist = 10;
	else if (camDist > 50) camDist = 50;
	glutPostRedisplay();

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Bezier Teapot");
	glutInitContextVersion (4, 1);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutTimerFunc(50,update, 0);
	glutMainLoop();
}

