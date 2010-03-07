/*
 * TetrisGameGUIController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISGAMEGUICONTROLLER_HPP_
#define TETRISGAMEGUICONTROLLER_HPP_

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"
class TetrisGameGUIController:public SignalSubscriber{
private:
	SignalBroker& signalbroker;

	gcn::SDLInput* input;
	gcn::OpenGLGraphics* graphics;
	gcn::OpenGLSDLImageLoader* imageLoader;

	XmlGui *xmlgui;
	gcn::Gui* gui;
	gcn::ImageFont* font;

	gcn::Label* score;

	gcn::Label* level;

	bool guiinitialized;

public:
	TetrisGameGUIController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Tetris", "TetrisGameGuiController"),
		signalbroker(signalbroker),
		xmlgui(new XmlGui()),
		gui(new gcn::Gui()),
		guiinitialized(false)
		{

		signalbroker.ConnectToSignal
		<SceneGraphController::CreatedSceneGraphHandler>
		(	"/scenegraphcontroller/createdscenegraph",
			boost::bind(&TetrisGameGUIController::CreateTetrisGUI, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&TetrisGameGUIController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<TetrisLogic::ScoreChangedHandler>
		( 	"/tetris/scorechanged",
			boost::bind(&TetrisGameGUIController::ScoreChanged, this, _1));


		SignalSubscriber::ConnectToSignal
		<TetrisLogic::LevelChangedHandler>
		( 	"/tetris/levelchanged",
			boost::bind(&TetrisGameGUIController::LevelChanged, this, _1));

	}
	virtual ~TetrisGameGUIController(){
		delete xmlgui;
		delete gui;

		if(guiinitialized){
			delete font;

			delete input;
			delete graphics;
			delete imageLoader;
		}
	}
protected:
	virtual void CreateTetrisGUI(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		if(name == "Tetris"){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading TetrisGUI");

			SDL_EnableUNICODE(1);
			SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

			imageLoader = new gcn::OpenGLSDLImageLoader();
			gcn::Image::setImageLoader(imageLoader);
			graphics = new gcn::OpenGLGraphics(640,480);
			input = new gcn::SDLInput();

			font = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/fixedfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
			gcn::Widget::setGlobalFont(font);

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing TetrisGUI XML");

			xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/tetrisgui.xml"));
			gui->setGraphics(graphics);
			gui->setInput(input);
			gui->setTop(xmlgui->getWidget("top"));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

			score = dynamic_cast<gcn::Label*>(xmlgui->getWidget("score"));
			level = dynamic_cast<gcn::Label*>(xmlgui->getWidget("level"));

			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded TetrisGUI");
			
			guiinitialized=true;

		}
	}
	void Logic(long dt, long t){
		if(guiinitialized){
			gui->logic();
		}
	}
	void ScoreChanged(int score){
		this->score->setCaption(boost::lexical_cast<std::string>(score));
	}
	void LevelChanged(int level){
		this->level->setCaption(boost::lexical_cast<std::string>(level));
	}


};

#endif /* TETRISGAMEGUICONTROLLER_HPP_ */
