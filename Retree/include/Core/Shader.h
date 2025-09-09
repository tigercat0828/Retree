#pragma once
#include <string>
#include <glm/glm.hpp>

using GLuint = unsigned int;
using GLint = int;
using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Shader {
public:
	string Name;
	GLuint ID{ 0 };
	Shader() = default;
	Shader(const string& filename);

	void CheckCompileError(GLuint ID, const string& type) const;
	void Use() const;
	void Unuse() const;
	void Delete() const;

	void SetBool(const string& name, bool value) const;
	void SetInt(const string& name, int value) const;
	void SetFloat(const string& name, float value) const;
	void SetVec2(const string& name, const vec2& vec) const;
	void SetVec3(const string& name, const vec3& vec) const;
	void SetVec4(const string& name, const vec4& vec) const;
	void SetMat4(const string& name, const mat4& mat);

	void CheckUniformFound(const string& name, GLint uniform) const;
};