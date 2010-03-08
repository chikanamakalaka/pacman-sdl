/*
 * TetrisConfigurationMenuController.hpp
 *
 *  Created on: Jan 20, 2010
 *      Author: asantos
 */

#ifndef TETRISCONFIGURATIONMENUCONTROLLER_HPP_
#define TETRISCONFIGURATIONMENUCONTROLLER_HPP_

#include <boost/bimap.hpp>
#include "XMLGuiChanMenuController.hpp"
class TetrisConfigurationMenuController:public XMLGuiChanMenuController{
private:
	SignalBroker& signalbroker;
	const std::string menuname;

	gcn::ImageFont* font;
	gcn::ImageFont* hoverfont;

	gcn::Slider* musicvolumeslider;
	gcn::Slider* effectsvolumeslider;

	gcn::TextField* lefttextfield;
	KeyboardBindingKeyListener* leftkeyboardbindingkeylistener;
	int leftkeyvalue;

	gcn::TextField* righttextfield;
	KeyboardBindingKeyListener* rightkeyboardbindingkeylistener;
	int rightkeyvalue;

	gcn::TextField* downtextfield;
	KeyboardBindingKeyListener* downkeyboardbindingkeylistener;
	int downkeyvalue;

	gcn::TextField* rotatetextfield;
	KeyboardBindingKeyListener* rotatekeyboardbindingkeylistener;
	int rotatekeyvalue;


	gcn::Label* applylabel;
	MenuItemMouseListener* applymouselistener;

	gcn::Label* backlabel;
	MenuItemMouseListener* backmouselistener;

public:
	TetrisConfigurationMenuController(SignalBroker& signalbroker):
		XMLGuiChanMenuController(signalbroker, "ConfigurationMenu", "Configuration"),
		signalbroker(signalbroker),
		menuname("ConfigurationMenu")
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisConfigurationMenuController::TetrisConfigurationMenuController():this->signalnamespace==" + this->signalnamespace);

	}
	virtual ~TetrisConfigurationMenuController(){

		if(IsMenuInitialized()){
			delete font;
			delete hoverfont;

			delete leftkeyboardbindingkeylistener;
			delete rightkeyboardbindingkeylistener;
			delete downkeyboardbindingkeylistener;
			delete rotatekeyboardbindingkeylistener;

			delete backmouselistener;
			delete applymouselistener;
		}
	}
protected:
	virtual void CreateMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loading Tetris ConfigurationMenu");

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

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Parsing ConfigurationMenu XML");

		xmlgui->parse(FileSystem::MakeUsrLocalPath("/menus/configurationmenu.xml"));
		gui->setGraphics(graphics);
		gui->setInput(input);
		gui->setTop(xmlgui->getWidget("top"));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Finding GuiChan widgets");

		TetrisDB tetrisdb;

		musicvolumeslider = dynamic_cast<gcn::Slider*>(xmlgui->getWidget("musicvolumeslider"));
		effectsvolumeslider = dynamic_cast<gcn::Slider*>(xmlgui->getWidget("soundeffectvolumeslider"));

		musicvolumeslider->setValue(tetrisdb.GetVolumeValueByName("Music"));
		effectsvolumeslider->setValue(tetrisdb.GetVolumeValueByName("Effects"));

		lefttextfield = dynamic_cast<gcn::TextField*>(xmlgui->getWidget("lefttextfield"));
		righttextfield = dynamic_cast<gcn::TextField*>(xmlgui->getWidget("righttextfield"));
		downtextfield = dynamic_cast<gcn::TextField*>(xmlgui->getWidget("downtextfield"));
		rotatetextfield = dynamic_cast<gcn::TextField*>(xmlgui->getWidget("rotatetextfield"));

		leftkeyboardbindingkeylistener = new KeyboardBindingKeyListener(menuname, "left", lefttextfield, signalbroker);
		rightkeyboardbindingkeylistener = new KeyboardBindingKeyListener(menuname, "right", righttextfield, signalbroker);
		downkeyboardbindingkeylistener = new KeyboardBindingKeyListener(menuname, "down", downtextfield, signalbroker);
		rotatekeyboardbindingkeylistener = new KeyboardBindingKeyListener(menuname, "rotate", rotatetextfield, signalbroker);


		lefttextfield->addKeyListener(leftkeyboardbindingkeylistener);
		righttextfield->addKeyListener(rightkeyboardbindingkeylistener);
		downtextfield->addKeyListener(downkeyboardbindingkeylistener);
		rotatetextfield->addKeyListener(rotatekeyboardbindingkeylistener);

		boost::bimap<int, std::string> keybindings = tetrisdb.GetKeyBindings();

		std::string lefttext;
		{
			boost::bimap<int, std::string>::right_const_iterator itr = keybindings.right.find("Left");
			lefttext = tetrisdb.GetKeyNameByValue(itr->second);
			leftkeyvalue = itr->second;
		}
		std::string righttext;
		{
			boost::bimap<int, std::string>::right_const_iterator itr = keybindings.right.find("Right");
			righttext = tetrisdb.GetKeyNameByValue(itr->second);
			rightkeyvalue = itr->second;
		}
		std::string downtext;
		{
			boost::bimap<int, std::string>::right_const_iterator itr = keybindings.right.find("Down");
			downtext = tetrisdb.GetKeyNameByValue(itr->second);
			downkeyvalue = itr->second;
		}
		std::string rotatetext;
		{
			boost::bimap<int, std::string>::right_const_iterator itr = keybindings.right.find("Rotate");
			rotatetext = tetrisdb.GetKeyNameByValue(itr->second);
			rotatekeyvalue = itr->second;
		}

		lefttextfield->setText(lefttext);
		righttextfield->setText(righttext);
		downtextfield->setText(downtext);
		rotatetextfield->setText(rotatetext);

		applylabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("applylabel"));
		backlabel = dynamic_cast<gcn::Label*>(xmlgui->getWidget("backlabel"));

		applymouselistener = new MenuItemMouseListener(menuname, "apply", applylabel, signalbroker);
		backmouselistener = new MenuItemMouseListener(menuname, "back", backlabel, signalbroker);

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");


		applylabel->addMouseListener(applymouselistener);
		backlabel->addMouseListener(backmouselistener);


		boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
		scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris Main Menu");

		MenuInitialized();
	}
	void MenuItemPressed(const std::string& name, gcn::Label* label){

	}
	void MenuItemReleased(const std::string& name, gcn::Label* label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label* label){
		if(name == "apply"){
			TetrisDB tetrisdb;
			tetrisdb.SetVolume("Music", static_cast<int>(this->musicvolumeslider->getValue()));
			tetrisdb.SetVolume("Effects", static_cast<int>(this->effectsvolumeslider->getValue()));

			tetrisdb.SetKeyBinding("Left", this->leftkeyvalue);
			tetrisdb.SetKeyBinding("Right", this->rightkeyvalue);
			tetrisdb.SetKeyBinding("Down", this->downkeyvalue);
			tetrisdb.SetKeyBinding("Rotate", this->rotatekeyvalue);


			signalbroker.InvokeSignal
				<SDLTetrisAudio::RefreshVolumeHandler>
				("/tetrisaudio/refreshvolume");

			signalbroker.InvokeSignal
				<TetrisInputView::RefreshKeyBindingsHandler>
				("/tetrisinput/refreshkeybindings");
		}

		signalbroker.InvokeSignal
			<GamestateController::StateChangeHandler>
			("/tetrisgamestatecontroller/quit");
	}
	void MenuItemEntered(const std::string& name, gcn::Label* label){
		label->setFont(hoverfont);
	}
	void MenuItemExited(const std::string& name, gcn::Label* label){
		label->setFont(font);
	}
	void KeyBindingKeyPressed(gcn::TextField* textfield, const std::string& name, int keyvalue){
		TetrisDB tetrisdb;
		textfield->setText(tetrisdb.GetKeyNameByValue(keyvalue));

		if(name=="left"){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "set left value");

			leftkeyvalue = keyvalue;
		}else if(name == "right"){
			rightkeyvalue = keyvalue;
		}else if(name == "down"){
			downkeyvalue = keyvalue;
		}else if(name == "rotate"){
			rotatekeyvalue = keyvalue;
		}
	}
	void KeyBindingKeyReleased(gcn::TextField* textfield, const std::string& name, int keyvalue){

	}

};

#endif /* TETRISCONFIGURATIONMENUCONTROLLER_HPP_ */
