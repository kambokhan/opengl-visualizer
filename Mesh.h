#pragma once

#include <GL/glew.h>
#include "Shader.h"

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void RenderMesh();
    void CalcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
		unsigned int vLength, unsigned int normalOffset);
	void ClearMesh();

	~Mesh();
private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount, vertexCount;


};

