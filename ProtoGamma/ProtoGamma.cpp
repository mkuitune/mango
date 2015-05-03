#include "GL/glew.h"
#include "SDL2/SDL.h"

#include "zmq.h"
#include "Commons/zhelpers.h"
#include "Commons/cma_sdl_util.h"
#include "Commons/cms_vec.h"
#include "Commons/cma_log.h"
#include "Commons/cma_messaging.h"

#include<cstdio>
#include<string>
#include<iostream>
#include<fstream>
#include<memory>
#include<thread>
#include<atomic>
#include<functional>
#include<sstream>
#include<set>

//
// GL utility functions
//
namespace cma {
class GL3Context {

public:

	LogWriter& m_log;

	std::set<GLuint> m_programs;

	GL3Context(LogWriter& log) :m_log(log)
	{
	}

	GLuint CreateShader(GLenum eShaderType, const char *strShaderFile)
	{
		char shaderSource[4096];
		char inChar;
		FILE *shaderFile;
		int i = 0;

		shaderFile = fopen(strShaderFile, "r");
		while (fscanf(shaderFile, "%c", &inChar) > 0)
		{
			shaderSource[i++] = inChar; //loading the file's chars into array
		}
		shaderSource[i - 1] = '\0';
		fclose(shaderFile);
		m_log(shaderSource); //print to make sure the string is loaded

		GLuint shader = glCreateShader(eShaderType);
		const char *ss = shaderSource;
		glShaderSource(shader, 1, &ss, NULL);

		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar strInfoLog[4096];
			glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

			char strShaderType[16];
			switch (eShaderType)
			{
			case GL_VERTEX_SHADER: sprintf(strShaderType, "vertex"); break;
			case GL_GEOMETRY_SHADER: sprintf(strShaderType, "geometry"); break;
			case GL_FRAGMENT_SHADER: sprintf(strShaderType, "fragment"); break;
			}

			m_log("Compile failure in ", strShaderType, " shader: ", strInfoLog);
			return -1;
		}
		else
			m_log("Shader compiled sucessfully!");

		m_programs.insert(shader);
		return shader;
	}


	GLuint BuildShaderProgram(const char *vsPath, const char *fsPath)
	{
		GLuint vertexShader;
		GLuint fragmentShader;

		vertexShader = CreateShader(GL_VERTEX_SHADER, vsPath);
		fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fsPath);

		GLuint tempProgram;
		tempProgram = glCreateProgram();

		glAttachShader(tempProgram, vertexShader);
		glAttachShader(tempProgram, fragmentShader);

		glLinkProgram(tempProgram); //linking!

		//error checking
		GLint status;
		glGetProgramiv(tempProgram, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar strInfoLog[4096];
			glGetProgramInfoLog(tempProgram, infoLogLength, NULL, strInfoLog);
			m_log("Shader linker failure: ", strInfoLog);
			return -1;
		}
		else
			m_log("Shader linked sucessfully!");

		glDetachShader(tempProgram, vertexShader);
		glDetachShader(tempProgram, fragmentShader);

		return tempProgram;
	}

};

}// cma

// cms does not contain platform specific code, only cma does
namespace cms{
	namespace VertexFormat{
		struct ColorPosition{
			Vec3f position;
			Vec4f color;
		};
	}
}


void checkLogGlError(cma::LogWriter& log){
	int err = glGetError();
	if (err){
		log("glError: ", std::to_string(err), ", ", std::string((char*) glewGetErrorString(err)));
		
	}
}

#if 1
void app_thread_runner()
{
	cma::LogWriter log;
	log.open();
	log.log("Application started");
	cma::GL3Context glc(log);

	// TODO open PUB socket for passing ui events as messages
	// TODO open SUB socket for accepting modifications to ui state

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return;

	// Configure Multisampling
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	SDL_Window *window = SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640, 480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == NULL)
	{
		log("There was an error creating the OpenGL context!\n");
		return;
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL)
	{
		log("There was an error creating the OpenGL context!\n");
		return;
	}

	SDL_GL_MakeCurrent(window, glContext);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//
	// Set multisampling options
	//
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	//MUST make a context AND make it current BEFORE glewInit()!
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status != 0)
	{
		log("Error: ", ((char*)glewGetErrorString(glew_status)));
//		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return;
	}

	/* Screen quad
	* []
	* */
	const float triangleVertices [] = {
		-1.0f, -1.0f, 0.0f, 
		1.0f, -1.0f, 0.0f, 
		-1.0f, 1.0f, 0.0f,

		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,

		//next part contains vertex colors
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};

	//size_t colorDataOffset = sizeof(triangleVertices) / 2;
	size_t verticesSize = 3 * sizeof(float) * 6;
    size_t colorDataOffset = verticesSize;
	size_t nTriangles = 2;

	float uniform_color [] = { 1.0, 1.0, 0.0, 1.0 };

	GLuint theShaderProgram;
	theShaderProgram = glc.BuildShaderProgram("vs3.glsl", "fs3.glsl");
	if (theShaderProgram == -1)
	{
		SDL_Quit();
		return;
	}

	log("Using program", std::to_string(theShaderProgram));

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.

	GLuint triangleBufferObject;
	glGenBuffers(1, &triangleBufferObject); //create the buffer

	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //formatting the data for the buffer

	glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
	glEnableVertexAttribArray(1); //attribute 1 is for vertex color data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) colorDataOffset); //color data is 48 bytes in to the array
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers
	glBindVertexArray(0);

	//log("glError: %d\n", std::to_string(glGetError()));
	checkLogGlError(log);

	char bGameLoopRunning = 1;
	while (bGameLoopRunning)
	{
		SDL_Event e;
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYUP)
			{
				//log.log_message("Key up");
				std::stringstream str;
				str << "Key press:";
				cma::sdl_keycode_to_stream(str, e.key.keysym.sym);
				log.log(str.str());
			}

			if (e.type == SDL_QUIT)
				bGameLoopRunning = 0;
			else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
				bGameLoopRunning = 0;
		}

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		/* drawing code in here! */
		glUseProgram(theShaderProgram);

		glBindVertexArray(vao);

		//glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //bind the buffer we're applying attributes to
		//glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
		//glEnableVertexAttribArray(1); //attribute 1 is for vertex color data
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
		//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) colorDataOffset); //color data is 48 bytes in to the array

		// Attach uniforms

		GLint uniform_color_loc = glGetUniformLocation(theShaderProgram, "uniform_color");
		glProgramUniform4fv(theShaderProgram, uniform_color_loc, 1, uniform_color);

		glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		glUseProgram(0);
		glBindVertexArray(0);
		/* drawing code above here! */

		SDL_GL_SwapWindow(window);
		SDL_Delay(20);
	}

	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
	return;
}

void event_listener_runner(){

}

int main(int argc, char **argv)
{

	cma::Messaging::init();
	cma::Log::init();
	// TODO event handler thread
	//std::thread event_thread(event_listener_runner);
	std::thread ui(app_thread_runner);
	// TODO open SUB socket for recieving ui events as messages
	// TODO open PUB socket for forwarding modifications to ui state
	//event_thread.join();
	ui.join();
	cma::Log::end();
	cma::Messaging::end();
	return 0;
}
#endif

#if 0
int main(int argc, char **argv)
{
	cma::start_log();
	SDL_Delay(100);
	cma::LogWriter log;
	log.open();
	log.log_message("Application started");

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_Window *window = SDL_CreateWindow("My Game Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if(glContext == NULL)
    {
        printf("There was an error creating the OpenGL context!\n");
        return 0;
    }

    const unsigned char *version = glGetString(GL_VERSION);
    if(version == NULL)
    {
        printf("There was an error creating the OpenGL context!\n");
        return 1;
    }

    SDL_GL_MakeCurrent(window, glContext);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    //MUST make a context AND make it current BEFORE glewInit()!
    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if(glew_status != 0)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return 1;
    }

    /* Screen quad 
     * []
     * */
    const float triangleVertices[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f,
         -1.0f, 1.0f, 0.0f, 1.0f,

         1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 0.0f, 1.0f,
         -1.0f, 1.0f, 0.0f, 1.0f,

        //next part contains vertex colors
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    size_t colorDataOffset = sizeof(triangleVertices) / 2;
    size_t nTriangles = 2;

    float uniform_color[] = {1.0, 1.0, 0.0, 1.0};

    GLuint theShaderProgram;
    theShaderProgram = BuildShaderProgram("vs3.glsl", "fs3.glsl");
    if(theShaderProgram == -1)
    {
        SDL_Quit();
        return 0;
    }

    printf("Using program %d\n", theShaderProgram);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.

    GLuint triangleBufferObject;
    glGenBuffers(1, &triangleBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers

    printf("glError: %d\n", glGetError());

    char bGameLoopRunning = 1;
    while(bGameLoopRunning)
    {
        SDL_Event e;
        if(SDL_PollEvent(&e))
        {
			if (e.type == SDL_KEYUP)
			{
				std::string msg = "Key up" + std::to_string(e.key.keysym.sym);
				log.log_message(msg.c_str());
			}
			
            if(e.type == SDL_QUIT)
                bGameLoopRunning = 0;
            else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                bGameLoopRunning = 0;
        }

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        /* drawing code in here! */
        glUseProgram(theShaderProgram);


        glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //bind the buffer we're applying attributes to
        glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
        glEnableVertexAttribArray(1); //attribute 1 is for vertex color data
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) colorDataOffset); //color data is 48 bytes in to the array

        
        // Attach uniforms

        GLint uniform_color_loc = glGetUniformLocation(theShaderProgram, "uniform_color");
        glProgramUniform4fv(theShaderProgram, uniform_color_loc, 1, uniform_color);


        glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glUseProgram(0);
        /* drawing code above here! */

        SDL_GL_SwapWindow(window);
        SDL_Delay(20);
    }

	cma::end_log();
    SDL_GL_DeleteContext(glContext);
    SDL_Quit();
	
    return 0;
}
#endif

#if 0
std::atomic<bool> printer_live;

void print_thread()
{

	while (printer_live)
	{

	}

	return;
}


int main(int argc, char* argv [])
{
	printer_live = true;
	std::thread printer(print_thread);
}
#endif