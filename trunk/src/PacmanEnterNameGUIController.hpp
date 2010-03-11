/*
 * PacmanMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef PACMANENTERNAMEGUICONTROLLER_HPP_
#define PACMANENTERNAMEGUICONTROLLER_HPP_

#include <boost/format.hpp>

#include "XMLGuiChanMenuController.hpp"
#include "PacmanDB.hpp"

class PacmanEnterNameGUIController:public XMLGuiChanMenuController{
public:
	typedef void(RefreshSharedScoreHandler)(int);
private:
	SignalBroker& signalbroker;
	const std::string menuname;
	int highscore;
	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

	gcn::Label* score;
	gcn::TextField* name;

	gcn::Label* continuelabel;
	MenuItemMouseListener* continuemouselistener;

public:
	PacmanEnterNameGUIController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "EnterNameMenu", "EnterName"),
		signalbroker(signalbroker),
		menuname("EnterNameMenu")
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"PacmanEnterNameGUIController::PacmanEnterNameGUIController():this->signalnamespace==" + this->signalnamespace);

		signalbroker.ConnectToSignal
		<PacmanEnterNameGUIController::RefreshSharedScoreHandler>
		(	"/pacmanenternamegui/refreshsharedscore",
			boost::bind(&PacmanEnterNameGUIController::RefreshSharedScore, this, _1));
			
	}
	virtual ~PacmanEnterNameGUIController(){
		if(IsMenuInitialized()){
			delete font;
			delete hoverfont;

			delete continuemouselistener;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Pacman EnterNameGUI");

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
				std::string("gcn::Exception:")+e.getMessage());
		}

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing HighScores XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/enternamegui.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		score = dynamic_cast<gcn::Label*>(xmlgui->getWidget("score"));
		this->name = dynamic_cast<gcn::TextField*>(xmlgui->getWidget("name"));
		continuelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("continue"));

		continuemouselistener = new MenuItemMouseListener(menuname, "enteredname", continuelabel, signalbroker);

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

		continuelabel->addMouseListener(continuemouselistener);


		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Pacman EnterNameGUI");

		MenuInitialized();
	}
	void RefreshSharedScore(int score){
		highscore = score;
		this->score->setCaption(boost::lexical_cast<std::string>(score));
	}
	void MenuItemPressed(const std::string& name, gcn::Label* label){


	}
	void MenuItemReleased(const std::string& name, gcn::Label* label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label* label){
		if(name == "enteredname"){
			PacmanDB pacmandb;
			pacmandb.AddScore(highscore, this->name->getText());

		}
		
		signalbroker.InvokeSignal
			<GamestateController::StateChangeHandler>
			("/pacmangamestatecontroller/"+name);

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

#endif /* PACMANENTERNAMEGUICONTROLLER_HPP_ */
