/*
 * util.hpp
 *
 *  Created on: May 5, 2009
 *      Author: asantos
 */


#if defined(linux)
	#ifndef GL_GLEXT_PROTOTYPES
		#define GL_GLEXT_PROTOTYPES
	#endif
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#if defined(_WIN32)
	#include <GL/GLee.h>
	#include <GL/gl.h>
#endif

#include "../math.hpp"

class ITexture:public Cloneable<ITexture>{
public:
	virtual ~ITexture(){}
	virtual void Bind()const{}
	virtual void Unload()const{}
};


class UnableToCreateSurface : public virtual std::exception{
private:
		const char* msg;
public:
	UnableToCreateSurface(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};
class UnhandledSurfaceFormat : public virtual std::exception{
public:
	UnhandledSurfaceFormat(){

	}
	char const* what()const throw(){
		return "UnhandledSurfaceFormat";
	}
};
class Vertex{
private:
	Vector4 position;
	Vector4 normal;
	Vector3 color;
	Vector2 texturecoords;
public:
	Vertex( float x=0.0f, float y=0.0f, float z=0.0f,
			float tu=0.0f, float tv=0.0f,
			float nx=0.0f, float ny=0.0f, float nz=0.0f)
	{
		position(0) = x;
		position(1) = y;
		position(2) = z;
		position(3) = 1.0f;
		normal(0) = nx;
		normal(1) = ny;
		normal(2) = nz;
		normal(3) = 1.0f;
		color(0) = 1.0f;
		color(1) = 1.0f;
		color(2) = 1.0f;
		texturecoords(0) = tu;
		texturecoords(1) = tv;

	}
	inline Vector4& GetPosition(){
		return position;
	}
	inline const Vector4& GetPosition()const{
		return position;
	}
	inline float GetX()const{
		return position(0);
	}
	inline float GetY()const{
		return position(1);
	}
	inline float GetZ()const{
		return position(2);
	}
	inline Vector4& GetNormal(){
		return normal;
	}
	inline const Vector4& GetNormal()const{
		return normal;
	}
	inline float GetNormalX()const{
		return normal(0);
	}
	inline float GetNormalY()const{
		return normal(1);
	}
	inline float GetNormalZ()const{
		return normal(2);
	}
	inline Vector3& GetColor(){
		return color;
	}
	inline const Vector3& GetColor()const{
		return color;
	}
	inline float GetColorR()const{
		return color(0);
	}
	inline float GetColorG()const{
		return color(1);
	}
	inline float GetColorB()const{
		return color(2);
	}
	inline Vector2& GetTextureCoordinates(){
		return texturecoords;
	}
	inline const Vector2& GetTextureCoordinates()const{
		return texturecoords;
	}
	inline float GetTextureU()const{
		return texturecoords(0);
	}
	inline float GetTextureV()const{
		return texturecoords(1);
	}
};

class Triangle{
private:
	std::list<Vertex> vertices;
public:
	Triangle(){
	}
	const std::list<Vertex>& GetVertices()const{
		return vertices;
	}
	void Clear(){
		vertices.empty();
	}
	void AddVertex(const Vertex& vertex){
		if(vertices.size()<3){
			vertices.push_back(vertex);
		}
	}
};

class Geometry{
private:
	bool visible;
public:
	Geometry(bool visible = true):visible(visible){}
	virtual ~Geometry(){

	}
	virtual void SetVisibility(bool visibility){
		visible = visibility;
	}
	virtual bool IsVisible()const{
		return visible;
	}
};

class TriangleStrip: public Geometry{
public:
	typedef std::list<Vertex> Vertices;
private:
	std::list<Vertex> vertices;
public:
	TriangleStrip(bool visible = true):Geometry(visible){
	}
	const std::list<Vertex>& GetVertices()const{
		return vertices;
	}
	void Clear(){
		vertices.empty();
	}
	void AddVertex(const Vertex& vertex){
		vertices.push_back(vertex);
	}
};

class IRenderable:public Cloneable<IRenderable>{
private:
	bool visible;
public:
	IRenderable(bool visible=true):visible(visible){}
	virtual ~IRenderable(){

	}
	virtual void SetVisibility(bool visibility){
		visible = visibility;
	}
	virtual bool IsVisible()const{
		return visible;
	}

	virtual void Render()const = 0;

};

class IRenderableFactory{
public:
	virtual boost::shared_ptr<IRenderable> CreateFromTriangleStrip(const TriangleStrip& trianglestrip) = 0;
};

namespace OpenGL{

class Texture:public ITexture{
private: GLuint texture;
public:
	Texture(){
		glGenTextures(1, &texture);
	}
	Texture(const Texture& texture):texture(texture.texture){
		
	}
	virtual ~Texture(){
	}
	boost::shared_ptr<ITexture> Clone()const{
		return boost::shared_ptr<ITexture>(new Texture(*this));
	}
	boost::shared_ptr<Texture> ConcreteClone()const{
		return boost::shared_ptr<Texture>(new Texture(*this));
	}
	void Bind()const{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	void Load(GLint width, GLint height, GLint numberOfColors, GLenum textureFormat, const GLvoid* pixels){
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTexImage2D(GL_TEXTURE_2D, 0, numberOfColors, width, height, 0,
						  textureFormat, GL_UNSIGNED_BYTE, pixels);
	}
	void Unload(){
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &texture);

	}
};


#ifdef GL_VERSION_1_5
class VBO: public IRenderable{
private:
	GLuint colorBuffer;
	GLuint normalBuffer;
	GLuint vertexBuffer;
	GLuint textureCoordBuffer;
	GLuint numvertices;
public:
	VBO(GLuint numvertices, GLuint colorBuffer, GLuint normalBuffer, GLuint vertexBuffer, GLuint textureCoordBuffer):
		numvertices(numvertices),
		colorBuffer(colorBuffer),
		normalBuffer(normalBuffer),
		vertexBuffer(vertexBuffer),
		textureCoordBuffer(textureCoordBuffer)
		{

		}

	boost::shared_ptr<IRenderable> Clone()const{
		return boost::shared_ptr<IRenderable>(new VBO(*this));
	}
	boost::shared_ptr<VBO> ConcreteClone()const{
		return boost::shared_ptr<VBO>(new VBO(*this));
	}
	virtual void Render()const{
		if(this->IsVisible()){
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			/*if(colorBuffer > 0){
				glEnableClientState(GL_COLOR_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
				glColorPointerEXT(3, GL_UNSIGNED_BYTE, 0, 0, 0);
			}

			if(normalBuffer > 0){
				glEnableClientState(GL_NORMAL_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
				glNormalPointerEXT(3, GL_FLOAT, 0, 0);
			}*/

			if(textureCoordBuffer > 0){
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, textureCoordBuffer);
				glTexCoordPointerEXT(2, GL_FLOAT, 0, 0, 0);
			}

			if(vertexBuffer > 0){
				glEnableClientState(GL_VERTEX_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glVertexPointer(3, GL_FLOAT, 0, 0);

				glDrawArrays(GL_TRIANGLE_STRIP, 0, numvertices);
			}


			if(colorBuffer > 0){
				glDisableClientState(GL_COLOR_ARRAY);
			}
			if(normalBuffer > 0){
				glDisableClientState(GL_NORMAL_ARRAY);
			}
			if(vertexBuffer > 0){
				glDisableClientState(GL_VERTEX_ARRAY);
			}
			if(textureCoordBuffer > 0){
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
};


class VBOFactory{
public:
	VBOFactory(){}
	boost::shared_ptr<IRenderable> CreateFromTriangleStrip(const TriangleStrip& trianglestrip){
		const std::list<Vertex>& vertices = trianglestrip.GetVertices();

		GLuint colorBuffer;
		GLuint colorBufferSize;

		GLuint normalBuffer;
		GLuint normalBufferSize;

		GLuint vertexBuffer;
		GLuint vertexBufferSize;

		GLuint textureCoordBuffer;
		GLuint textureCoordBufferSize;

		GLuint numvertices;

		vertexBufferSize = sizeof(GLfloat)*3*vertices.size();
		GLfloat* vertexData = new GLfloat[vertices.size()*3];

		textureCoordBuffer = sizeof(GLfloat)*2*vertices.size();
		GLfloat* textureCoordData = new GLfloat[vertices.size()*2];


		//glGenBuffers(1, &colorBuffer);
		//glGenBuffers(1, &normalBuffer);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &textureCoordBuffer);



		std::list<Vertex>::const_iterator itr = vertices.begin();
		for(unsigned int i=0, j=0; itr!=vertices.end(); i+=3, j+=2, itr++){
			vertexData[i]=itr->GetX();
			vertexData[i+1]=itr->GetY();
			vertexData[i+2]=itr->GetZ();

			textureCoordData[j]=itr->GetTextureU();
			textureCoordData[j+1]=itr->GetTextureV();
		}


		/*if(colorBufferSize > 0){
			this->colorBufferSize = colorBufferSize;
			glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
			glBufferData(GL_ARRAY_BUFFER, colorBufferSize, colorData, usageMode);
		}
		if(normalBufferSize > 0){
			this->normalBufferSize = normalBufferSize;
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			glBufferData(GL_ARRAY_BUFFER, normalBufferSize, normalData, usageMode);
		}*/

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexData, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textureCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, textureCoordBufferSize, textureCoordData, GL_STATIC_DRAW);


		boost::shared_ptr<VBO> vbo(new VBO(vertices.size(), 0, 0, vertexBuffer, textureCoordBuffer));
		delete[] vertexData;
		delete[] textureCoordData;
		return vbo;
	}
};
#endif //defined(GL_VERSION_1_5)

class DisplayList: public IRenderable{
private:
	GLuint displaylist;
public:
	DisplayList(GLuint displaylist, bool visible):IRenderable(visible), displaylist(displaylist){

	}
	boost::shared_ptr<IRenderable> Clone()const{
		return boost::shared_ptr<IRenderable>(new DisplayList(*this));
	}
	boost::shared_ptr<DisplayList> ConcreteClone()const{
		return boost::shared_ptr<DisplayList>(new DisplayList(*this));
	}
	virtual void Render()const {
		if(this->IsVisible()){
			glCallList(displaylist);
		}
	}
};
class DisplayListFactory: public IRenderableFactory{
private:
	typedef std::list<GLuint> GLDisplayLists;
	typedef std::list<boost::shared_ptr<DisplayList> > DisplayLists;
	GLDisplayLists gldisplaylists;
	DisplayLists displaylists;
public:
	DisplayListFactory(){}
	virtual ~DisplayListFactory(){
		{
			GLDisplayLists::const_iterator itr = gldisplaylists.begin();
			for(;itr!=gldisplaylists.end();itr++){
				glDeleteLists(*itr, 1);
			}
		}
	}
	boost::shared_ptr<IRenderable> CreateFromTriangleStrip(const TriangleStrip& trianglestrip){
		GLuint gldisplaylist = glGenLists(1);

		glNewList(gldisplaylist, GL_COMPILE);
		glBegin(GL_TRIANGLE_STRIP);
		TriangleStrip::Vertices vertices = trianglestrip.GetVertices();
		TriangleStrip::Vertices::const_iterator itr = vertices.begin();
		for(;itr!=vertices.end();itr++){
			glTexCoord2f(itr->GetTextureU(), itr->GetTextureV());
			glVertex3f(itr->GetX(), itr->GetY(), itr->GetZ());
		}
		glEnd();
		glEndList();
		boost::shared_ptr<DisplayList> displaylist(new DisplayList(gldisplaylist, trianglestrip.IsVisible()));
		displaylists.push_back(displaylist);
		return displaylist;
	}
};


} //namespace OpenGL

