/* CDisk_ConcentricRings.h
 *
 *  Created on: Nov. 5, 2013
 *      Author: bkloppenborg
 */

#include "CDisk_ConcentricRings.h"
#include "CShaderFactory.h"
#include "CCylinder.h"

CDisk_ConcentricRings::CDisk_ConcentricRings()
: 	CModel()
{
	// give this object a name
	id = "disk_concentric_rings";
	name = "Concentric Ring Disk";

	addParameter("T_eff", 5000, 2E3, 1E6, false, 100, "T_eff", "Effective temperature (Kelvin)");
	addParameter("r_in", 0.1, 0.1, 10, false, 0.1, "Inner Radius", "Inner radius");
	addParameter("radius", 20, 0.1, 20, false, 1.0, "Radius", "Radius of the disk");
	addParameter("height", 5, 0.1, 10, false, 1.0, "Height", "Height of the disk");
	addParameter("n_rings", 50, 1, 100, false, 1, "N Rings", "An integer number of rings used in the model");

	// We load the power-law shader by default.
	auto shaders = CShaderFactory::Instance();
	mShader = shaders.CreateShader("disk_power_law");

	// Resize the texture, 1 element is sufficient.
	mFluxTexture.resize(1);
	mPixelTemperatures.resize(1);
}

CDisk_ConcentricRings::~CDisk_ConcentricRings()
{

}

shared_ptr<CModel> CDisk_ConcentricRings::Create()
{
	return shared_ptr<CModel>(new CDisk_ConcentricRings());
}

void CDisk_ConcentricRings::Init()

{
	// Generate the verticies and elements
	vector<vec3> vbo_data;
	vector<unsigned int> elements;
	unsigned int z_divisions = 20;
	unsigned int phi_divisions = 50;
	unsigned int r_divisions = 20;

	mRimStart = 0;
	CCylinder::GenreateRim(vbo_data, elements, z_divisions, phi_divisions);
	mRimSize = elements.size();
	// Calculate the offset in vertex indexes for the GenerateMidplane function
	// to generate correct element indices.
	unsigned int vertex_offset = vbo_data.size();
	mMidplaneStart = mRimSize;
	CCylinder::GenerateMidplane(vbo_data, elements, vertex_offset, r_divisions, phi_divisions);
	mMidplaneSize = elements.size() - mRimSize;

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

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Could not create buffers");

	// Initialize the shader variables and texture following the default packing
	// scheme.
	InitShaderVariables();
	InitTexture();

	// All done. Un-bind from the VAO, VBO, and EBO to prevent it from being
	// modified by subsequent calls.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Failed to bind back default buffers");

	// Indicate the model is ready to use.
	mModelReady = true;
}

void CDisk_ConcentricRings::preRender(double & max_flux)
{
	double temperature = float(mParams["T_eff"].getValue());
	mPixelTemperatures[0] = temperature;
	TemperatureToFlux(mPixelTemperatures, mFluxTexture, mWavelength, max_flux);
}

void CDisk_ConcentricRings::Render(const glm::mat4 & view, const GLfloat & max_flux)
{
	if(!mModelReady)
		Init();

	// Look up the parameters:
	const double r_in = mParams["r_in"].getValue();
	const double MaxRadius  = mParams["radius"].getValue();
	const double MaxHeight  = mParams["height"].getValue();
	int n_rings  = ceil(mParams["n_rings"].getValue());

	NormalizeFlux(max_flux);

	// Activate the shader
	GLuint shader_program = mShader->GetProgram();
	mShader->UseShader();

	// bind back to the VAO
	glBindVertexArray(mVAO);

	// Define the maximum height and radius
	GLint uniMaxRadius = glGetUniformLocation(shader_program, "r_max");
	glUniform1f(uniMaxRadius, MaxRadius);

	GLint uniMaxHeight = glGetUniformLocation(shader_program, "z_max");
	glUniform1f(uniMaxHeight, MaxHeight);

	// Set the value for the inner radius.
	GLint uniInnerRadius = glGetUniformLocation(shader_program, "r_in");
	glUniform1f(uniInnerRadius, r_in);

	// Define the view:
	GLint uniView = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	GLint uniTranslation = glGetUniformLocation(shader_program, "translation");
	glUniformMatrix4fv(uniTranslation, 1, GL_FALSE, glm::value_ptr(Translate()));

	GLint uniRotation = glGetUniformLocation(shader_program, "rotation");
	glUniformMatrix4fv(uniRotation, 1, GL_FALSE, glm::value_ptr(Rotate()));

	// Look up the scale variable location. We use it below.
	GLint uniScale = glGetUniformLocation(shader_program, "scale");

	// bind to this object's texture
	glBindTexture(GL_TEXTURE_RECTANGLE, mFluxTextureID);
	// upload the image
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, mFluxTexture.size(), 1, 0, GL_RGBA,
			GL_FLOAT, &mFluxTexture[0]);

	glDisable(GL_DEPTH_TEST);

	// Render each cylindrical wall:

    // Init scale variables
	double radius = 0;
	double height = MaxHeight;

    glm::mat4 scale;
    glm::mat4 r_scale;
    glm::mat4 h_scale = glm::scale(mat4(), glm::vec3(1.0, 1.0, height));

    // Render each of the concentric rings
	double dr = (MaxRadius - r_in) / (n_rings - 1);
	for(unsigned int i = 0; i < n_rings; i++)
	{
		// Scale the radius:
		radius = r_in + i * dr;
		r_scale = glm::scale(mat4(), glm::vec3(radius, radius, 1.0));
		scale = h_scale * r_scale;

		glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));
		glDrawElements(GL_TRIANGLE_STRIP, mRimSize, GL_UNSIGNED_INT, 0);
	}

	// Render the midplane
	r_scale = glm::scale(mat4(), glm::vec3(MaxRadius, MaxRadius, 1.0));
	glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));

	// Draw the midplane
	glDrawElements(GL_TRIANGLE_STRIP, mMidplaneSize, GL_UNSIGNED_INT, (void*) (mMidplaneStart * sizeof(float)));

	glEnable(GL_DEPTH_TEST);

	// bind back to the default texture.
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	CHECK_OPENGL_STATUS_ERROR(glGetError(), "Rendering failed.");
}

/// Overrides the default CModel::SetShader function.
void CDisk_ConcentricRings::SetShader(CShaderPtr shader)
{
	// This mode does not accept different shaders, do nothing here.
}
