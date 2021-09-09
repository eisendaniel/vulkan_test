#version 450

layout(location=0)out vec3 fragColor;

vec2 positions[6]=vec2[](
    vec2(0.,-.75),
    vec2(.65,.375),
    vec2(-.65,.375),
    vec2(-.65,-.375),
    vec2(.65,-.375),
    vec2(0.,.75)
);

vec3 colors[6]=vec3[](
    vec3(1.,0.,0.),
    vec3(0.,1.,0.),
    vec3(0.,0.,1.),
    vec3(1.,0.,0.),
    vec3(0.,1.,0.),
    vec3(0.,0.,1.)
);

void main(){
    gl_Position=vec4(positions[gl_VertexIndex],0.,1.);
    fragColor=colors[gl_VertexIndex];
}