#include "Application.h"

#include <string>
#include <iostream>
#include <stdexcept>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

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
		
		//gamepad = new Gamepad("XInput0@localhost");
		//tracker = new Tracker("Lunettes@localhost");

		_PCaveInWorld = glm::mat3(0, -1, 0,
								  0, 0, 1,
								  -1, 0, 0);
		_OCaveInWorld = glm::vec3(-1, 0.3, 1.8f);
		_OEyeInCave = glm::vec3(0, 0.11f, 0.6);
	}

	Application::~Application()
	{
		delete gamepad;
		delete tracker;
	}
	
	void Application::update(float dt)
	{
		_time += dt;

		//gamepad->update();
		//tracker->update();

		/*float x, y;
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
		position.z = 1.7f;

		{
			double x, y, z;
			tracker->getPos(x, y, z);

			double dist = x + 1;
			double angle = atan2(1, dist);

			_pipeline.perspective(static_cast<float>(angle * 180 / PI_D) * 2, 16, 9, 0.0001f, 1000, dist, 0.01f);

			double wx, wy, wz;
			
			wy = -x;
			wz = y;
			wx = -z;

			rpos = position;

			glm::vec3 right = glm::cross(direction, glm::vec3(0, 0, 1));
			glm::vec3 forward = glm::cross(glm::vec3(0, 0, 1), right);
			rpos += static_cast<float>(wy) * forward + static_cast<float>(wx) * right
				+ static_cast<float>(wz) * glm::vec3(0, 0, 1);
		}*/
	}

	void Application::draw(float et)
	{
		static TransformPipelineStereo::Eye eyes[2] = {TransformPipelineStereo::Left,
													   TransformPipelineStereo::Right};
		static GLenum masks[2][3] = {{GL_TRUE, GL_FALSE, GL_FALSE},
									 {GL_FALSE, GL_TRUE, GL_TRUE}};
		static GLenum buffers[2] = {GL_BACK_LEFT, GL_BACK_RIGHT};

		_OCaveInWorld.x = (sin(_time) + 1) * -20;
		
		_pipeline.identity();
		_pipeline.perspective(_PCaveInWorld, _OCaveInWorld, _OEyeInCave, 0.01f, 100.0f, 0.1f);
		
		glBindSampler(0, *_sampler);
		glActiveTexture(GL_TEXTURE0);
		
		for (int i = 0; i < 2; ++i) {
			//glDrawBuffer(buffers[i]);
			glColorMask(masks[i][0], masks[i][1], masks[i][2], GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glUniformMatrix4fv(_uniformMvp, 1, GL_FALSE, glm::value_ptr(_pipeline.getMvp(eyes[i])));
			
			for (int i = 0; i < 5; ++i) {
				glBindTexture(GL_TEXTURE_2D, *_textures[i]);
				_models[i].draw();
			}
		}
	}

}