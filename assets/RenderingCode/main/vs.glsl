#version 410

layout (location = 0) in vec3 vtxPosition;
layout (location = 1) in vec3 vtxNormal;
layout (location = 2) in vec2 texCoords;

uniform mat4 proj_mat, view_mat, model_mat;
mat4 modelview_normal;
vec4 normal_vec_homog;
vec4 position_cam_coords_homog;
vec3 position_cam_coords;
out vec3 normal_vec;
out vec3 position;
out vec2 textCoords;

void main () {
	position_cam_coords_homog = vec4(view_mat*model_mat*vec4(vtxPosition, 1.0));
	position_cam_coords = vec3(position_cam_coords_homog[0]/position_cam_coords_homog [3],
                    position_cam_coords_homog[1]/position_cam_coords_homog[3],
                    position_cam_coords_homog[2]/position_cam_coords_homog[3]);
	gl_Position = proj_mat*position_cam_coords_homog;
	modelview_normal = mat4(transpose(inverse(view_mat*model_mat)));
	normal_vec_homog = vec4(modelview_normal*vec4(vtxNormal, 1.0));
	normal_vec = vec3(normal_vec_homog[0]/normal_vec_homog[3],
                    normal_vec_homog[1]/normal_vec_homog[3],
                    normal_vec_homog[2]/normal_vec_homog[3]);
    normal_vec = normalize(normal_vec);
    position = vec3(position_cam_coords);
    textCoords = vec2(texCoords);
	
}
