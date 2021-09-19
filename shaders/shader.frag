#version 450

layout(binding=1)uniform sampler2D txr_sampler;

layout(location=0)in vec3 frag_color;
layout(location=1)in vec2 frag_txr_coord;

layout(location=0)out vec4 out_color;

void main(){
    out_color=vec4(frag_txr_coord,1.,1.)*texture(txr_sampler,frag_txr_coord);
}
