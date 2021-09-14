#version 450

layout(binding=0)uniform UniformBuferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;

layout(location=0)in vec3 in_position;
layout(location=1)in vec3 in_color;
layout(location=2)in vec3 in_txr_coord;

layout(location=0)out vec3 frag_color;
layout(location=1)out vec3 frag_txr_coord;

void main(){
    gl_Position=ubo.proj*ubo.view*ubo.model*vec4(in_position,1.);
    frag_color=in_color;
    frag_txr_coord=in_txr_coord;
}