#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>

// function declarations

struct ShaderSource {
	std::string vertexSource;
	std::string fragmentSource;
};

static unsigned int CompileShader(unsigned int type, const std::string& source);

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

static ShaderSource ParseShader(const std::string& filepath);



int main(void)
{
	GLFWwindow* window;


	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* this runs if glew could not be initialised */
	if (glewInit() != GLEW_OK)
		std::cout << "error!" << std::endl;

	//std::cout << glGetString(GL_VERSION) << std::endl;

	// ============
	// buffer stuff
	// ============


	float bufferPositionData[] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};	// store the data that will be placed in the VRAM

	unsigned int buffer;
	glGenBuffers(1, &buffer); // give the buffer an ID
	//std::cout << buffer << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, buffer); // bind the buffer to the ID
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), bufferPositionData, GL_STATIC_DRAW); // set aside places in memory for the data, then give it the data

	glEnableVertexAttribArray(0); // enable the attribute - tell the machine to use the new layout for the attribute
	// the 0's here mean that this is modifying the positions of the vertices, since there are no others here right now
	// all of them are going to be 0.

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); /* tell opengl how to interpret the data it gets -
	in this case, it is affecting the buffer that is linked to 'buffer' (that is, the vertex buffer) and tells it how
	many pieces of inforrmation are part of each thing, as well as the data type (there is also some other stuff, though
	this is not too necessary). */


	ShaderSource sourceCode = ParseShader("basic.shader");
	std::cout << sourceCode.vertexSource << std::endl;
	std::cout << sourceCode.fragmentSource << std::endl;


	unsigned int shader = CreateShader(sourceCode.vertexSource, sourceCode.fragmentSource); // build the shaders - create and compile them both
	glUseProgram(shader); // make the program use the shader




	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// make a triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		
	}
	glDeleteProgram(shader); // clean the shader program once it's finished using it
	glfwTerminate();
	return 0;
}













static unsigned int CompileShader(unsigned int type, const std::string& source) {


	unsigned int id = glCreateShader(type); // 'id' is the handle for the shader that we are using
	const char* src = source.c_str(); // turn the source into a c-type string


	glShaderSource(id, 1, &src, nullptr); // load the source into the shader
	glCompileShader(id); // run the built in compiler for shaders



	// ================
	// check for errors
	// ================



	int status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);



	if (status == GL_FALSE) { // if there was an error, print the error
		
		int len;
		
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char* message = (char*)alloca(len * sizeof(char)); // declare the message variable as a pointer to a range of values.


		glGetShaderInfoLog(id, len, &len, message); // return the information of the error

		// tell the uesr that an error occurred, and print the error
		std::cout << "Issue compiling the " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
		std::cout << message << std::endl;

		// clean up by deleting the shader
		glDeleteShader(id);

		return 0;

	}



	return id; // return the shader's handle
}




static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram(); // make the program 

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); // run the above function to load the vertex shader's source into it, and compile it
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);


	glAttachShader(program, vs); // attach the shaders to the program
	glAttachShader(program, fs);
	glLinkProgram(program); // makes the shaders use the attached shaders when rendering things
	glValidateProgram(program); // checks the attached shaders to see if what is attached can be used in the program

	return program;

	// the shaders are in a function now, so they don't need to be in their own thing any more.
	glDeleteShader(vs);
	glDeleteShader(fs);


}



static ShaderSource ParseShader(const std::string& filepath) {

	
	std::ifstream stream(filepath); // open a stream from a file

	std::string line; // buffer to store data
	std::stringstream ss[2]; // two string streams, one for the vertex and one for the fragment



	enum class ShaderType {
		NONE = -1, 
		VERTEX = 0, 
		FRAGMENT = 1
	};


	ShaderType type = ShaderType::NONE; // hold the type of shader currently in use - this changes every time the function finds the word 'shader' on a line somewhere
	
	// this is being ignored at the moment which is annoying 


	while (getline(stream, line))
	{

		if (line.find("#shader") != std::string::npos) 
		{ 
			// if it finds the text '#shader'

			if (line.find("vertex") != std::string::npos) {
				// set mode to vertex: 
				type = ShaderType::VERTEX;
			}

			else if (line.find("fragment") != std::string::npos) {
				// set mode to fragment: 
				type = ShaderType::FRAGMENT;
			}

		}

		else {

			ss[(int)type] << line << "\n"; // add the code to the appropriate string stream

		}

	}

	return { ss[0].str(), ss[1].str() };

}