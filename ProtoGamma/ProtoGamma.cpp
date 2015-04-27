//compile with this command on Ubuntu 12.04 machine:
//gcc ProtoAlpha.cpp -o sdl2-opengl-sample -Wall -std=c99 -I/usr/local/include/SDL2 -lSDL2 -I/usr/include/GL -lGL -lGLEW -Wall
#include "GL/glew.h"
#include "SDL2/SDL.h"

#include "zmq.h"
#include "Commons/zhelpers.h"

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

//#define CMA_LOG_SOCKET "tcp://localhost:5558"

// PUB/SUB channel for logging
#define CMA_LOG_SOCKET "inproc://log"

// PUB/SUB channel for events from windowing thread
#define CMA_UIEVENT_SOCKET "inproc://uievent"

// PUB/SUB channel for modification to windowing state
#define CMA_UIMODIFY_SOCKET "inproc://uimodify"

	class LogConfig{
	public:
		const static std::string logfile;
	};
	const std::string LogConfig::logfile = std::string("log.txt");

	class Log{
	public:
		enum Mode {
			WriteStdout = 1, WriteFile = 2
		};

		std::ofstream file;
		int m_mode;
		bool m_overwrite;
		Log(){
			m_mode = WriteStdout | WriteFile;
			m_overwrite = true;
		}

		void open(){
			if (m_overwrite)
				file.open(LogConfig::logfile);
			else
				file.open(LogConfig::logfile, std::ios::app);
		}

		void write(const char* msg){
			if (m_mode & WriteFile)
				file << msg << std::endl;
			if (m_mode & WriteStdout)
				std::cout << msg << std::endl;
		}

		~Log(){
			file.close();
		}
	//
	// Logging resources
	//
		static std::shared_ptr<Log> s_log;
		static std::shared_ptr<std::thread> s_logThread;
		static std::atomic<bool> s_run_log; // TODO replace with messaging
		static void* s_msg_context;

		static void log_runner()
		{
			// Open server socket
			//		void* context = zmq_ctx_new(); // Can use this only if forked...
			void* receiver = zmq_socket(s_msg_context, ZMQ_SUB);
			//		zmq_bind(receiver, CMA_LOG_SOCKET);
			zmq_connect(receiver, CMA_LOG_SOCKET);
			zmq_setsockopt(receiver, ZMQ_SUBSCRIBE, "", 0);

			std::vector<char> msgBuf(2056);
			Log::s_log->write("Logger service started");
			while (s_run_log){
				// Read messagces from log socket
				// Write message to currently active log
				bool succ = s_recv_str(receiver, msgBuf);
				if (succ)
				{
					s_log->write(&msgBuf[0]);
				}
			}
			// Close server socket
			zmq_close(receiver);
			//		zmq_ctx_destroy(context);
			s_log->write("Logger service finalized.");
		}

		static void init(){
			// Create context
			s_msg_context = zmq_ctx_new();
			// Create log pointer
			s_log.reset(new Log());
			s_log->open();
			// Start log thread
			s_run_log = true;
			s_logThread.reset(new std::thread(log_runner));
		}
		static void end(){
			s_run_log = false;
			s_logThread->join();
			zmq_ctx_destroy(s_msg_context);
		}
	};

	std::shared_ptr<Log> Log::s_log;
	std::shared_ptr<std::thread> Log::s_logThread;
	std::atomic<bool> Log::s_run_log; // TODO replace with messaging
	void* Log::s_msg_context;//


	/// Use only from one thread.
	class LogWriter {
	public:
		void* m_sender;
		void* m_context;

		LogWriter() :m_context(Log::s_msg_context), m_sender(0){
		}

		~LogWriter(){
			// Close ZMQ client socket
			zmq_close(m_sender);
			//zmq_ctx_destroy(m_context);
		}

		void open(){
			if (m_sender) return;
			m_sender = zmq_socket(m_context, ZMQ_PUB);
			//zmq_connect(m_sender, CMA_LOG_SOCKET);
			zmq_bind(m_sender, CMA_LOG_SOCKET);
		}
		
		void operator()(const std::string& msg){ log(msg); }
		void operator()(const char* msg){ log(msg); }
		void operator()(const std::string& fst, const std::string& snd){ log(fst + snd); }
		void operator()(const std::string& fst, const std::string& snd,
			const std::string& thrd){ log(fst + snd + thrd); }
		void operator()(const std::string& fst, const std::string& snd,
			const std::string& thrd, const std::string& fourth){ log(fst + snd + thrd + fourth); }

		void log(const std::string& msg){
			log(msg.c_str());
		}

		// ZMQ client socket
		void log(const char* message){
			size_t threadHash = std::hash<std::thread::id>()(std::this_thread::get_id());
			std::string fullMessage = "thread:" + std::to_string(threadHash) + " " + std::string(message);
			if (m_sender){
				s_send(m_sender, fullMessage.c_str());
				//				s_send_noblock(m_sender, fullMessage.c_str());
			}
		}
	};

	std::ostream& SDLKeycodeToStream(std::ostream& os, Sint32 kc){
		// Append to stream based on keycode
		if (kc == 0 || kc >= SDLK_SLEEP) os << "<UNKNOWN>";
		else if (kc < SDLK_CAPSLOCK){
			os << ((char) kc);
		}
		else
		{
			switch (kc){
			case SDLK_CAPSLOCK: os << "CAPSLOCK"; break;
			case SDLK_F1: os << "F1"; break;
			case SDLK_F2: os << "F2"; break;
			case SDLK_F3: os << "F3"; break;
			case SDLK_F4: os << "F4"; break;
			case SDLK_F5: os << "F5"; break;
			case SDLK_F6: os << "F6"; break;
			case SDLK_F7: os << "F7"; break;
			case SDLK_F8: os << "F8"; break;
			case SDLK_F9: os << "F9"; break;
			case SDLK_F10: os << "F10"; break;
			case SDLK_F11: os << "F11"; break;
			case SDLK_F12: os << "F12"; break;
			case SDLK_PRINTSCREEN: os << "PRINTSCREEN"; break;
			case SDLK_SCROLLLOCK: os << "SCROLLLOCK"; break;
			case SDLK_PAUSE: os << "PAUSE"; break;
			case SDLK_INSERT: os << "INSERT"; break;
			case SDLK_HOME: os << "HOME"; break;
			case SDLK_PAGEUP: os << "PAGEUP"; break;
			case SDLK_DELETE: os << "DELETE"; break;
			case SDLK_END: os << "END"; break;
			case SDLK_PAGEDOWN: os << "PAGEDOWN"; break;
			case SDLK_RIGHT: os << "RIGHT"; break;
			case SDLK_LEFT: os << "LEFT"; break;
			case SDLK_DOWN: os << "DOWN"; break;
			case SDLK_UP: os << "UP"; break;
			case SDLK_NUMLOCKCLEAR: os << "NUMLOCKCLEAR"; break;
			case SDLK_KP_DIVIDE: os << "KP_DIVIDE"; break;
			case SDLK_KP_MULTIPLY: os << "KP_MULTIPLY"; break;
			case SDLK_KP_MINUS: os << "KP_MINUS"; break;
			case SDLK_KP_PLUS: os << "KP_PLUS"; break;
			case SDLK_KP_ENTER: os << "KP_ENTER"; break;
			case SDLK_KP_1: os << "KP_1"; break;
			case SDLK_KP_2: os << "KP_2"; break;
			case SDLK_KP_3: os << "KP_3"; break;
			case SDLK_KP_4: os << "KP_4"; break;
			case SDLK_KP_5: os << "KP_5"; break;
			case SDLK_KP_6: os << "KP_6"; break;
			case SDLK_KP_7: os << "KP_7"; break;
			case SDLK_KP_8: os << "KP_8"; break;
			case SDLK_KP_9: os << "KP_9"; break;
			case SDLK_KP_0: os << "KP_0"; break;
			case SDLK_KP_PERIOD: os << "KP_PERIOD"; break;
			case SDLK_APPLICATION: os << "APPLICATION"; break;
			case SDLK_POWER: os << "POWER"; break;
			case SDLK_KP_EQUALS: os << "KP_EQUALS"; break;
			case SDLK_F13: os << "F13"; break;
			case SDLK_F14: os << "F14"; break;
			case SDLK_F15: os << "F15"; break;
			case SDLK_F16: os << "F16"; break;
			case SDLK_F17: os << "F17"; break;
			case SDLK_F18: os << "F18"; break;
			case SDLK_F19: os << "F19"; break;
			case SDLK_F20: os << "F20"; break;
			case SDLK_F21: os << "F21"; break;
			case SDLK_F22: os << "F22"; break;
			case SDLK_F23: os << "F23"; break;
			case SDLK_F24: os << "F24"; break;
			case SDLK_EXECUTE: os << "EXECUTE"; break;
			case SDLK_HELP: os << "HELP"; break;
			case SDLK_MENU: os << "MENU"; break;
			case SDLK_SELECT: os << "SELECT"; break;
			case SDLK_STOP: os << "STOP"; break;
			case SDLK_AGAIN: os << "AGAIN"; break;
			case SDLK_UNDO: os << "UNDO"; break;
			case SDLK_CUT: os << "CUT"; break;
			case SDLK_COPY: os << "COPY"; break;
			case SDLK_PASTE: os << "PASTE"; break;
			case SDLK_FIND: os << "FIND"; break;
			case SDLK_MUTE: os << "MUTE"; break;
			case SDLK_VOLUMEUP: os << "VOLUMEUP"; break;
			case SDLK_VOLUMEDOWN: os << "VOLUMEDOWN"; break;
			case SDLK_KP_COMMA: os << "KP_COMMA"; break;
			case SDLK_KP_EQUALSAS400: os << "KP_EQUALSAS400"; break;
			case SDLK_ALTERASE: os << "ALTERASE"; break;
			case SDLK_SYSREQ: os << "SYSREQ"; break;
			case SDLK_CANCEL: os << "CANCEL"; break;
			case SDLK_CLEAR: os << "CLEAR"; break;
			case SDLK_PRIOR: os << "PRIOR"; break;
			case SDLK_RETURN2: os << "RETURN2"; break;
			case SDLK_SEPARATOR: os << "SEPARATOR"; break;
			case SDLK_OUT: os << "OUT"; break;
			case SDLK_OPER: os << "OPER"; break;
			case SDLK_CLEARAGAIN: os << "CLEARAGAIN"; break;
			case SDLK_CRSEL: os << "CRSEL"; break;
			case SDLK_EXSEL: os << "EXSEL"; break;
			case SDLK_KP_00: os << "KP_00"; break;
			case SDLK_KP_000: os << "KP_000"; break;
			case SDLK_THOUSANDSSEPARATOR: os << "THOUSANDSSEPARATOR"; break;
			case SDLK_DECIMALSEPARATOR: os << "DECIMALSEPARATOR"; break;
			case SDLK_CURRENCYUNIT: os << "CURRENCYUNIT"; break;
			case SDLK_CURRENCYSUBUNIT: os << "CURRENCYSUBUNIT"; break;
			case SDLK_KP_LEFTPAREN: os << "KP_LEFTPAREN"; break;
			case SDLK_KP_RIGHTPAREN: os << "KP_RIGHTPAREN"; break;
			case SDLK_KP_LEFTBRACE: os << "KP_LEFTBRACE"; break;
			case SDLK_KP_RIGHTBRACE: os << "KP_RIGHTBRACE"; break;
			case SDLK_KP_TAB: os << "KP_TAB"; break;
			case SDLK_KP_BACKSPACE: os << "KP_BACKSPACE"; break;
			case SDLK_KP_A: os << "KP_A"; break;
			case SDLK_KP_B: os << "KP_B"; break;
			case SDLK_KP_C: os << "KP_C"; break;
			case SDLK_KP_D: os << "KP_D"; break;
			case SDLK_KP_E: os << "KP_E"; break;
			case SDLK_KP_F: os << "KP_F"; break;
			case SDLK_KP_XOR: os << "KP_XOR"; break;
			case SDLK_KP_POWER: os << "KP_POWER"; break;
			case SDLK_KP_PERCENT: os << "KP_PERCENT"; break;
			case SDLK_KP_LESS: os << "KP_LESS"; break;
			case SDLK_KP_GREATER: os << "KP_GREATER"; break;
			case SDLK_KP_AMPERSAND: os << "KP_AMPERSAND"; break;
			case SDLK_KP_DBLAMPERSAND: os << "KP_DBLAMPERSAND"; break;
			case SDLK_KP_VERTICALBAR: os << "KP_VERTICALBAR"; break;
			case SDLK_KP_DBLVERTICALBAR: os << "KP_DBLVERTICALBAR"; break;
			case SDLK_KP_COLON: os << "KP_COLON"; break;
			case SDLK_KP_HASH: os << "KP_HASH"; break;
			case SDLK_KP_SPACE: os << "KP_SPACE"; break;
			case SDLK_KP_AT: os << "KP_AT"; break;
			case SDLK_KP_EXCLAM: os << "KP_EXCLAM"; break;
			case SDLK_KP_MEMSTORE: os << "KP_MEMSTORE"; break;
			case SDLK_KP_MEMRECALL: os << "KP_MEMRECALL"; break;
			case SDLK_KP_MEMCLEAR: os << "KP_MEMCLEAR"; break;
			case SDLK_KP_MEMADD: os << "KP_MEMADD"; break;
			case SDLK_KP_MEMSUBTRACT: os << "KP_MEMSUBTRACT"; break;
			case SDLK_KP_MEMMULTIPLY: os << "KP_MEMMULTIPLY"; break;
			case SDLK_KP_MEMDIVIDE: os << "KP_MEMDIVIDE"; break;
			case SDLK_KP_PLUSMINUS: os << "KP_PLUSMINUS"; break;
			case SDLK_KP_CLEAR: os << "KP_CLEAR"; break;
			case SDLK_KP_CLEARENTRY: os << "KP_CLEARENTRY"; break;
			case SDLK_KP_BINARY: os << "KP_BINARY"; break;
			case SDLK_KP_OCTAL: os << "KP_OCTAL"; break;
			case SDLK_KP_DECIMAL: os << "KP_DECIMAL"; break;
			case SDLK_KP_HEXADECIMAL: os << "KP_HEXADECIMAL"; break;
			case SDLK_LCTRL: os << "LCTRL"; break;
			case SDLK_LSHIFT: os << "LSHIFT"; break;
			case SDLK_LALT: os << "LALT"; break;
			case SDLK_LGUI: os << "LGUI"; break;
			case SDLK_RCTRL: os << "RCTRL"; break;
			case SDLK_RSHIFT: os << "RSHIFT"; break;
			case SDLK_RALT: os << "RALT"; break;
			case SDLK_RGUI: os << "RGUI"; break;
			case SDLK_MODE: os << "MODE"; break;
			case SDLK_AUDIONEXT: os << "AUDIONEXT"; break;
			case SDLK_AUDIOPREV: os << "AUDIOPREV"; break;
			case SDLK_AUDIOSTOP: os << "AUDIOSTOP"; break;
			case SDLK_AUDIOPLAY: os << "AUDIOPLAY"; break;
			case SDLK_AUDIOMUTE: os << "AUDIOMUTE"; break;
			case SDLK_MEDIASELECT: os << "MEDIASELECT"; break;
			case SDLK_WWW: os << "WWW"; break;
			case SDLK_MAIL: os << "MAIL"; break;
			case SDLK_CALCULATOR: os << "CALCULATOR"; break;
			case SDLK_COMPUTER: os << "COMPUTER"; break;
			case SDLK_AC_SEARCH: os << "AC_SEARCH"; break;
			case SDLK_AC_HOME: os << "AC_HOME"; break;
			case SDLK_AC_BACK: os << "AC_BACK"; break;
			case SDLK_AC_FORWARD: os << "AC_FORWARD"; break;
			case SDLK_AC_STOP: os << "AC_STOP"; break;
			case SDLK_AC_REFRESH: os << "AC_REFRESH"; break;
			case SDLK_AC_BOOKMARKS: os << "AC_BOOKMARKS"; break;
			case SDLK_BRIGHTNESSDOWN: os << "BRIGHTNESSDOWN"; break;
			case SDLK_BRIGHTNESSUP: os << "BRIGHTNESSUP"; break;
			case SDLK_DISPLAYSWITCH: os << "DISPLAYSWITCH"; break;
			case SDLK_KBDILLUMTOGGLE: os << "KBDILLUMTOGGLE"; break;
			case SDLK_KBDILLUMDOWN: os << "KBDILLUMDOWN"; break;
			case SDLK_KBDILLUMUP: os << "KBDILLUMUP"; break;
			case SDLK_EJECT: os << "EJECT"; break;
			case SDLK_SLEEP: os << "SLEEP"; break;
			}
		}
		return os;
	}


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

		/* So we've compiled our shaders, now we need to link them in to the program
		that will be used for rendering. */

		/*This section could be broken out into a separate function, but we're doing it here
		because I'm not using C++ STL features that would make this easier. Tutorial doing so is
		here: http://www.arcsynthesis.org/gltut/Basics/Tut01%20Making%20Shaders.html */

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
	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers

	log("glError: %d\n", std::to_string(glGetError()));

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
				cma::SDLKeycodeToStream(str, e.key.keysym.sym);
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

	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
	return;
}

void event_listener_runner(){

}

int main(int argc, char **argv)
{
	cma::Log::init();
	// TODO event handler thread
	//std::thread event_thread(event_listener_runner);
	std::thread ui(app_thread_runner);
	// TODO open SUB socket for recieving ui events as messages
	// TODO open PUB socket for forwarding modifications to ui state
	//event_thread.join();
	ui.join();
	cma::Log::end();
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