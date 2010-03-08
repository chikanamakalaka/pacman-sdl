/*
 * TetrisMainMenuController.hpp
 *
 *  Created on: Aug 26, 2009
 *      Author: asantos
 */

#ifndef TETRISHIGHSCORESGUICONTROLLER_HPP_
#define TETRISHIGHSCORESGUICONTROLLER_HPP_

#include "XMLGuiChanMenuController.hpp"

class TetrisHighScoresGUIController:public XMLGuiChanMenuController{
public:
	typedef void(ReloadHighScoresHandler)();
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

	std::vector<gcn::Label*> highscores;

	gcn::Label* continuelabel;
	MenuItemMouseListener* continuemouselistener;

public:
	TetrisHighScoresGUIController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "HighScoresMenu", "HighScores"),
		signalbroker(signalbroker),
		menuname("HighScoresMenu")
		{

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisHighScoresGUIController::TetrisHighScoresGUIController():this->signalnamespace==" + this->signalnamespace);

		signalbroker.ConnectToSignal
		<TetrisHighScoresGUIController::ReloadHighScoresHandler>
		(	"/highscores/reload",
			boost::bind(&TetrisHighScoresGUIController::ReloadHighScores, this));
	}
	virtual ~TetrisHighScoresGUIController(){
		if(IsMenuInitialized()){
			delete font;
			delete hoverfont;

			delete continuemouselistener;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris HighScoresGUI");

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
				std::string("gcn::Exception:") + e.getMessage());
		}

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing HighScores XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/highscoresgui.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");
		highscores.resize(10);
		highscores[0] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore0"));
		highscores[1] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore1"));
		highscores[2] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore2"));
		highscores[3] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore3"));
		highscores[4] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore4"));
		highscores[5] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore5"));
		highscores[6] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore6"));
		highscores[7] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore7"));
		highscores[8] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore8"));
		highscores[9] = dynamic_cast<gcn::Label*>(xmlgui->getWidget("highscore9"));
		continuelabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("continue"));


		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");

		continuemouselistener = new MenuItemMouseListener(menuname, "quit", continuelabel, signalbroker);
		continuelabel->addMouseListener(continuemouselistener);

		std::string highscoresstr;
		std::stringstream ss;
		TetrisDB tetrisdb;
		std::list<TetrisDB::Score> highscores = tetrisdb.HighScores();
		std::list<TetrisDB::Score>::const_iterator itr = highscores.begin();

		if(itr!=highscores.end()){
			for(int i=0;itr!=highscores.end();itr++,i++){
				this->highscores[i]->setCaption((boost::format("%1% %|10t|%2%") % itr->GetName() % itr->GetScore()).str());
			}
		}


		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris HighScoresGUI");

		MenuInitialized();
	}
	void ReloadHighScores(){

		std::string highscoresstr;
		std::stringstream ss;
		TetrisDB tetrisdb;
		std::list<TetrisDB::Score> highscores = tetrisdb.HighScores();
		std::list<TetrisDB::Score>::const_iterator itr = highscores.begin();

		if(itr!=highscores.end()){
			for(int i=0;itr!=highscores.end();itr++,i++){
				this->highscores[i]->setCaption((boost::format("%1% %|10t|%2%") % itr->GetName() % itr->GetScore()).str());
			}
		}
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
