#include "ShaderLoader.h"

#include "FreeImage.h"
#include "mesh.h"
#include "light.h"
#include "viewing.h"
#include "scene.h"

#include "glew.h"
#include "glut.h"
#include <math.h>
#include <iostream>
using namespace std;

mesh **object;
light *light_data;
viewing *view_data;
scene *scene_data;
unsigned int *texObject;

GLhandleARB MyShader;
GLhandleARB Hair;

int check = 0;
int use = 0;
int cnt = 0;
int windowSize[2];
float zoom = 0.0f;
float zoom_change = 3.0f;
float rot = 0.0f;
float rotate_change = 3.0f;
int select = -1;
float x_change;
float y_change;
int program;
float hair_length = 0.5f;
float length_change = 0.1f;
int seg = 15;
float grav = -0.1f;

void LoadCube(int num);
void LoadTexture(char* pFilename);
void lighting();
void display();
void reshape(GLsizei , GLsizei );
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void LoadShaders();
void setShaders();
char *textFileRead(char *fn);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

int main(int argc, char** argv)
{
	light_data = new light("Peter.light", 2);
	view_data = new viewing("Peter.view");
	scene_data = new scene("Peter.scene", 2);
	object = new mesh*[scene_data->nobj];
	for (int i = 0; i < scene_data->nobj; i++)
	{
		object[i] = new mesh(scene_data->obj_name[i]);
	}
	texObject = new unsigned int[scene_data->texture_num];

	glutInit(&argc, argv);
	glutInitWindowSize(view_data->viewport[2], view_data->viewport[3]);
	glutInitWindowPosition(view_data->viewport[0], view_data->viewport[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Peter");
	
	glewInit();
	FreeImage_Initialise();
	glGenTextures(scene_data->texture_num, texObject);
	for (int i = 0, object = 0; i < scene_data->texture_num; i++)
	{
		cout << i << "  " << object << "  " << endl;
		string ef(scene_data->effect[object]);
		string compare = "";
		if (object + 1 != scene_data->nobj) compare.assign(scene_data->effect[object + 1]);
		use = scene_data->texture_use[object];
		check = scene_data->texture_use[object + 1];
		if (check == use && ef == compare && ef != "no")
		{
			cout << "!!" << endl;
			i--;
			object++;
		}
		else if (ef == "no")
		{
			while (1)
			{
				object++;
				string compare(scene_data->effect[object]);
				if (compare != "no")
				{
					i--;
					break;
				}
			}
		}
		else if (ef == "cube")
		{
			cout << "QQ" << endl;
			LoadCube(i);
			i += 5;
			object++;
		}
		else if (ef == "multi")
		{
			LoadTexture(scene_data->texture_name[i]);
			i++;
			LoadTexture(scene_data->texture_name[i]);
			object++;
		}
		else
		{
			LoadTexture(scene_data->texture_name[i]);
			object++;
		}
	}
	FreeImage_DeInitialise();

	LoadShaders();
	setShaders();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();

	delete texObject;
	for (int i = scene_data->nobj - 1; i >= 0; i--)
	{
		delete[] object[i];
	}
	delete[] object;
	delete scene_data;
	delete view_data;
	delete light_data;

	return 0;
}

void LoadCube(int num)
{
	FIBITMAP* pImage0 = FreeImage_Load(FreeImage_GetFileType(scene_data->texture_name[num], 0), scene_data->texture_name[num]);
	FIBITMAP* pImage1 = FreeImage_Load(FreeImage_GetFileType(scene_data->texture_name[num+1], 0), scene_data->texture_name[num+1]);
	FIBITMAP* pImage2 = FreeImage_Load(FreeImage_GetFileType(scene_data->texture_name[num+2], 0), scene_data->texture_name[num+2]);
	FIBITMAP* pImage3 = FreeImage_Load(FreeImage_GetFileType(scene_data->texture_name[num+3], 0), scene_data->texture_name[num+3]);
	FIBITMAP* pImage4 = FreeImage_Load(FreeImage_GetFileType(scene_data->texture_name[num+4], 0), scene_data->texture_name[num+4]);
	FIBITMAP* pImage5 = FreeImage_Load(FreeImage_GetFileType(scene_data->texture_name[num+5], 0), scene_data->texture_name[num+5]);
	FIBITMAP *p32BitsImage0 = FreeImage_ConvertTo32Bits(pImage0);
	FIBITMAP *p32BitsImage1 = FreeImage_ConvertTo32Bits(pImage1);
	FIBITMAP *p32BitsImage2 = FreeImage_ConvertTo32Bits(pImage2);
	FIBITMAP *p32BitsImage3 = FreeImage_ConvertTo32Bits(pImage3);
	FIBITMAP *p32BitsImage4 = FreeImage_ConvertTo32Bits(pImage4);
	FIBITMAP *p32BitsImage5 = FreeImage_ConvertTo32Bits(pImage5);
	int iWidth0 = FreeImage_GetWidth(p32BitsImage0);
	int iHeight0 = FreeImage_GetHeight(p32BitsImage0);
	int iWidth1 = FreeImage_GetWidth(p32BitsImage1);
	int iHeight1 = FreeImage_GetHeight(p32BitsImage1);
	int iWidth2 = FreeImage_GetWidth(p32BitsImage2);
	int iHeight2 = FreeImage_GetHeight(p32BitsImage2);
	int iWidth3 = FreeImage_GetWidth(p32BitsImage3);
	int iHeight3 = FreeImage_GetHeight(p32BitsImage3);
	int iWidth4 = FreeImage_GetWidth(p32BitsImage4);
	int iHeight4 = FreeImage_GetHeight(p32BitsImage4);
	int iWidth5 = FreeImage_GetWidth(p32BitsImage5);
	int iHeight5 = FreeImage_GetHeight(p32BitsImage5);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[cnt]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB,
		iWidth0, iHeight0, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage0));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB,
		iWidth1, iHeight1, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage1));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB,
		iWidth2, iHeight2, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage2));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB,
		iWidth3, iHeight3, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage3));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB,
		iWidth4, iHeight4, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage4));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB,
		iWidth5, iHeight5, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage5));
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	float fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(p32BitsImage0);
	FreeImage_Unload(pImage0);
	FreeImage_Unload(p32BitsImage1);
	FreeImage_Unload(pImage1);
	FreeImage_Unload(p32BitsImage2);
	FreeImage_Unload(pImage2);
	FreeImage_Unload(p32BitsImage3);
	FreeImage_Unload(pImage3);
	FreeImage_Unload(p32BitsImage4);
	FreeImage_Unload(pImage4);
	FreeImage_Unload(p32BitsImage5);
	FreeImage_Unload(pImage5);
	cnt++;
}

void LoadTexture(char* pFilename)
{
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, texObject[cnt]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	float fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
	cnt++;
}

void lighting()
{
	cout << "lighting" << endl;
	GLfloat light_specular[2][4];
	GLfloat light_diffuse[2][4];
	GLfloat light_ambient[2][4];
	GLfloat light_position[2][4];

	int i, j;
	for (i = 0; i < light_data->nlight; i++)
	{
		for (j = 0; j < 3; j++)
		{
			light_specular[i][j] = light_data->s[i][j];
			light_diffuse[i][j] = light_data->d[i][j];
			light_ambient[i][j] = light_data->a[i][j];
			light_position[i][j] = light_data->pos[i][j];
		}

		light_specular[i][j] = 1.0f;
		light_diffuse[i][j] = 1.0f;
		light_ambient[i][j] = 1.0f;
		light_position[i][j] = 1.0f;
	}

	GLfloat ambient[3];
	for (i = 0; i < 3; i++)
	{
		ambient[i] = light_data->amb[i];
	}

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	int lightN = GL_LIGHT0;
	for (int i = 0; i < light_data->nlight; i++) 
	{
		glEnable(lightN + i);
		glLightfv(lightN + i, GL_POSITION, light_position[i]);
		glLightfv(lightN + i, GL_DIFFUSE, light_diffuse[i]);
		glLightfv(lightN + i, GL_SPECULAR, light_specular[i]);
		glLightfv(lightN + i, GL_AMBIENT, light_ambient[i]);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

void display()
{
	cout << "display" << endl;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //clear the buffer
	glClearDepth(1.0f);                        //Z buffer
	glEnable(GL_DEPTH_TEST);                  
	glDepthFunc(GL_LEQUAL);  
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear color and z buffer

	glViewport(view_data->viewport[0], view_data->viewport[1], windowSize[0], windowSize[1]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view_data->theta, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], view_data->dnear_d, view_data->dfar_d);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	view_data->eye[0], view_data->eye[1], view_data->eye[2],	// eye
				view_data->vat[0], view_data->vat[1], view_data->vat[2],	// center
				view_data->vup[0], view_data->vup[1], view_data->vup[2]);   // up
	
	float X = view_data->eye[0] * view_data->eye[0];
	float Y = view_data->eye[1] * view_data->eye[1];
	float Z = view_data->eye[2] * view_data->eye[2];
	float Q = X + Y + Z; 
	X = X / Q; Y = Y / Q; Z = Z / Q;
	glTranslatef(X*zoom, Y*zoom, Z*zoom);
	glRotatef(rot, view_data->vat[0] + view_data->vup[0], view_data->vat[1] + view_data->vup[1], view_data->vat[2] + view_data->vup[2]);
	
	
	lighting();

	cnt = 0;
	mesh *cur;
	int lastMaterial = -1;
	for (int i = 0; i < scene_data->nobj; i++)
	{
		cur = object[i];
		lastMaterial = -1;
		glPushMatrix();
		glTranslatef(scene_data->t[i][0], scene_data->t[i][1], scene_data->t[i][2]);
		glRotatef(scene_data->angle[i], scene_data->r[i][0], scene_data->r[i][1], scene_data->r[i][2]);
		glScalef(scene_data->s[i][0], scene_data->s[i][1], scene_data->s[i][2]);
		if (i == 0)
		{
			glUseProgram(MyShader);
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(MyShader, "lightNumber"), light_data->nlight);
			GLint location = glGetUniformLocation(MyShader, "colorTexture");
			if (location == -1)
				printf("Cant find texture name: colorTexture\n");
			else
				glUniform1i(location, 0);
		}
		else if (i == 1)
		{
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(Hair);
			glActiveTexture(GL_TEXTURE0);
			glUniform1f(glGetUniformLocation(Hair, "hairLength"), hair_length);
			glUniform1i(glGetUniformLocation(Hair, "segment"), seg);
			glUniform3f(glGetUniformLocation(Hair, "gravity"), 0.0f, grav, 0.0f);
			glDepthMask(GL_TRUE);
		}
			
		//bind texture
		string effect(scene_data->effect[i]);		
		if (effect == "no");
		else if (effect == "single"){
			cout << scene_data->obj_name[i] << "   " << effect << "    " << scene_data->texture_name[scene_data->texture_use[i]] << "   " << cnt << endl;
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[cnt]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (scene_data->texture_use[i] == scene_data->texture_use[i + 1]) cnt = cnt;
			else cnt++;
		}
		else if (effect == "multi")
		{
			for (int j = 0; j < 2; j++)
			{
				cout << scene_data->obj_name[i] << "   " << effect << "    "  << scene_data->texture_name[scene_data->texture_use[i] + j] << "   " << cnt<<endl;
				glActiveTexture(GL_TEXTURE0 + j);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texObject[cnt]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				cnt++;
			}
			if (scene_data->texture_use[i] == scene_data->texture_use[i + 1]) cnt -= 2 ;
		}
		else if (effect == "cube")
		{
			cout << scene_data->obj_name[i] << "   " << effect << "    " << scene_data->texture_name[scene_data->texture_use[i]] << "   " << cnt << endl;
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[cnt]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			if (scene_data->texture_use[i] == scene_data->texture_use[i + 1]) cnt = cnt;
			else cnt++;
		}

		//draw object
		for (size_t s = 0; s < cur->fTotal; ++s)
		{
			if (lastMaterial != cur->faceList[s].m)	//different material
			{
				lastMaterial = (int)cur->faceList[s].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, cur->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, cur->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, cur->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &cur->mList[lastMaterial].Ns);
			}

			
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j < 3; ++j)
			{
				//textex corrd. object->tList[object->faceList[i][j].t].ptr
				if (effect == "no");
				else if (effect == "single")
				{
					glTexCoord2fv(cur->tList[cur->faceList[s][j].t].ptr);
				}
				else if (effect == "multi")
				{
					glMultiTexCoord2fv(GL_TEXTURE0, cur->tList[cur->faceList[s][j].t].ptr);
					glMultiTexCoord2fv(GL_TEXTURE1, cur->tList[cur->faceList[s][j].t].ptr);
				}
				else if (effect == "cube");

				glNormal3fv(cur->nList[cur->faceList[s][j].n].ptr);
				glVertex3fv(cur->vList[cur->faceList[s][j].v].ptr);
			}
			glEnd();
		}

		//disable texture
		if (effect == "no");
		else if (effect == "single")
		{
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (effect == "multi")
		{
			for (int j = 1; j >= 0; j--)
			{
				glActiveTexture(GL_TEXTURE0 + j);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
		else if (effect == "cube")
		{
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		glPopMatrix();
	}
	glUseProgram(0);
	glutSwapBuffers();
}

bool ShaderLoad(GLhandleARB programId, char* shaderSrc, GLenum shaderType)
{
	FILE *fp;
	GLhandleARB h_shader;
	GLcharARB *shader_string;
	GLint str_length, maxLength;
	GLint isCompiled = GL_FALSE, isLinked = GL_FALSE;
	GLcharARB *pInfoLog;

	// open the file of shader source code
	if ((fp = fopen(shaderSrc, "r")) == NULL)
	{
		fprintf(stderr, "Error : Failed to read the OpenGL shader source \"%s\".\n", shaderSrc);
		return false;
	}

	// allocate memory for program string and load it.
	shader_string = (GLcharARB*)malloc(sizeof(GLcharARB)* 65536);
	str_length = (GLint)fread(shader_string, 1, 65536, fp);
	fclose(fp);

	// Create and load shader string.
	h_shader = glCreateShader(shaderType);
	if (h_shader == 0)
	{
		fprintf(stderr, "Error : Failed to create OpenGL shader object \"%s\".\n", shaderSrc);
		return false;
	}
	glShaderSource(h_shader, 1, (const GLcharARB**)&shader_string, &str_length);
	free(shader_string);

	// Compile the vertex shader, print out the compiler log message.
	glCompileShader(h_shader);

	// get compile state information
	glGetObjectParameterivARB(h_shader, GL_OBJECT_COMPILE_STATUS_ARB, &isCompiled);

	if (!isCompiled)
	{
		fprintf(stderr, "Error : Failed to compile OpenGL shader source \"%s\".\n", shaderSrc);
		glGetObjectParameterivARB(h_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
		glGetInfoLogARB(h_shader, maxLength, &str_length, pInfoLog);
		fprintf(stderr, "%s\n", pInfoLog);
		free(pInfoLog);
		return false;
	}
	glAttachShader(programId, h_shader);

	// delete the shader object, since we have attached it with the program object.
	glDeleteShader(h_shader);

	// Link the program and print out the linker log message
	glLinkProgram(programId);
	glGetObjectParameterivARB(programId, GL_OBJECT_LINK_STATUS_ARB, &isLinked);

	if (!isLinked)
	{
		fprintf(stderr, "Error : Failed to link OpenGL shader \"%s\".\n", shaderSrc);
		glGetObjectParameterivARB(programId, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
		glGetInfoLogARB(programId, maxLength, &str_length, pInfoLog);
		fprintf(stderr, "%s\n", pInfoLog);
		free(pInfoLog);
		return false;
	}

	return true;
}

void reshape(GLsizei w, GLsizei h)
{
	cout << "reshape" << endl;
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y){
	switch (key)
	{
	case 'w':    
		zoom += zoom_change;
		break;
	case 's':    
		zoom -= zoom_change;
		break;
	case 'a':    
		rot += rotate_change;
		break;
	case 'd':    
		rot -= rotate_change;
		break;
	case 'r':
		hair_length += length_change;
		break;
	case 'f':
		if ((hair_length-length_change) > 0) hair_length -= length_change;
		break;
	case 't':
		seg += 1;
		break;
	case 'g':
		if ((seg-1) >= 0)seg -= 1;
		break;
	case 'y':
		grav += 0.1f;
		break;
	case 'h':
		grav -= 0.1f;
		break;
	case 27:	//Esc   
		exit(0);
		break;
	}
	if (key >= '1' && key <= '9'){
		int n = key - '1';
		if (n < scene_data->nobj){
			select = n;
		}
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
	int startX, startY;
	if (state == GLUT_DOWN){
		startX = x;
		startY = y;
		if (select >= 0){
			x_change = x - scene_data->t[select][0];
			y_change = y + scene_data->t[select][1];
		}
	}
	else if (state == GLUT_UP){
		//printf("the mouse moves %d %d \n", x - startX, y - startY);
	}
}

void motion(int x, int y){
	if (select >= 0) {
		scene_data->t[select][0] = 0.1*(x - x_change);
		scene_data->t[select][1] = -0.1*(y - y_change);	
		glutPostRedisplay();
	}
}

void LoadShaders()
{
	MyShader = glCreateProgram();
	Hair = glCreateProgram();
	if (MyShader != 0)
	{
		ShaderLoad(MyShader, "PhongShading.vert", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "PhongShading.frag", GL_FRAGMENT_SHADER);
	}
	if (Hair != 0)
	{
		ShaderLoad(Hair, "VertexNormalVisualizer.vert", GL_VERTEX_SHADER);
		ShaderLoad(Hair, "VertexNormalVisualizer.frag", GL_FRAGMENT_SHADER);
		ShaderLoad(Hair, "VertexNormalVisualizer.geom", GL_GEOMETRY_SHADER);
	}
}

char *textFileRead(char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {

		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char)* (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);

		}
	}

	return content;
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printShaderInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else{
		printf("Shader Info Log: OK\n");
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printProgramInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else{
		printf("Program Info Log: OK\n");
	}
}

void setShaders()
{
	int vertShader, geomShader, fragShader;
	char *vertSource = NULL, *geomSource = NULL, *fragSource = NULL;

	//First, create our shaders 
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Read in the programs
	vertSource = textFileRead("VertexNormalVisualizer.vert");
	geomSource = textFileRead("VertexNormalVisualizer.geom");
	fragSource = textFileRead("VertexNormalVisualizer.frag");

	//Setup a few constant pointers for below
	const char *vv = vertSource;
	const char *gg = geomSource;
	const char *ff = fragSource;

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(geomShader, 1, &gg, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);

	free(vertSource);
	free(geomSource);
	free(fragSource);

	glCompileShader(vertShader);
	glCompileShader(geomShader);
	glCompileShader(fragShader);

	program = glCreateProgram();

	glAttachShader(program, vertShader);
	glAttachShader(program, geomShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);

	//You can use glUseProgram(program) whenever you want to render something with the program.
	//If you want to render with the fixed pipeline, use glUseProgram(0).

	printShaderInfoLog(vertShader);
	printShaderInfoLog(geomShader);
	printShaderInfoLog(fragShader);
	printProgramInfoLog(program);
}
