#shader vertex

#version 330 core

layout(location = 0) in vec4 position;

void main() {

	glPosition = position;

};




#shader fragment

#version 330 core 

layout(location = 0) out vec4 colour; 

void main() {

	colour = vec4(0.05, 0.75, 0.45, 1.0); 

};