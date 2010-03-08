/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISCREDITSGUICONTROLLER_HPP_
#define TETRISCREDITSGUICONTROLLER_HPP_

#include "XMLGuiChanMenuController.hpp"

class TetrisCreditsGUIController:public XMLGuiChanMenuController{
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

	gcn::Label* continuelabel;
	MenuItemMouseListener* continuemouselistener;

public:
	TetrisCreditsGUIController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "CreditsMenu", "Credits"),
		signalbroker(signalbroker),
		menuname("CreditsMenu")
		{

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisCreditsGUIController::TetrisCreditsGUIController():this->signalnamespace==" + this->signalnamespace);
	}
	virtual ~TetrisCreditsGUIController(){

		if(IsMenuInitialized()){
			delete font;
			delete hoverfont;

			delete continuemouselistener;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris CreditsGUI");

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		try{
		imageLoader = new gcn::OpenGLSDLImageLoader();
		gcn::Image::setImageLoader(imageLoader);
		graphics = new gcn::OpenGLGraphics(640,480);
		input = new gcn::SDLInput();

		font = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/fixedfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		hoverfont = new gcn::ImageFont(FileSystem::MakeUsrLocalPath("/images/hoverfont.png"), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		gcn::Widget::setGlobalFont(font);
		}catch(gcn::Exception e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				"gcn::Exception:" + e.getMessage());
		}

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing Credits XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/creditsgui.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		continuelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("continue"));


		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

		continuemouselistener = new MenuItemMouseListener(menuname, "quit", continuelabel, signalbroker);
		continuelabel->addMouseListener(continuemouselistener);


		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris HighScoresGUI");

		MenuInitialized();
	}
	void MenuItemPressed(const std::string& name, gcn::Label* label){

	}
	void MenuItemReleased(const std::string& name, gcn::Label* label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label* label){
		signalbroker.InvokeSignal
			<GamestateController::StateChangeHandler>
			("/tetrisgamestatecontroller/"+name);
	}
	void MenuItemEntered(const std::string& name, gcn::Label* label){
		label->setFont(hoverfont);
	}
	void MenuItemExited(const std::string& name, gcn::Label* label){
		label->setFont(font);
	}
	void KeyBindingKeyPressed(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}
	void KeyBindingKeyReleased(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}

};

#endif /* TETRISHIGHSCORESGUICONTROLLER_HPP_ */
