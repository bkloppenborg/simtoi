/* CVoronoi.cpp
 *
 *  Created on: Jan. 10, 2014
 *      Author: bkloppenborg
 */

#include "CVoronoi.h"
#include "CShaderFactory.h"

CVoronoi::CVoronoi(unsigned int n_regions = 1)
: 	CModel(3*n_regions)
{
	// give this object a name
	mName = "Voronoi tesselation model";
	mNRegions = n_regions;

	for(unsigned int i = 0; i < mNRegions; i++)
	{
		// Define the placement of the density disk model parameters
		mParamNames.push_back("x");
		SetParam(mBaseParams + 3*i + 1, 0);
		SetFree(mBaseParams + 3*i + 1, true);
		SetMax(mBaseParams + 3*i + 1, -12);
		SetMin(mBaseParams + 3*i + 1, 12);

		mParamNames.push_back("y");
		SetParam(mBaseParams + 3*i + 2, 0);
		SetFree(mBaseParams + 3*i + 2, true);
		SetMax(mBaseParams + 3*i + 2, -12);
		SetMin(mBaseParams + 3*i + 2, 12);

		mParamNames.push_back("flux");
		SetParam(mBaseParams + 3*i + 3, 1);
		SetFree(mBaseParams + 3*i + 3, true);
		SetMax(mBaseParams + 3*i + 3, 1);
		SetMin(mBaseParams + 3*i + 3, 0);
	}

	// We only ever use the default shader in this instance.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("default");
}

CVoronoi::~CVoronoi()
{

}

shared_ptr<CModel> CVoronoi::Create()
{
	return shared_ptr<CModel>(new CVoronoi(16));
}

/// Draws a unit cylindrical wall in the z-direction from (z = -0.5 ... 0.5, at r = 1)
void CVoronoi::GenerateCone(vector<vec3> & vertices, vector<unsigned int> & elements)
{
	double coneRadius = 6;
	unsigned int faces = 50;
	double cone_angle = 45;

	double degInc = (2 * PI)/faces;
	float height = coneRadius / tan(cone_angle * PI / 180.0);

	// generate the vertices. We just make triangles with one corner at the
	// origin
	double curDeg = 0;
	float x = 0;
	float y = 0;
	for(unsigned int i = 0; i < faces; i++)
	{
		// top of the cone
		vertices.push_back(vec3(0.0, 0.0, 0.0));
		vertices.push_back(vec3(0.0, 0.0, 1.0));

		for(unsigned int j = 0; j < 2; j++)
		{
			x = coneRadius * cos(curDeg + j * degInc);
			y = coneRadius * sin(curDeg + j * degInc);

			vertices.push_back(vec3(x, y, -1.0 * height));
			vertices.push_back(vec3(0.0, 0.0, 1.0));
		}
		curDeg += degInc;
	}

	// now generate the indexes
	for(unsigned int i = 0; i < 3*faces; i++)
	{
		elements.push_back(i);
	}
}

void CVoronoi::Init()
{
	// Generate the verticies and elements
	vector<vec3> vbo_data;
	vector<unsigned int> elements;

	CVoronoi::GenerateCone(vbo_data, elements);
	mNElements = elements.size();

	// Create a new Vertex Array Object, Vertex Buffer Object, and Element Buffer
	// object to store the model's information.
	//
	// First generate the VAO, this stores all buffer information related to this object
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	// Generate and bind to the VBO. Upload the verticies.
	glGenBuffers(1, &mVBO); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vbo_data.size() * sizeof(vec3), &vbo_data[0], GL_STATIC_DRAW);
	// Generate and bind to the EBO. Upload the elements.
	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);

	// Next we need to define the storage format for this object for the shader.
	// First get the shader and activate it
	GLuint shader_program = mShader->GetProgram();
	glUseProgram(shader_program);
	// Now start defining the storage for the VBO.
	// The 'vbo_data' variable stores a unit cylindrical shell (i.e. r = 1,
	// h = -0.5 ... 0.5).
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
	// Now define the normals, if they are used
	GLint normAttrib = glGetAttribLocation(shader_program, "normal");
	if(normAttrib > -1)
	{
		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	}

	// Check that things loaded correctly.
	CWorkerThread::CheckOpenGLError("CVoronoi.Init()");

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Indicate the model is ready to use.
	mModelReady = true;
}

void CVoronoi::Render(GLuint framebuffer_object, const glm::mat4 & view)
{
	if(!mModelReady)
		Init();

	// Bind to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	// Activate the shader
	GLuint shader_program = mShader->GetProgram();
	mShader->UseShader();
	GLint uniColorFlag = glGetUniformLocation(shader_program, "color_from_uniform");
	glUniform1i(uniColorFlag, true);

	// bind back to the VAO
	glBindVertexArray(mVAO);

	// Look up the color uniform
    GLint uniColor = glGetUniformLocation(shader_program, "uni_color");

    // Define zero rotation
	glm::mat4 rotation = mat4();
	GLint uniRotation = glGetUniformLocation(shader_program, "rotation");
	glUniformMatrix4fv(uniRotation, 1, GL_FALSE, glm::value_ptr(rotation));

	// Define the view:
	GLint uniView = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	// Look up the translation uniform:
	GLint uniTranslation = glGetUniformLocation(shader_program, "translation");

	GLint uniScale = glGetUniformLocation(shader_program, "scale");
	glm::mat4 scale = glm::scale(mat4(), glm::vec3(1.0, 1.0, 1.0));
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));

	float x = 0;
	float y = 0;
	vec2 color = vec2(0.0, 1.0);
	glm::mat4 location;
	for(unsigned int region = 0; region < mNRegions; region++)
	{
		x = mParams[mBaseParams + 3*region + 1];
		y = mParams[mBaseParams + 3*region + 2];
		// redefine the color
		color.x = mParams[mBaseParams + 3*region + 3];

		// define the position
		location = glm::translate(mat4(), vec3(x, y, 0.0));
		glUniformMatrix4fv(uniTranslation, 1, GL_FALSE, glm::value_ptr(location));

		// define the color
	    glUniform2fv(uniColor, 1, glm::value_ptr(color));

		// draw the cone
//		glDrawElements(GL_POINTS, mNElements, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, mNElements, GL_UNSIGNED_INT, 0);
	}

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CWorkerThread::CheckOpenGLError("CVoronoi.Render()");
}

/// Overrides the default CModel::SetShader function.
void CVoronoi::SetShader(CShaderPtr shader)
{
	// This mode does not accept different shaders, do nothing here.
}
