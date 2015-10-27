#ifndef EBONY_APPLICATION_H_
#define EBONY_APPLICATION_H_

#include "Gamepad.h"
#include "Tracker.h"
#include "graphics/opengl/opengl.h"
#include "graphics/TransformPipelineStereo.h"
#include "graphics/StaticModel.h"
#include <SDL2/SDL.h>

#include "utils\maths.h"
#include <memory>

namespace ebony {

	class Application {
	public:
		Application(SDL_Window *window);
		~Application();
		
		void update(float dt);
		void draw(float extrapolationTime);
		void incr(float step);
			
	private:
		std::vector<StaticModel> _models;
		std::vector<std::shared_ptr<gl::Texture>> _textures;
		std::shared_ptr<gl::Program> _program;
		std::shared_ptr<gl::Sampler> _sampler;
		TransformPipelineStereo _pipeline;
		float _time = 0;
		GLuint _uniformMvp;
		GLuint _uniformSampler;

		float _screenD = 1;
		SDL_Window *window;
		Gamepad *gamepad;
		Tracker *tracker;
		float theta = -1.96725;
		float phi = -0.8050;

		glm::mat3 _PCaveInWorld;
		glm::vec3 _OCaveInWorld;
		glm::vec3 _OEyeInCave;
	};

}

#endif
