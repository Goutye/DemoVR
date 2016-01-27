#ifndef EBONY_APPLICATION_H_
#define EBONY_APPLICATION_H_

#include "Gamepad.h"
#include "Tracker.h"
#include "graphics/opengl/opengl.h"
#include "graphics/TransformPipelineStereo.h"
#include "graphics/StaticModel.h"
#include <SDL2/SDL.h>

#include "utils/maths.h"
#include <memory>

namespace ebony {

	class Application {
	public:
		Application(SDL_Window *window);
		~Application();
		
		void update(float dt);
		void draw(float extrapolationTime);
			
	private:
		std::vector<StaticModel> _models;
		std::vector<std::shared_ptr<gl::Texture>> _textures;
		std::shared_ptr<gl::Program> _program;
		std::shared_ptr<gl::Sampler> _sampler;
		TransformPipelineStereo _pipeline;
		float _time = 0;
		GLuint _uniformMvp;
		GLuint _uniformSampler;

		SDL_Window *window;
		Gamepad *gamepad;
		Tracker *tracker;

		glm::mat3 _PCameraInWorld;
		glm::mat3 _PCaveInWorld;
		glm::vec3 _OCameraInWorld;
		glm::vec3 _OEyeInCave;
	};

}

#endif
