#ifndef EBONY_APPLICATION_H_
#define EBONY_APPLICATION_H_

#include "Gamepad.h"
#include "graphics/opengl/opengl.h"
#include "graphics/TransformPipelineStereo.h"
#include <SDL2/SDL.h>

#include "utils\maths.h"
#include <memory>

namespace ebony {

	class Application {
	public:
		Application(SDL_Window *window);
		
		void update(float dt);
		void draw(float extrapolationTime);
		void incr(float step);
			
	private:
		float _screenD = 1;
		SDL_Window *window;
		std::shared_ptr<gl::VertexArray> _vao;
		std::shared_ptr<gl::Buffer> _vbo;
		std::shared_ptr<gl::Program> _program;
		TransformPipelineStereo _pipeline;
		float _time = 0;
		GLuint _uniformMvp;
		Gamepad *gamepad;
		glm::vec3 position;
		glm::vec3 direction;
		float theta = -1.96725;
		float phi = -0.8050;
	};

}

#endif
