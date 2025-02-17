#include "Shader.h"

int Shader::getUniformLocation(const char* name) const
{
	if (uniformLocations.find(name) != uniformLocations.end()) return uniformLocations[name];
	int location = glGetUniformLocation(ID, name);
	uniformLocations[name] = location;
	return location;
}

string Shader::get_file_contents(const char* filename)
{
	ifstream file(filename, ios::binary);
	if (!file) { cout << "Shader not found: " << filename << endl; return ""; }
	return string(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

Shader::Shader(const char* vertFile, const char* fragFile)
{
	string vertexSourceCode = get_file_contents(vertFile);
	string fragmentSourceCode = get_file_contents(fragFile);
	const char* vertexSource = vertexSourceCode.c_str();
	const char* fragmentSource = fragmentSourceCode.c_str();

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}