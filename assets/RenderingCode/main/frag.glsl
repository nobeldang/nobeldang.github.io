#version 410


// Define INPUTS from fragment shader
in vec3 normal_vec;
in vec3 position;
in vec2 textCoords;

uniform mat4 view_mat;
uniform float alpha;
uniform vec3 delta_position;
uniform float diffuse_flag, specular_flag, albedo_flag;
vec3 cam_pos = vec3(0.0, 0.0, 0.0);
vec3 ka = vec3(0.0, 0.4, 0.6);
vec3 ks = vec3(0.7, 0.3, 0.0);
vec3 Ia = vec3(0.2, 0.3, 0.4);
vec3 I_dir = vec3(0.5, 0.3, 0.8);
vec3 I_pt = vec3(0.6, 0.3, 0.1);

vec3 normal;
vec3 l_dir = normalize(vec3(2.0, 3.0, 1.0));
vec3 l = normalize(vec3(cam_pos  + delta_position - position));
vec3 v = normalize(vec3(cam_pos - position));
vec3 half_vec;
vec3 half_vec_dir;
vec3 phong;

// These come from the VAO for texture coordinates.
//in vec2 texture_coords;

// And from the uniform outputs for the textures setup in main.cpp.
uniform sampler2D texture00;
uniform sampler2D texture01;

out vec4 fragment_color; //RGBA color
void main () {
    vec4 clrT = albedo_flag==1.0?texture(texture00, textCoords):vec4(1.0, 1.0, 1.0, 1.0);
    normal = vec3(normalize(normal_vec));
    half_vec = normalize(vec3(l + v));
    half_vec_dir = normalize(vec3(l_dir + v));
    phong = (vec3(dot(ka, Ia)) + 
    vec3(dot(ka, I_pt))*max(0, dot(normal, l))*diffuse_flag + vec3(dot(ka, I_dir))*max(0, dot(normal, l_dir))*diffuse_flag)*clrT.rgb +
    vec3(dot(ks, I_pt))*max(0, pow(dot(normal, half_vec), alpha))*specular_flag +
     + vec3(dot(ks, I_dir))*max(0, pow(dot(normal, half_vec_dir), alpha))*specular_flag;
    
    fragment_color = vec4(phong, 1.0);

}
