#include "Common.h"
#include "Quad.h"


CQuad::CQuad()
{}

CQuad::~CQuad()
{}


// Create the quad, including its geometry, texture mapping, normal, and colour
void CQuad::Create(string filename)
{
	// Load the texture
	m_texture.Load(filename, true);

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a VBO
	m_vbo.Create();
	m_vbo.Bind();

	// Vertex positions
	glm::vec3 quadVertices[4] =
	{
		glm::vec3(-1, -1, 0),
		glm::vec3(-1, 1, 0),
		glm::vec3(1, -1, 0),
		glm::vec3(1, 1, 0),
	};

	// Texture coordinates
	glm::vec2 quadTexCoords[4] =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1),
		glm::vec2(1, 0.0f),
		glm::vec2(1, 1)
	};

	// Quad normal
	glm::vec3 quadNormal = glm::vec3(0.0f, 1.0f, 0.0f);

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 4; i++) {
		m_vbo.AddData(&quadVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&quadTexCoords[i], sizeof(glm::vec2));
		m_vbo.AddData(&quadNormal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Render the quad as a triangle strip
void CQuad::Render(bool bindTexture)
{
	glBindVertexArray(m_vao);
	if (bindTexture) m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// Release resources
void CQuad::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}