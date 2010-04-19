/*
 * util.hpp
 *
 *  Created on: May 5, 2009
 *      Author: asantos
 */
#include <cmath>

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

namespace OpenGL{

class VertexData{
public:
	float x, y, z;
	float u, v;
	float r, g, b;
	float nx, ny, nz;
	float padding[5]; //pad to 64 byte boundary

	VertexData():x(0), y(0), z(0), u(0), v(0), r(0), g(0), b(0), nx(0), ny(0), nz(0){
	}
	VertexData(float x, float y, float z, float u, float v,
			float r, float g, float b,
			float nx, float ny, float nz):
			x(x), y(y), z(z), u(u), v(v), r(r), g(g), b(b), nx(nx), ny(ny), nz(nz){
	}
};

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
	GLuint numvertices;
	GLuint vertexDataBuffer;
public:
	VBO(GLuint numvertices, GLuint vertexDataBuffer):
		numvertices(numvertices),
		vertexDataBuffer(vertexDataBuffer)
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
			//glEnableClientState(GL_COLOR_ARRAY);
			//glEnableClientState(GL_NORMAL_ARRAY);
			glClientActiveTexture(GL_TEXTURE0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);

			glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuffer);


			glVertexPointer(3, GL_FLOAT, sizeof(VertexData), 0);
			glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData), (void*)(sizeof(float)*3));
			//glColorPointerEXT(3, GL_UNSIGNED_BYTE, 0, 0, 0);
			//glNormalPointerEXT(3, GL_FLOAT, 0, 0);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, numvertices);

			//glDisableClientState(GL_COLOR_ARRAY);
			//glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);


			//glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	void UpdateTextureCoords(const std::list<VertexData>& vertexData){
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuffer);
		VertexData* data = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		std::list<VertexData>::const_iterator itr = vertexData.begin();
		for(unsigned int i=0; i<numvertices && itr != vertexData.end();i++, itr++){
			data[i].u = itr->u;
			data[i].v = itr->v;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
};


class VBOFactory{
public:
	VBOFactory(){}
	boost::shared_ptr<IRenderable> CreateFromTriangleStrip(const TriangleStrip& trianglestrip){
		const std::list<Vertex>& vertices = trianglestrip.GetVertices();

		GLuint vertexDataSize = sizeof(VertexData)*vertices.size();
		VertexData* vertexData = new VertexData[vertices.size()];

		std::list<Vertex>::const_iterator itr = vertices.begin();
		for(unsigned int i=0; itr!=vertices.end(); i++, itr++){
			vertexData[i]=VertexData(itr->GetX(), itr->GetY(), itr->GetZ(), itr->GetTextureU(), itr->GetTextureV(), itr->GetColorR(), itr->GetColorG(), itr->GetColorB(), itr->GetNormalX(), itr->GetNormalY(), itr->GetNormalZ());
		}
		GLuint vertexDataBuffer;
		glGenBuffers(1, &vertexDataBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_DYNAMIC_DRAW);

		boost::shared_ptr<VBO> vbo(new VBO(vertices.size(), vertexDataBuffer));
		delete[] vertexData;
		return vbo;
	}
};
class VBOTextureAnimation:public IAnimation{
private:
	State state;
	float elapsed;
	std::map<float, std::vector<TextureAnimationKey> > keys;
	std::string name;
	bool loop;
public:
	VBOTextureAnimation(const TextureAnimation& textureanimation):
		state(textureanimation.GetState()),
		elapsed(0.0f),
		keys(textureanimation.GetTextureAnimationKeys()),
		name(textureanimation.GetName()),
		loop(textureanimation.Loops()){}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){
		if(state == Playing){
			elapsed+=dtf;
			RenderableProperty& renderableproperty = scenenode.GetSceneNodeProperty<RenderableProperty>("renderable");
			IRenderable& renderable = renderableproperty.GetRenderable();
			VBO& vbo = dynamic_cast<VBO&>(renderable);
			if(&vbo){
				std::list<VertexData> vertexData;
				//populate vertexData
				//find key
				//if has keys
				std::map<float, std::vector<TextureAnimationKey> >::const_iterator key;
				std::map<float, std::vector<TextureAnimationKey> >::const_iterator last = keys.end();
				if(keys.size() > 0){
					last--;
					if(tf > last->first && loop){
						//if looped animation and elapsed is past the end, loop the elapsed around to the start
						elapsed = std::fmod(elapsed, last->first);
					}
					//if only one key, return that key
					if(keys.size() == 1){
						key = keys.begin();
					}
					//if before earliest time, return earliest key
					else if(elapsed < keys.begin()->first){
						key = keys.begin();
					}
					//if after latest time, return last key or loop
					else if(elapsed > last->first){
						key = last;
					}else{
						//else between two keys
						std::map<float, std::vector<TextureAnimationKey> >::const_iterator itr = keys.begin();
						//find key immediately before
						for(;itr!=keys.end(); itr++){
							if(itr->first > elapsed){
								key = itr;
								break;
							}
						}
					}
					if(key==keys.end()){
						key = last;
					}
					for(std::vector<TextureAnimationKey>::const_iterator itr = key->second.begin(); itr != key->second.end(); itr++){
						vertexData.push_back(VertexData(0,0,0, itr->GetTextureCoordinates()(0), itr->GetTextureCoordinates()(1), 0,0,0, 0,0,0));
					}
					vbo.UpdateTextureCoords(vertexData);
				}
			}
		}
	}
	const std::string GetType()const{
		return "textureanimation";
	}
	const std::string& GetName()const{
		return name;
	}
	State GetState()const{
		return state;
	}
	virtual void Play(){
		state = Playing;
	}
	virtual void Pause(){
		state = Paused;
	}
	virtual void Stop(){
		state = Stopped;
		elapsed = 0.0f;
	}
};

class VBOTextureAnimationFactory{
private:
	std::list<boost::shared_ptr<VBOTextureAnimation> > vbotextureanimations;
public:
	boost::shared_ptr<VBOTextureAnimation> CreateFromTextureAnimation(const TextureAnimation& textureanimation){
		boost::shared_ptr<VBOTextureAnimation> vbotextureanimation(new VBOTextureAnimation(textureanimation));
		vbotextureanimations.push_back(vbotextureanimation);
		return vbotextureanimation;
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

