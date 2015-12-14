#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{



Grid::Grid(int width, int depth)
{
	//setup shader
	_shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/grid_shader.vert");
	_shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/grid_shader.frag");
	_shader.CreateAndLinkProgram();
	_shader.Use();	
		_shader.AddAttribute("vVertex");
		_shader.AddUniform("MVP"); 
	_shader.UnUse();

	glm::vec3* vertices = new glm::vec3[((width+1)+(depth+1))*2];
	_totalIndices = width*depth;
	GLushort* indices = new GLushort[_totalIndices];
	 
	int count = 0;
	int width_2 = width/2;
	int depth_2 = depth/2;
	int i=0 ;

	for( i=-width_2; i<=width_2;  i++) 
	{		  
		vertices[count++] = glm::vec3( i,0,-depth_2);
		vertices[count++] = glm::vec3( i,0, depth_2);

		vertices[count++] = glm::vec3( -width_2,0,i);
		vertices[count++] = glm::vec3(  width_2,0,i);
	}

	//fill indices array
	GLushort* id = &indices[0]; 
	for (i = 0; i < width*depth; i+=4) 
	{            
		*id++ = (GLushort)i;
		*id++ = (GLushort)(i+1);
		*id++ = (GLushort)(i+2);
		*id++ = (GLushort)(i+3);
	}

	//setup vao and vbo stuff
	glGenVertexArrays(1, &_vaoID);
	glGenBuffers(1, &_vboVerticesID);
	glGenBuffers(1, &_vboIndicesID);
	 
	glBindVertexArray(_vaoID);	

		glBindBuffer (GL_ARRAY_BUFFER, _vboVerticesID);
		glBufferData (GL_ARRAY_BUFFER, ((width+1)+(depth+1))*2*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		 
		glEnableVertexAttribArray(_shader["vVertex"]);
		glVertexAttribPointer(_shader["vVertex"], 3, GL_FLOAT, GL_FALSE,0,0);
		  
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * _totalIndices, &indices[0], GL_STATIC_DRAW);
		 
	glBindVertexArray(0);
	delete [] indices;
	delete [] vertices;	 
}


Grid::~Grid()
{
	//Destroy shader
	_shader.DeleteShaderProgram();

	//Destroy vao and vbo
	glDeleteBuffers(1, &_vboVerticesID);
	glDeleteBuffers(1, &_vboIndicesID);
	glDeleteVertexArrays(1, &_vaoID);
}
 
void Grid::Render(const float* MVP) 
{
	_shader.Use();				
		glUniformMatrix4fv(_shader("MVP"), 1, GL_FALSE, MVP);
		glBindVertexArray(_vaoID);
			glDrawElements(GL_LINES, _totalIndices, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	_shader.UnUse();
}



	} // namespace Renderables
} // namespace CoreFx