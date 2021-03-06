//  Team members - Jiale Feng, Geethanjali Jeevanatham, Chloe McPherson
//  GLSphereExt.cpp
//  HCI557_GLSphereDirect
//

#include "GLSphereDirectSpot.h"




GLSphereDirect::GLSphereDirect(float center_x, float center_y, float center_z, float radius, int rows, int segments )
{

    _center.x() = center_x;
    _center.y() = center_y;
    _center.z() = center_z;


    _radius = radius;
    _rows = rows;
    _segments = segments;

    _render_normal_vectors = false;

    initShader();
    initVBO();

    initShaderNormal();
    initVBONormals();

}


GLSphereDirect::~GLSphereDirect()
{

    // Program clean up when the window gets closed.
    glDeleteVertexArrays(1, _vaoID);
    glDeleteVertexArrays(1, _vaoIDNormals);
    glDeleteProgram(_program);
    glDeleteProgram(_program_normals);
}





/*
 Inits the shader program for this object
 */
void GLSphereDirect::initShader(void)
{
#ifdef _WIN32
    // This loads the shader program from a file
	_program = LoadAndCreateShaderProgram("../data/shaders/p1.vs", "../data/shaders/p1.fs");
#else
	 // This loads the shader program from a file
	_program = LoadAndCreateShaderProgram("../../data/shaders/p1.vs", "../../data/shaders/p1.fs");
#endif
    glUseProgram(_program);


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Vertex information / names

    glBindAttribLocation(_program, 0, "in_Position");
    glBindAttribLocation(_program, 1, "in_Normal");
    glBindAttribLocation(_program, 2, "in_Color");


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Define the model view matrix.


    _modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Create our model matrix which will halve the size of our model


    _projectionMatrixLocation = glGetUniformLocation(_program, "projectionMatrixBox"); // Get the location of our projection matrix in the shader
    _viewMatrixLocation = glGetUniformLocation(_program, "viewMatrixBox"); // Get the location of our view matrix in the shader
    _modelMatrixLocation = glGetUniformLocation(_program, "modelMatrixBox"); // Get the location of our model matrix in the shader



    glUniformMatrix4fv(_projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix()[0][0] ); // Send our projection matrix to the shader
    glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &viewMatrix()[0][0]); // Send our view matrix to the shader
    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); // Send our model matrix to the shader


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Material
    _material._diffuse_material = glm::vec3(0.0, 0.0, 1.0);
    _material._ambient_material = glm::vec3(0.0, 0.0, 0.0);
    _material._specular_material = glm::vec3(1.0, 1.0, 1.0);
    _material._shininess = 1.0;


    _material._ambientColorPos = glGetUniformLocation(_program, "ambient_color");
    _material._diffuseColorPos = glGetUniformLocation(_program, "diffuse_color");
    _material._specularColorPos = glGetUniformLocation(_program, "specular_color");
    _material._shininessIdx = glGetUniformLocation(_program, "shininess");


    // Send the material to your shader program
    glUniform3fv(_material._ambientColorPos, 1, &_material._ambient_material[0] );
    glUniform3fv(_material._diffuseColorPos, 1, &_material._diffuse_material[0]);
    glUniform3fv(_material._specularColorPos, 1, &_material._specular_material[0]);
    glUniform1f(_material._shininessIdx, _material._shininess);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Light


    //Normal light
	// define the position of the light and send the light position to your shader program
	_light_source0._lightPos = glm::vec4(0.0, 0.0, 30.0, 1.0);
    _light_source0._ambient_intensity = 0.1;
    _light_source0._specular_intensity = 0.1;
    _light_source0._diffuse_intensity = 10.0;


	// Read all the index values from the shader program
    _light_source0._ambientIdx = glGetUniformLocation(_program, "ambient_intensity0");
    _light_source0._diffuseIdx = glGetUniformLocation(_program, "diffuse_intensity0");
    _light_source0._specularIdx = glGetUniformLocation(_program, "specular_intensity0");
    _light_source0._lightPosIdx = glGetUniformLocation(_program, "light_position0");


    // Send the light information to your shader program
    glUniform1f(_light_source0._ambientIdx, _light_source0._ambient_intensity );
    glUniform1f(_light_source0._diffuseIdx, _light_source0._diffuse_intensity);
    glUniform1f(_light_source0._specularIdx, _light_source0._specular_intensity);
    glUniform4fv(_light_source0._lightPosIdx, 1, &_light_source0._lightPos[0]);


	//Spotlight
    // define the position of the light and send the light position to your shader program
    _light_source1._lightPos = glm::vec4(0.0, 0.0, 30, 0.0);
    _light_source1._ambient_intensity = 0.3;
    _light_source1._specular_intensity = 2.0;
    _light_source1._diffuse_intensity = 5.0;
    _light_source1._attenuation_coeff = 0.02;

    _light_source1._cone_angle = 6.0; // in degree
    _light_source1._cone_direction = glm::vec3(-2.0, 0.0, -25.0); // this must be aligned with the object and light position.


    // Read all the index values from the shader program
    _light_source1._ambientIdx = glGetUniformLocation(_program, "ambient_intensity");
    _light_source1._diffuseIdx = glGetUniformLocation(_program, "diffuse_intensity");
    _light_source1._specularIdx = glGetUniformLocation(_program, "specular_intensity");
    _light_source1._attenuation_coeffIdx = glGetUniformLocation(_program, "attenuationCoefficient");
    _light_source1._lightPosIdx = glGetUniformLocation(_program, "light_position");
    _light_source1._cone_angleIdx = glGetUniformLocation(_program, "cone_angle");
    _light_source1._cone_directionIdx = glGetUniformLocation(_program, "cone_direction");

    // Send the light information to your shader program
    glUniform1f(_light_source1._ambientIdx, _light_source1._ambient_intensity );
    glUniform1f(_light_source1._diffuseIdx, _light_source1._diffuse_intensity);
    glUniform1f(_light_source1._specularIdx, _light_source1._specular_intensity);
    glUniform1f(_light_source1._attenuation_coeffIdx, _light_source1._attenuation_coeff);
    glUniform4fv(_light_source1._lightPosIdx, 1, &_light_source1._lightPos[0]);

    glUniform1f(_light_source1._cone_angleIdx, _light_source1._cone_angle);
    glUniform3fv(_light_source1._cone_directionIdx, 1, &_light_source1._cone_direction[0]);



    glUseProgram(0);

}
