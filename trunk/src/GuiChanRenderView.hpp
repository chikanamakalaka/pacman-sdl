/*
 * CEGUIRenderView.hpp
 *
 *  Created on: Aug 24, 2009
 *      Author: asantos
 */

#ifndef GUICHANRENDERVIEW_HPP_
#define GUICHANRENDERVIEW_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

class GuiChanGui: public IRenderable{
private:
	gcn::Gui* gui;
public:
	GuiChanGui(gcn::Gui* gui, bool visible = true):IRenderable(visible), gui(gui){

	}
	boost::shared_ptr<IRenderable> Clone()const{
		return boost::shared_ptr<IRenderable>(new GuiChanGui(*this));
	}
	boost::shared_ptr<GuiChanGui> ConcreteClone()const{
		return boost::shared_ptr<GuiChanGui>(new GuiChanGui(*this));
	}
	virtual void Render()const {
		if(gui && gui->getTop()){
			gui->draw();
		}
	}
};

#endif /* CEGUIRENDERVIEW_HPP_ */
