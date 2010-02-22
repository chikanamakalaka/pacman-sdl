/*
 * TextureFactory.hpp
 *
 *  Created on: May 6, 2009
 *      Author: asantos
 */

#include <boost/ptr_container/ptr_list.hpp>
#include <SDL/SDL_image.h>


template<typename T>
class TextureFactory{
private:
	SignalBroker& signalbroker;

	typedef std::list<boost::shared_ptr<ITexture> > Textures;
	Textures textures;
public:
	TextureFactory(SignalBroker& signalbroker):signalbroker(signalbroker){}
	~TextureFactory(){
		{
			Textures::iterator itr = textures.begin();
			for(;itr!=textures.end();itr++){
				(*itr)->Unload();
			}
		}
	}
	boost::shared_ptr<T> LoadFromSDLSurface(const SDL_Surface* surface){
		if(surface) {
			GLenum textureFormat;
			GLint numberOfColors = surface->format->BytesPerPixel;
			if(numberOfColors == 4){
					if(surface->format->Rmask == 0x000000ff)
						textureFormat = GL_RGBA;
					else
						textureFormat = GL_BGRA;
			} else if(numberOfColors == 3){
					if (surface->format->Rmask == 0x000000ff)
						textureFormat = GL_RGB;
					else
						textureFormat = GL_BGR;
			} else {
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "warning: the image is not truecolor..  this will probably break.");
					throw UnhandledSurfaceFormat();
			}
			boost::shared_ptr<T> texture(new T());
			texture->Load(surface->w, surface->h, numberOfColors,
					  textureFormat, surface->pixels );
			textures.push_back(texture);
			return texture;
		}else{
			throw "LoadFromSDLSurface: No surface given.";
		}

	}
	boost::shared_ptr<T> LoadFromFilePath(const std::string& filepath){
		SDL_Surface *surface;

		if((surface = IMG_Load(filepath.c_str()))){
			return LoadFromSDLSurface(surface);
		}else{
			throw UnableToCreateSurface(filepath.c_str());
		}
	}
};
