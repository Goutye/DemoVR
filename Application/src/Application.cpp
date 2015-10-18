#include "Application.h"

#include <string>
#include <iostream>
#include <stdexcept>

#include <GL/glew.h>

#include "utils/io.h"

using namespace std;
using namespace ebony;

namespace ebony {

	Application::Application(SDL_Window *window) : window(window)
	{
		_program = make_shared<gl::Program>();
		glClearColor(1, 1, 1, 1);
		
		string xmlSource, error;
		string modelNames[] = {"floor", "table", "suzanne", "cone", "torus"};
		
		readFile("assets/programs/default.xml", xmlSource);
		
		if (!linkProgramFromXml(*_program, xmlSource, error)) {
			throw runtime_error(error);
		}
		
		glUseProgram(*_program);
		_uniformMvp = glGetUniformLocation(*_program, "uMvp");
		_uniformSampler = glGetUniformLocation(*_program, "uSampler");
		
		_pipeline.perspective(70, 16, 9, 0.001f, 100, 3.0f, 0.015f);

		for (int i = 0; i < 5; ++i) {
			shared_ptr<gl::Texture> texture = make_shared<gl::Texture>();

			_models.push_back(StaticModel("assets/models/" + modelNames[i] + ".cobj"));
			_textures.push_back(texture);

			if (!gl::loadTextureFromFile(*texture, "assets/textures/" + modelNames[i] + ".png", error)) {
				throw runtime_error(error);
			}
			
			gl::generateMipmaps(*texture);
		}

		_sampler = make_shared<gl::Sampler>();
		glSamplerParameteri(*_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(*_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(*_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(*_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		gamepad = new Gamepad("XInput0@localhost");
		position = glm::vec3(2.f, -1.f, 1.f);
	}
	
	void Application::update(float dt)
	{
		_time += dt;
		gamepad->update();

		float x, y;
		gamepad->getAnalogR(x, y);
		theta += y * dt;
		phi -= x * dt;
		direction = glm::vec3(sin(theta) * cos(phi),
							sin(theta) * sin(phi),
							cos(theta));

		x = 0, y = 0;
		gamepad->getAnalogL(x, y);
		x *= 0.1, y *= 0.1;
		position += y * direction + x * glm::cross(direction, glm::vec3(0, 0, 1));
		position.z = 1.9f;
	}

	void Application::incr(float step)
	{
		_screenD += step;
		_pipeline.perspective(70, 16, 9, 0.0001f, 1000, _screenD, 0.01f);
	}
	
	void Application::draw(float et)
	{
		static TransformPipelineStereo::Eye eyes[2] = {TransformPipelineStereo::Left,
													   TransformPipelineStereo::Right};
		static GLenum masks[2][3] = {{GL_FALSE, GL_TRUE, GL_TRUE},
									 {GL_TRUE, GL_FALSE, GL_FALSE}};

		_pipeline.lookat(position,
						 position + direction,
						 glm::vec3(0, 0, 1));
		
		_pipeline.identity();
		
		glBindSampler(0, *_sampler);
		glActiveTexture(GL_TEXTURE0);
		
		for (int i = 0; i < 2; ++i) {
			glColorMask(masks[i][0], masks[i][1], masks[i][2], GL_FALSE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glUniformMatrix4fv(_uniformMvp, 1, GL_FALSE, glm::value_ptr(_pipeline.getMvp(eyes[i])));
			
			for (int i = 0; i < 5; ++i) {
				glBindTexture(GL_TEXTURE_2D, *_textures[i]);
				_models[i].draw();
			}
		}
	}

}