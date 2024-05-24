#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	track_width = 90.0f;

	m_texture.Load("resources\\textures\\track.png");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Turning on anisotropic filtering for the texture
	GLfloat anisotropy; 
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	m_texture.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
}

CCatmullRom::~CCatmullRom()
{}

void CCatmullRom::SetControlPoints()
{
	m_controlPoints.push_back(glm::vec3(0, 100, 800));
	m_controlPoints.push_back(glm::vec3(-500, 0, 1200));
	m_controlPoints.push_back(glm::vec3(-750, 0, 900));
	m_controlPoints.push_back(glm::vec3(-300, 0, -100));
	m_controlPoints.push_back(glm::vec3(-600, 50, -600));

	m_controlPoints.push_back(glm::vec3(-150, 80, -1300));
	m_controlPoints.push_back(glm::vec3(-220, 100, -1780));
	m_controlPoints.push_back(glm::vec3(-500, 120, -1900));
	m_controlPoints.push_back(glm::vec3(-780, 140, -1780));
	m_controlPoints.push_back(glm::vec3(-900, 160, -1500));
	m_controlPoints.push_back(glm::vec3(-780, 180, -1220));
	m_controlPoints.push_back(glm::vec3(-500, 200, -1100));

	m_controlPoints.push_back(glm::vec3(300, 250, -1200));
	m_controlPoints.push_back(glm::vec3(700, 300, -1050));
	m_controlPoints.push_back(glm::vec3(800, 300, -800));
	m_controlPoints.push_back(glm::vec3(700, 275, -550));
	m_controlPoints.push_back(glm::vec3(400, 250, -200));
	m_controlPoints.push_back(glm::vec3(500, 200, 400));
}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;
}

// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}

// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size()-1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}

// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}
}

void CCatmullRom::CreateCentreline()
{
	SetControlPoints();

	UniformlySampleControlPoints(750);

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto i = 0; i < m_centrelinePoints.size(); i++) {
		vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::CreateOffsetCurves()
{
	// Use TNB frame to calculate left and right offset curves
	for (auto i = 0; i < m_centrelinePoints.size(); i++) 
	{
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];
		glm::vec3 y = glm::vec3(0, 1, 0);

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, y));

		glm::vec3 l = p - (track_width / 2.0f) * N;
		glm::vec3 r = p + (track_width / 2.0f) * N;

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r	);
	}

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto i = 0; i < m_leftOffsetPoints.size(); i++) {
		vbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	// Create a VBO
	CVertexBufferObject vbo2;
	vbo2.Create();
	vbo2.Bind();

	for (auto i = 0; i < m_rightOffsetPoints.size(); i++) {
		vbo2.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		vbo2.AddData(&texCoord, sizeof(glm::vec2));
		vbo2.AddData(&normal, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	vbo2.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::CreateTrack()
{
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	int path_size = m_leftOffsetPoints.size();

	for (auto i = 0; i < path_size + 1; i++) {
		glm::vec2 texCoord1(0.0f, i * 0.25f);
		glm::vec2 texCoord2(1.0f, i * 0.25f);

		vbo.AddData(&m_leftOffsetPoints[i % path_size], sizeof(glm::vec3));
		vbo.AddData(&texCoord1, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		vbo.AddData(&m_rightOffsetPoints[i % path_size], sizeof(glm::vec3));
		vbo.AddData(&texCoord2, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		m_vertexCount += 2;
	}

	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::RenderCentreline()
{
	glLineWidth(5.0f);
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	glLineWidth(5.0f);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());

	glLineWidth(5.0f);
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());
}

void CCatmullRom::RenderTrack()
{
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int CCatmullRom::CurrentLap(float d)
{
	return (int)(d / m_distances.back());
}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);