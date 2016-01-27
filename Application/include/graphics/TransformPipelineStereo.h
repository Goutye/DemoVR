#ifndef EBONY_GRAPHICS_TRANSFORM_PIPELINE_STEREO_H_
#define EBONY_GRAPHICS_TRANSFORM_PIPELINE_STEREO_H_

#include <deque>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ebony {

    struct Screen {
        float width;
        float height;
        glm::mat4 p_screen_cave;
        glm::vec3 o_screen_cave;
        int vwidth;
        int vheight;
        int vx;
        int vy;
    };

	class TransformPipelineStereo {
	public:
		
		enum Eye {
			Left,
			Right
		};
		
		TransformPipelineStereo();
		TransformPipelineStereo(const TransformPipelineStereo &tp) = delete;
		
		TransformPipelineStereo &operator=(const TransformPipelineStereo &tp) = delete;

		void perspective(const glm::mat3 &PCameraInWorld, const glm::mat3 &PCaveInWorld, const glm::vec3 &OCameraInWorld,
						 const glm::vec3 &OEyeInCave, float near, float far, float eyeDistance, const Screen &screen);

		void lookat(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);
		
		TransformPipelineStereo &translate(const glm::vec3 &offset);
		TransformPipelineStereo &translate(float x, float y, float z);

		TransformPipelineStereo &rotate(float rx, float ry, float rz);
		TransformPipelineStereo &rotate(const glm::vec3 &axis, float angle);

		TransformPipelineStereo &scale(float scale);
		TransformPipelineStereo &scale(float sx, float sy, float sz);
		TransformPipelineStereo &scale(const glm::vec3 &scale);

		TransformPipelineStereo &identity();

		void save();
		void restore();

		const glm::mat4 &getProjection(Eye eye);
		const glm::mat4 &getModel();
		const glm::mat4 &getView(Eye eye);
		const glm::mat4 &getModelView(Eye eye);
		const glm::mat4 &getMvp(Eye eye);
		const glm::mat3 &getNormal(Eye eye);
	private:
		void updateCache();
		
		float _eyeDistance;
		glm::mat4 _projection[2];
		glm::mat4 _view[2];
		glm::mat4 _model;
		std::deque<glm::mat4> _modelStack;

		bool _isDirty;
		glm::mat4 _modelView[2];
		glm::mat4 _mvp[2];
		glm::mat3 _normal[2];
	};

}

#endif
