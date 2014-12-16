class Drawer

{
public:
	Drawer();
	~Drawer();
	void Draw(ShaderLoader & mShaderLoader);
	void DrawInit(Mesh* aMash);

private:
	Mesh* mMash;
	unsigned int mVboVertices;
	unsigned int mVboNormals;
	unsigned int mIboElements;
};

Drawer::Drawer()
{

}

Drawer::~Drawer()
{
}

void Drawer::DrawInit(Mesh* aMash)
{
	mMash = aMash;
    if (aMash->vertices.size() > 0) {
      glGenBuffers(1, &mVboVertices);
      glBindBuffer(GL_ARRAY_BUFFER, mVboVertices);
      glBufferData(GL_ARRAY_BUFFER, aMash->vertices.size() * sizeof(aMash->vertices[0]),
           aMash->vertices.data(), GL_STATIC_DRAW);
    }
    
    if (aMash->normals.size() > 0) {
      glGenBuffers(1, &mVboNormals);
      glBindBuffer(GL_ARRAY_BUFFER, mVboNormals);
      glBufferData(GL_ARRAY_BUFFER, aMash->normals.size() * sizeof(aMash->normals[0]),
           aMash->normals.data(), GL_STATIC_DRAW);
    }
    
    if (aMash->elements.size() > 0) {
      glGenBuffers(1, &mIboElements);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIboElements);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, aMash->elements.size() * sizeof(aMash->elements[0]),
           aMash->elements.data(), GL_STATIC_DRAW);
    }
  
 
}
void Drawer::Draw(ShaderLoader &mShaderLoader)
{
	glClearColor(0.45, 0.45, 0.45, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
  glUseProgram(mShaderLoader.program);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIboElements);
 
  if (mVboVertices != 0) {
	glBindBuffer(GL_ARRAY_BUFFER, mVboVertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(float)*3, 0 );
 
    glBindBuffer(GL_ARRAY_BUFFER, mVboNormals);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(float)*3, 0 );
	glUniformMatrix4fv(mShaderLoader.getUniform_m(), 1, GL_FALSE, glm::value_ptr(mMash->object2world));
    /* Transform normal vectors with transpose of inverse of upper left
       3x3 model matrix (ex-gl_NormalMatrix): */
    glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(mMash->object2world)));
	glUniformMatrix3fv(mShaderLoader.getUniform_uniform_m_3x3_inv_transp(), 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));

	glDrawElements(GL_TRIANGLES, mMash->elements.size()*sizeof(float) , GL_UNSIGNED_INT, 0);
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    /* Apply object's transformation matrix */
	
    
    /* Push each element in buffer_vertices to the vertex shader */
   /* if (this->ibo_elements != 0) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
      int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
    }*/
 
    
}