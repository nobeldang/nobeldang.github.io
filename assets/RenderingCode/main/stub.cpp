/******************************************************************************|
| CPSC 4050/6050 Computer Garphics Assignment 5, Daljit Singh Dhillon, 2020    |
| Reference:                                                                   |
|                                                                              |
| Some OpenGL setup code here including math_funcs, and gl_utils               |
| are from Angton Gerdelan and "Anton's OpenGL 4 Tutorials."                   |
| http://antongerdelan.net/opengl/                                             |
| Email: anton at antongerdelan dot net                                        |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
|******************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>


#include <math.h>
#include <cmath>
#include <time.h>

#include "maths_funcs.h"   // Anton's maths functions.
#include "gl_utils.h"      // Anton's opengl functions and small utilities like logs
#include "stb_image.h"     // Sean Barrett's image loader with Anton's load_texture()

#define _USE_MATH_DEFINES
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

mat4 view_mat;
mat4 proj_mat;
mat4 model_mat;
float alpha;
vec3 delta_position;
int y_steps;
int radial_steps;
int pointCount;
bool diffuse_flag, specular_flag, albedo_flag;

void loadSurfaceOfRevolution()
{
	// Get the inputs
	printf("\n Enter number of steps (integer) for y-dim: ");
	scanf("%d", &y_steps);
	printf("\n Enter number of steps (integer) for radial-dim: ");
	scanf("%d", &radial_steps);

	printf("\n Entered vals are: y_steps = %d, and radial_steps = %d.", y_steps, radial_steps);
  // -----------------------------------------------------------------------------------------

/*------------------------------CREATE GEOMETRY-------------------------------*/
	GLfloat vp[y_steps*radial_steps*6*3];    // array of vertex points
	GLfloat vn[y_steps*radial_steps*6*3];		// array for vertex normals
	GLfloat vt[y_steps*radial_steps*6*2];		// array for texture coordinates

	// mat4 basis;
	// vec4 xs;
	// vec4 ys;
	// basis = mat4(1.0, -3.0, 3.0, -1.0,
	// 								0.0, 3.0, -6.0, 3.0,
	// 								0.0, 0.0, 3.0, -3.0,
	// 								0.0, 0.0, 0.0, 1.0);

	// vec2 ctrPt1 = vec2(0.2, 0.6);
	// vec2 ctrPt2 = vec2(0.6, 0.2);
	// vec2 ctrPt3 = vec2(0.6, -0.2);
	// vec2 ctrPt4 = vec2(0.2, -0.6);

	float ctrl_xs[4] = {0.2, 0.6, 0.6, 0.2};
	float ctrl_ys[4] = {-0.6, -0.2, 0.2, 0.6};

	float dt = 1.0/(float(y_steps));
	printf("dt: %f", dt);
	float dr_deg = 360/radial_steps;
	double dr = (dr_deg * M_PI) / 180.0;

	float t = 1.0;
	float r = 0.0;
	float max_rt;

	float xt;
	float yt;
	float zt;

	float p_mat[y_steps + 1][radial_steps + 1][3];
	for (int i = 0; i <= y_steps; i ++){
		max_rt = (pow((1-t), 3)*ctrl_xs[0]) + (3*t*pow((1-t), 2)*ctrl_xs[1]) + (3*pow(t, 2)*(1-t)*ctrl_xs[2]) + (pow(t, 3)*ctrl_xs[3]);
		yt = pow((1-t), 3)*ctrl_ys[0] + 3*t*pow((1-t), 2)*ctrl_ys[1] + 3*pow(t, 2)*(1-t)*ctrl_ys[2] + pow(t, 3)*ctrl_ys[3];
		// printf("\nAt t = %f", t);
		for (int j =0; j<=radial_steps; j++){
			xt = max_rt*cos(r);
			zt = max_rt*sin(r); // anticlockwise order
			r += dr;
			// printf("\n i, j: %d, %d", i, j);
			// printf("\n yt = %f, xt = %f and zt = %f.", yt, xt, zt);
			p_mat[i][j][0] = xt;
			p_mat[i][j][1] = yt;
			p_mat[i][j][2] = zt;
		}

		t -= dt; // top to bottom
		r = 0.0;
	}

	int float_index = 0;
	int tex_index = 0;

	for (int i = 0; i <y_steps; i++){
		for (int j = 0; j<radial_steps; j++){
			vec3 vtx_1 = vec3(p_mat[i][j][0], p_mat[i][j][1], p_mat[i][j][2]);
			vec3 vtx_2 = vec3(p_mat[i][j+1][0], p_mat[i][j+1][1], p_mat[i][j+1][2]);
			vec3 vtx_3 = vec3(p_mat[i+1][j+1][0], p_mat[i+1][j+1][1], p_mat[i+1][j+1][2]);
			vec3 vtx_4 = vec3(p_mat[i][j][0], p_mat[i][j][1], p_mat[i][j][2]);
			vec3 vtx_5 = vec3(p_mat[i+1][j+1][0], p_mat[i+1][j+1][1], p_mat[i+1][j+1][2]);
			vec3 vtx_6 = vec3(p_mat[i+1][j][0], p_mat[i+1][j][1], p_mat[i+1][j][2]);

			vec3 vec12 = vtx_1 - vtx_2;
			vec3 vec13 = vtx_1 - vtx_3;
			vec3 vec23 = vtx_2 - vtx_3;
			vec3 vec21 = vtx_2 - vtx_1;
			vec3 vec31 = vtx_3 - vtx_1;
			vec3 vec32 = vtx_3 - vtx_2;

			vec3 vec45 = vtx_4 - vtx_5;
			vec3 vec46 = vtx_4 - vtx_6;
			vec3 vec56 = vtx_5 - vtx_6;
			vec3 vec54 = vtx_5 - vtx_4;
			vec3 vec64 = vtx_6 - vtx_4;
			vec3 vec65 = vtx_6 - vtx_5;

			vec3 vtx_normal_1 = normalise(cross(vec12, vec13));
			vec3 vtx_normal_2 = normalise(cross(vec23, vec21));
			vec3 vtx_normal_3 = normalise(cross(vec31, vec32));
			vec3 vtx_normal_4 = normalise(cross(vec45, vec46));
			vec3 vtx_normal_5 = normalise(cross(vec56, vec54));
			vec3 vtx_normal_6 = normalise(cross(vec64, vec65));

			// texture coordinates
			vt[tex_index] = (float)j / (float)radial_steps;
			tex_index += 1;
			vt[tex_index] = 1.0f - ((float)i / (float)y_steps);
			tex_index += 1;

			vt[tex_index] = (float)(j+1) / (float)radial_steps;
			tex_index += 1;
			vt[tex_index] = 1.0f - ((float)i / (float)y_steps);
			tex_index += 1;

			vt[tex_index] = (float)(j+1) / (float)radial_steps;
			tex_index += 1;
			vt[tex_index] = 1.0f - ((float)(i+1) / (float)y_steps);
			tex_index += 1;

			vt[tex_index] = (float)(j) / (float)radial_steps;
			tex_index += 1;
			vt[tex_index] = 1.0f - ((float)(i) / (float)y_steps);
			tex_index += 1;

			vt[tex_index] = (float)(j+1) / (float)radial_steps;
			tex_index += 1;
			vt[tex_index] = 1.0f - ((float)(i+1) / (float)y_steps);
			tex_index += 1;

			vt[tex_index] = (float)(j) / (float)radial_steps;
			tex_index += 1;
			vt[tex_index] = 1.0f - ((float)(i+1) / (float)y_steps);
			tex_index += 1;

			// vtx position and normal coordinates
			vp[float_index] = vtx_1.v[0];
			vn[float_index] = vtx_normal_1.v[0];
			float_index += 1;
			vp[float_index] = vtx_1.v[1];
			vn[float_index] = vtx_normal_1.v[1];
			float_index += 1;
			vp[float_index] = vtx_1.v[2];
			vn[float_index] = vtx_normal_1.v[2];
			float_index += 1;

			vp[float_index] = vtx_2.v[0];
			vn[float_index] = vtx_normal_2.v[0];
			float_index += 1;
			vp[float_index] = vtx_2.v[1];
			vn[float_index] = vtx_normal_2.v[1];
			float_index += 1;
			vp[float_index] = vtx_2.v[2];
			vn[float_index] = vtx_normal_2.v[2];
			float_index += 1;

			vp[float_index] = vtx_3.v[0];
			vn[float_index] = vtx_normal_3.v[0];
			float_index += 1;
			vp[float_index] = vtx_3.v[1];
			vn[float_index] = vtx_normal_3.v[1];
			float_index += 1;
			vp[float_index] = vtx_3.v[2];
			vn[float_index] = vtx_normal_3.v[2];
			float_index += 1;

			vp[float_index] = vtx_4.v[0];
			vn[float_index] = vtx_normal_4.v[0];
			float_index += 1;
			vp[float_index] = vtx_4.v[1];
			vn[float_index] = vtx_normal_4.v[1];
			float_index += 1;
			vp[float_index] = vtx_4.v[2];
			vn[float_index] = vtx_normal_4.v[2];
			float_index += 1;

			vp[float_index] = vtx_5.v[0];
			vn[float_index] = vtx_normal_5.v[0];
			float_index += 1;
			vp[float_index] = vtx_5.v[1];
			vn[float_index] = vtx_normal_5.v[1];
			float_index += 1;
			vp[float_index] = vtx_5.v[2];
			vn[float_index] = vtx_normal_5.v[2];
			float_index += 1;

			vp[float_index] = vtx_6.v[0];
			vn[float_index] = vtx_normal_6.v[0];
			float_index += 1;
			vp[float_index] = vtx_6.v[1];
			vn[float_index] = vtx_normal_6.v[1];
			float_index += 1;
			vp[float_index] = vtx_6.v[2];
			vn[float_index] = vtx_normal_6.v[2];
			float_index += 1;

		}
	}


	// VAO -- vertex attribute objects bundle the various things associated with vertices
	GLuint vao;
	glGenVertexArrays (1, &vao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (vao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates
	// MODIFY THE FOLLOWING BLOCK OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, y_steps*radial_steps*6*3 * sizeof (GLfloat), vp, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// VBO -- normals -- needed for shading calcuations
	// ADD CODE TO POPULATE AND LOAD PER-VERTEX SURFACE NORMALS
	// [HINT] Vertex normals are organized in same order as that for vertex coordinates
	GLuint points_vno;
	glGenBuffers(1, &points_vno);
	glBindBuffer(GL_ARRAY_BUFFER, points_vno);
	glBufferData(GL_ARRAY_BUFFER, y_steps*radial_steps*6*3 * sizeof (GLfloat), vp, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

    // VBO -- vt -- texture coordinates
	// ADD CODE TO POPULATE AND LOAD PER-VERTEX TEXTURE COORDINATES
	// [HINT] texture coordinates are organized in same order as that for vertex coordinates
	// [HINT] there are two texture coordinates instead of three vertex coordinates for each vertex
	GLuint texture_coords_vbo;
	glGenBuffers(1, &texture_coords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texture_coords_vbo);
	glBufferData(GL_ARRAY_BUFFER, y_steps*radial_steps*6*2 * sizeof(GLfloat), vt, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
}


void loadUniforms(GLuint shader_programme)
{
/*---------------------------SET RENDERING DEFAULTS---------------------------*/

	// Choose vertex and fragment shaders to use as well as view and proj matrices.
	int model_mat_location = glGetUniformLocation (shader_programme, "model_mat");
	int view_mat_location  = glGetUniformLocation (shader_programme, "view_mat");
	int proj_mat_location  = glGetUniformLocation (shader_programme, "proj_mat");
	int alpha_location  = glGetUniformLocation (shader_programme, "alpha");
	int delta_position_location  = glGetUniformLocation (shader_programme, "delta_position");
	int diffuse_flag_location  = glGetUniformLocation (shader_programme, "diffuse_flag");
	int specular_flag_location  = glGetUniformLocation (shader_programme, "specular_flag");
	int albedo_flag_location  = glGetUniformLocation (shader_programme, "albedo_flag");

	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m); // location, count, transpose_flag, values
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat.m);
	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model_mat.m);
	glUniform1f (alpha_location, alpha); // location, value
	glUniform1f (diffuse_flag_location, diffuse_flag); // location, value
	glUniform1f (specular_flag_location, specular_flag); // location, value
	glUniform1f (albedo_flag_location, albedo_flag); // location, value
	glUniform3fv (delta_position_location, 1, delta_position.v); // location, count, values


	// WRITE CODE TO LOAD OTHER UNIFORM VARIABLES LIKE FLAGS FOR ENABLING OR DISABLING CERTAIN FUNCTIONALITIES
}

void drawSurfaceOfRevolution()
{
	// MODIFY THIS LINE OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	glDrawArrays(GL_TRIANGLES, 0, 6*3*y_steps*radial_steps);
}

void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// MODIFY THIS FUNCTION FOR KEYBOARD INTERACTIVITY
	//GLFW Reference Links:
	// Callback Example: https://www.glfw.org/docs/3.3/input_guide.html#input_key
	// List of Keys: https://www.glfw.org/docs/3.3/group__keys.html

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
		// increase the alpha value by a factor of 2
		if (alpha < 1.0){
			alpha = 1.0;
		}
		alpha *= 2.0;
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
		// decrease the alpha value by a factor of 2
		alpha /= 2.0;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
			// light position up
		delta_position += vec3(0.0, 0.1, 0.0);
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
			// light position down
			delta_position -= vec3(0.0, 0.1, 0.0);
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		// light position right
		delta_position += vec3(0.1, 0.0, 0.0);
		}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
			// light position left
			delta_position -= vec3(0.1, 0.0, 0.0);

	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
			// light position forward
			delta_position += vec3(0.0, 0.0, 0.1);
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
			// light position backward
			delta_position -= vec3(0.0, 0.0, 0.1);

	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
			// diffuse flag toggle
			diffuse_flag = !diffuse_flag;
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
			// specular flag
			specular_flag = !specular_flag;
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
			// albedo texture flag
			albedo_flag = !albedo_flag;

	}


	if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
		// Close window when esacape is pressed
			glfwSetWindowShouldClose (g_window, 1);
	}

}
