#include "pch.h"

#include "Core/Shader.h"

using std::ifstream;
using std::stringstream;

Shader::Shader(const string& filename) {
	fs::path assetsPath = fs::current_path() / "Assets" / "Shaders";

	string vshader = filename + ".vert";		// vertex shader
	string fshader = filename + ".frag";		// fragment shader
	string gshader = filename + ".geom";		// geometry shader

	fs::path vPath = assetsPath / vshader;
	fs::path fPath = assetsPath / fshader;
    fs::path gPath = assetsPath / gshader;
	
	bool isGeoshaderUsed = fs::exists(gPath);
	
	Name = filename;

	// v stand for vertex shader; f stand for fragment shader
	ifstream vFile, fFile, gFile;
	stringstream vStream, fStream, gStream;
	string vSource, fSource, gSource;
	
	vFile.exceptions(ifstream::failbit | ifstream::badbit);
	fFile.exceptions(ifstream::failbit | ifstream::badbit);
    gFile.exceptions(ifstream::failbit | ifstream::badbit);

	try {
		vFile.open(vPath);
		vStream << vFile.rdbuf();
		vSource = vStream.str();
		vFile.close();

		fFile.open(fPath);
		fStream << fFile.rdbuf();
		fSource = fStream.str();
		fFile.close();

		if (isGeoshaderUsed) {
			gFile.open(gPath);
			gStream << gFile.rdbuf();
			gSource = gStream.str();
			gFile.close();
		}
	}
	catch (ifstream::failure& e) {
		Logger::Error("Fail to open shader files : {}", Name);
		Logger::Error("{}", e.what());
	}
	
	int success;
	char infoLog[512];

	ID = glCreateProgram();


	GLuint vID = glCreateShader(GL_VERTEX_SHADER);
	const char* vSourceCStr = vSource.c_str();
	glShaderSource(vID, 1, &vSourceCStr, NULL);
	glCompileShader(vID);
	CheckCompileError(vID, "VERTEX");
	glAttachShader(ID, vID);

	GLuint fID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fSourceCStr = fSource.c_str();
	glShaderSource(fID, 1, &fSourceCStr, NULL);
	glCompileShader(fID);
	CheckCompileError(fID, "FRAGMENT");
	glAttachShader(ID, fID);
	

	GLuint gID;
	if (isGeoshaderUsed) {
		gID = glCreateShader(GL_GEOMETRY_SHADER);
		const char* gSourceCStr = gSource.c_str();
		glShaderSource(gID, 1, &gSourceCStr, NULL);
		glCompileShader(gID);
		CheckCompileError(gID, "GEOMETRY");
		glAttachShader(ID, gID);
	}
	
	glLinkProgram(ID);
	CheckCompileError(ID, "PROGRAM");

	glDeleteShader(vID);
	glDeleteShader(fID);
    if (isGeoshaderUsed) glDeleteShader(gID);
}

void Shader::CheckCompileError(GLuint ID, const string& type) const {
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(ID, 1024, NULL, infoLog);
			Logger::Warn("Failed to compile \"{}\" {} shader ID {}\n{}", Name, type, ID, infoLog);
		}
		else {
			Logger::Info("Success to compile \"{}\" {} shader ID {}", Name, type, ID);
		}
	}
	else {
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 1024, NULL, infoLog);
			Logger::Warn("Failed to link \"{}\" shader program ID {}\n{}", Name, ID, infoLog);
		}
		else {
			Logger::Info("Success to link \"{}\" shader program ID {}", Name, ID);
		}
	}
}
void Shader::Use() const { 
	glUseProgram(ID); 
}
void Shader::Unuse() const { 
	glUseProgram(0); 
}
void Shader::Delete() const { 
	glDeleteProgram(ID); 
}
void Shader::SetBool(const string& name, bool value) const {
	GLint uniform = glGetUniformLocation(ID, name.c_str());
	CheckUniformFound(name, uniform);
	glUniform1i(uniform, value);
}
void Shader::SetInt(const string& name, int value) const {
	GLint uniform = glGetUniformLocation(ID, name.c_str());
	CheckUniformFound(name, uniform);
	glUniform1i(uniform, value);
}
void Shader::SetFloat(const string& name, float value) const {
	GLint uniform = glGetUniformLocation(ID, name.c_str());
	CheckUniformFound(name, uniform);
	glUniform1f(uniform, value);
}
void Shader::SetVec2(const string& name, const vec2& vec) const {
	GLint uniform = glGetUniformLocation(ID, name.c_str());
	CheckUniformFound(name.c_str(), uniform);
	glUniform2fv(uniform, 1, &vec[0]);
}
void Shader::SetVec3(const string& name, const vec3& vec) const {
	GLint uniform = (glGetUniformLocation(ID, name.c_str()));
	CheckUniformFound(name, uniform);
	glUniform3fv(uniform, 1, &vec[0]);
}
void Shader::SetVec4(const string& name, const vec4& vec) const {
	GLint uniform = glGetUniformLocation(ID, name.c_str());
	CheckUniformFound(name, uniform);
	glUniform4fv(uniform, 1, &vec[0]);
}
void Shader::SetMat4(const string& name, const mat4& mat) {
	GLint uniform = glGetUniformLocation(ID, name.c_str());
	CheckUniformFound(name, uniform);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, &mat[0][0]);
}
void Shader::CheckUniformFound(const string& name, GLint uniform) const {
	if (uniform == -1) {
		Logger::Error("Uniform '{}' not found in shader program \"{}\"", name, Name);
		exit(-1);
		return;
	}
}
