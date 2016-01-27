#include "Application.h"

#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/io.h"

using namespace std;
using namespace ebony;
using namespace glm;

namespace ebony {

    vector<Screen> screens;

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
		
		gamepad = new Gamepad("XInput0@localhost");
		//tracker = new Tracker("Lunettes@localhost");

		_PCaveInWorld = glm::mat3(1, 0, 0,
								  0, 0, 1,
								  0, -1, 0);
		_OCameraInWorld = glm::vec3(2, 0, 1.5f);
		_OEyeInCave = glm::vec3(0, 0, 1.0f);

        Screen screen1 = {2, 2.2, 
            glm::inverse(glm::transpose(mat4(
                 vec4(1, 0, 0, 0),
                 vec4(0, 1, 0, 0),
                 vec4(0, 0, 1, 0),
                 vec4(0, 0, 0, 1)))),
            vec3(0, 0, 0),
            640, 720, 640, 0
        };

        /*Screen screen2 = {0.2, 0.22, 
            glm::inverse(glm::transpose(mat4(
                 glm::normalize(vec4(1, 0, -1, 0)),
                 glm::normalize(vec4(0, 1, 0, 0)),
                 glm::normalize(vec4(1, 0, 1, 0)),
                 glm::normalize(vec4(0, 0, 0, 1))))),
            vec3(-0.17, 0.11, 0.07),
            640, 720, 0, 0
        };*/

		Screen screen2 = {2, 2.2, 
            glm::inverse(glm::transpose(mat4(
                 glm::normalize(vec4(0, 0, 1, 0)),
                 glm::normalize(vec4(0, 1, 0, 0)),
                 glm::normalize(vec4(-1, 0, 0, 0)),
                 glm::normalize(vec4(0, 0, 0, 1))))),
            vec3(-1, 0, 1),
            640, 720, 0, 0
        };

        screens.push_back(screen1);
        screens.push_back(screen2);
	}

	Application::~Application()
	{
		delete gamepad;
		delete tracker;
	}
	
	void Application::update(float dt)
	{
		_time += dt;

		gamepad->update();

		float x, y;

		gamepad->getAnalogR(x, y);
		_PCameraInWorld = glm::mat3(glm::rotate(glm::mat4(_PCameraInWorld), -x * dt, glm::vec3(0, 0, 1)));

		glm::vec3 move;

		gamepad->getAnalogL(move.x, move.y);
		_OCameraInWorld += _PCameraInWorld * move * dt;

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

		//_OCaveInWorld.x = (sin(_time) + 1) * -2;
        //_PCaveInWorld = mat3(glm::rotate(mat4(_PCaveInWorld), 0.1f, vec3(0, 1, 0)));

        glBindSampler(0, *_sampler);
		glActiveTexture(GL_TEXTURE0);

		int a = 0;

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT);

        for (const Screen &screen : screens) {
			if (a > 0) break;
			//a++;

		    _pipeline.identity();
		    _pipeline.perspective(_PCameraInWorld, _PCaveInWorld, _OCameraInWorld, _OEyeInCave, 0.01f, 100.0f, 0.06f, screen);

			glViewport(screen.vx, screen.vy, screen.vwidth, screen.vheight);
		
		    for (int i = 0; i < 2; ++i) {
			    //glDrawBuffer(buffers[i]);
			    glColorMask(masks[i][0], masks[i][1], masks[i][2], GL_TRUE);
			    glClear(GL_DEPTH_BUFFER_BIT);
			
			    glUniformMatrix4fv(_uniformMvp, 1, GL_FALSE, glm::value_ptr(_pipeline.getMvp(eyes[i])));
			
			    for (int i = 0; i < 5; ++i) {
				    glBindTexture(GL_TEXTURE_2D, *_textures[i]);
				    _models[i].draw();
			    }
		    }
        }
	}

}