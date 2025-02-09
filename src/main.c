#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <GL/glew.h>


typedef struct {
	SDL_Window* window;
	SDL_GLContext context;
} OpenGlEnvironment;


// Definition of a new window
typedef struct {
	int width;
	int height;
	const char* title;
} NewWindow;


const NewWindow windowDef = {
	.width = 800,
	.height = 600,
	.title = "Hello World"
};


// Shader code, using GLSL
const char* shaderVert =
	"#version 410\n"
	"in vec3 a_vertex_position;\n"
	"void main() {\n"
	"    gl_Position = vec4( a_vertex_position, 1.0 );\n"
	"}";
const char* shaderFrag =
	"#version 410\n"
	"out vec4 o_frag_colour;\n"
	"void main() {\n"
	"    o_frag_colour = vec4( 0.0, 0.5, 0.75, 1.0 );\n"
	"}";


int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Error: Unable to initialise SDL. SDL Error: %s\n", SDL_GetError());
		return 1;
	}
	int error = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	if (error != 0) {
		printf("Error: with SDL_GL_CONTEXT_MAJOR_VERSION, %s\n", SDL_GetError());
	}
	error = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	if (error != 0) {
		printf("Error: with SDL_GL_CONTEXT_MINOR_VERSION, %s\n", SDL_GetError());
	}
	error = SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE);
	if (error != 0) {
		printf("Error: with SDL_GL_CONTEXT_FLAGS, %s\n", SDL_GetError());
	}
	SDL_Window* window = SDL_CreateWindow(windowDef.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowDef.width, windowDef.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		printf("Error: Window could not be created! SDL Error: %s\n", SDL_GetError());
		return 2;
	}
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		printf("Error: OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return 3;
	}
	glewExperimental = GL_TRUE;
	const GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("Error: initialising GLEW! %p\n", glewGetErrorString(glewError));
		return 4;
	}

	// Create the geometry to display
	GLuint triangleVao; // mesh/attribute descriptor handle
	GLuint triangleVbo; // handle to OpenGL copy of buffer
	const float points[] = {0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};
	glGenVertexArrays(1, &triangleVao);
	glGenBuffers(1, &triangleVbo);
	glBindVertexArray(triangleVao);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// build shaders
	const GLuint vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderHandle, 1, &shaderVert, NULL);
	glCompileShader(vertexShaderHandle);
	int params = -1;
	glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE) {
		printf("Error: vertex shader %u did not compile\n", vertexShaderHandle);
		const int maxLength = 2048;
		char slog[maxLength];
		int actualLength = 0;
		glGetShaderInfoLog(vertexShaderHandle, maxLength, &actualLength, slog);
		printf("Shader info log for GL index %u:\n%s\n", vertexShaderHandle, slog);
		glDeleteShader(vertexShaderHandle);
		return 6;
	}
	const GLuint fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderHandle, 1, &shaderFrag, NULL);
	glCompileShader(fragmentShaderHandle);
	params = -1;
	glGetShaderiv(fragmentShaderHandle, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE) {
		printf("Error: fragment shader %u did not compile\n", fragmentShaderHandle);
		const int maxLength = 2048;
		char shaderInfoLog[maxLength];
		int actualLength = 0;
		glGetShaderInfoLog(fragmentShaderHandle, maxLength, &actualLength, shaderInfoLog);
		printf("Shader info log for GL index %u:\n%s\n", fragmentShaderHandle, shaderInfoLog);
		glDeleteShader(vertexShaderHandle);
		glDeleteShader(fragmentShaderHandle);
		return 8;
	}

	// Link shaders into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fragmentShaderHandle);
	glAttachShader(shaderProgram, vertexShaderHandle);
	glLinkProgram(shaderProgram);
	params = -1;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &params);
	if (params != GL_TRUE) {
		printf("Error: could not link shader program GL index %u\n", shaderProgram);
		const int maxLength = 2048;
		char programInfoLog[maxLength];
		int actualLength = 0;
		glGetProgramInfoLog(shaderProgram, maxLength, &actualLength, programInfoLog);
		printf("Program info log for GL index %u:\n%s\n", shaderProgram, programInfoLog);
		glDeleteProgram(shaderProgram);
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 10;
	}

	// Start render loop
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	bool appIsRunning = true;
	while (appIsRunning) {
		// Check for sdl events (only for quit so far)
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				appIsRunning = false;
			}
		}

		// OpenGl render
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(triangleVao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glUseProgram(0);

		// Tell sdl to show the rendered buffer
		SDL_GL_SwapWindow(window);
		SDL_Delay(0);
	}

	// App has been flagged to stop, so tidy before we finish
	glDeleteShader(vertexShaderHandle);
	glDeleteShader(fragmentShaderHandle);
	glDeleteProgram(shaderProgram);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
