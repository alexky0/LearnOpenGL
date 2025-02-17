#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Shader
{
private:
	int ID;
	mutable unordered_map<string, GLint> uniformLocations;
	int getUniformLocation(const char* name) const;
	static string get_file_contents(const char* filename);
public:
	Shader(const char* vertFile, const char* fragFile);

	void Bind() const { glUseProgram(ID); }
	void Unbind() const { glUseProgram(0); }
	void Delete() const { glDeleteProgram(ID); }

	void setMat4fv(const char* name, const glm::mat4& matrix) const { glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)); }
	void set1i(const char* name, int num) const { glUniform1i(getUniformLocation(name), num); }
	void set1f(const char* name, float num) const { glUniform1f(getUniformLocation(name), num); }
	void setVec3(const char* name, float f0, float f1, float f2) const { glUniform3f(getUniformLocation(name), f0, f1, f2); }
	void setVec3(const char* name, const glm::vec3& vec) const { glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z); }
};