#pragma once
#include <string>
using GLuint = unsigned int;
using std::string;

class Texture {
public:		// ctor
	Texture() = default;
	Texture(const string& filename);
private:	// properties
	string Name;
	GLuint ID{ 0 };
	int width;
	int height;
	int channels;
public:		// method
	void Bind();
	void Unbind();
	void GetSize(int& width, int& height) const ;
	string GetName() const ;
};

