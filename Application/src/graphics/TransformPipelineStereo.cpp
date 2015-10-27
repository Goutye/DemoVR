#include "graphics/TransformPipelineStereo.h"

#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/maths.h"

using namespace std;
using namespace glm;

namespace ebony {

	TransformPipelineStereo::TransformPipelineStereo()
	{
		_isDirty = true;
	}

	void TransformPipelineStereo::perspective(const glm::mat3 &PCaveInWorld, const glm::vec3 &OCaveInWorld,
											  const glm::vec3 &OEyeInCave, float near, float far, float eyeDistance)
	{
		static const glm::vec2 screenSize(0.4f, 0.22f);
		static const glm::mat3 PScreenInCaveInv = glm::inverse(glm::mat3(1, 0, 0,
																		 0, 1, 0,
																		 0, 0, 1));
		static const glm::vec3 OScreenInCave(0, 0.11f, 0);

		_eyeDistance = eyeDistance;

		glm::mat4 viewMatrix;
		glm::vec3 OEyeInScreen;

		OEyeInScreen = glm::vec3(mat4(PScreenInCaveInv) * glm::translate(glm::mat4(1.0f), -OScreenInCave) * glm::vec4(OEyeInCave, 1.0f));

		viewMatrix = glm::mat4(PScreenInCaveInv)
			* glm::translate(glm::mat4(1.0f), -OScreenInCave) * glm::mat4(glm::inverse(PCaveInWorld))
			* glm::translate(glm::mat4(1.0f), -OCaveInWorld);
		
		{
			OEyeInScreen.x -= eyeDistance / 2;

			float left = (-screenSize.x / 2 - OEyeInScreen.x) * near / glm::abs(OEyeInScreen.z);
			float right = (screenSize.x / 2 - OEyeInScreen.x) * near / glm::abs(OEyeInScreen.z);
			float top = (screenSize.y / 2 - OEyeInScreen.y) * near / glm::abs(OEyeInScreen.z);
			float bottom = (-screenSize.y / 2 - OEyeInScreen.y) * near / glm::abs(OEyeInScreen.z);
		
			_projection[Left] = glm::frustum(left, right, bottom, top, near, far);
			_view[Left] = glm::translate(glm::mat4(1.0f), -OEyeInScreen) * viewMatrix;
		}

		{
			OEyeInScreen.x += eyeDistance;

			float left = (-screenSize.x / 2 - OEyeInScreen.x) * near / glm::abs(OEyeInScreen.z);
			float right = (screenSize.x / 2 - OEyeInScreen.x) * near / glm::abs(OEyeInScreen.z);
			float top = (screenSize.y / 2 - OEyeInScreen.y) * near / glm::abs(OEyeInScreen.z);
			float bottom = (-screenSize.y / 2 - OEyeInScreen.y) * near / glm::abs(OEyeInScreen.z);
		
			_projection[Right] = glm::frustum(left, right, bottom, top, near, far);
			_view[Right] = glm::translate(glm::mat4(1.0f), -OEyeInScreen) * viewMatrix;
		}
		
		_isDirty = true;
	}

	void TransformPipelineStereo::lookat(const vec3 &position, const vec3 &target, const vec3 &up)
	{
		/*_view[Left] = glm::lookAt(position, target, up);
		_view[Right] = glm::lookAt(position, target, up);
		
		_isDirty = true;*/
	}
		
	TransformPipelineStereo &TransformPipelineStereo::translate(const vec3 &offset)
	{
		_model = glm::translate(_model, offset);
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::translate(float x, float y, float z)
	{
		_model = glm::translate(_model, vec3(x, y, z));
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::rotate(float rx, float ry, float rz)
	{
		_model = glm::rotate(_model, rx, vec3(1, 0, 0));
		_model = glm::rotate(_model, ry, vec3(0, 1, 0));
		_model = glm::rotate(_model, rz, vec3(0, 0, 1));
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::rotate(const vec3 &axis, float angle)
	{
		_model = glm::rotate(_model, angle, axis);
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::scale(float scale)
	{
		_model = glm::scale(_model, vec3(scale, scale, scale));
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::scale(float sx, float sy, float sz)
	{
		_model = glm::scale(_model, vec3(sx, sy, sz));
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::scale(const vec3 &scale)
	{
		_model = glm::scale(_model, scale);
		_isDirty = true;

		return *this;
	}

	TransformPipelineStereo &TransformPipelineStereo::identity()
	{
		_model = mat4();
		_isDirty = true;

		return *this;
	}

	void TransformPipelineStereo::save()
	{
		_modelStack.push_back(_model);
	}

	void TransformPipelineStereo::restore()
	{
		if (_modelStack.empty()) {
			_model = mat4();
		} else {
			_model = _modelStack.back();
			_modelStack.pop_back();
		}

		_isDirty = true;
	}

	const mat4 &TransformPipelineStereo::getProjection(Eye eye)
	{
		updateCache();
		return _projection[eye];
	}

	const mat4 &TransformPipelineStereo::getModel()
	{
		updateCache();
		return _model;
	}

	const mat4 &TransformPipelineStereo::getView(Eye eye)
	{
		updateCache();
		return _view[eye];
	}

	const mat4 &TransformPipelineStereo::getModelView(Eye eye)
	{
		updateCache();
		return _modelView[eye];
	}

	const mat4 &TransformPipelineStereo::getMvp(Eye eye)
	{
		updateCache();
		return _mvp[eye];
	}

	const mat3 &TransformPipelineStereo::getNormal(Eye eye)
	{
		updateCache();
		return _normal[eye];
	}

	void TransformPipelineStereo::updateCache()
	{
		if (!_isDirty) {
			return;
		}

		_isDirty = false;
		
		for (int i = 0; i < 2; ++i) {
			_modelView[i] = _view[i] * _model;
			_mvp[i] = _projection[i] * _modelView[i];
			_normal[i] = mat3(transpose(inverse(_modelView[i])));
		}
	}

}
