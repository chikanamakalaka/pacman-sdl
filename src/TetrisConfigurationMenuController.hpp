/*
 * TetrisConfigurationMenuController.hpp
 *
 *  Created on: Jan 20, 2010
 *      Author: asantos
 */

#ifndef TETRISCONFIGURATIONMENUCONTROLLER_HPP_
#define TETRISCONFIGURATIONMENUCONTROLLER_HPP_

#include <boost/bimap.hpp>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include "xmlguichan/xmlgui.h"
class TetrisConfigurationMenuController:public SignalSubscriber{
private:
	class MenuItemMouseListener : public gcn::MouseListener
	{
	public:
		typedef void(MenuItemPressed)(const std::string&, gcn::Label& label);
		typedef void(MenuItemReleased)(const std::string&, gcn::Label& label);
		typedef void(MenuItemClicked)(const std::string&, gcn::Label& label);
		typedef void(MenuItemEntered)(const std::string&, gcn::Label& label);
		typedef void(MenuItemExited)(const std::string&, gcn::Label& label);
	private:
		const std::string name;
		gcn::Label& label;
		SignalBroker& signalbroker;
	public:
		MenuItemMouseListener(const std::string& name, gcn::Label& label, SignalBroker& signalbroker):
			name(name),
			label(label),
			signalbroker(signalbroker){}
		void mousePressed(gcn::MouseEvent& mouseEvent){
			std::stringstream ss;
			ss << "Mouse pressed " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemPressed>
				( "/configurationmenu/item/pressed", name, label);
		}
		void mouseReleased(gcn::MouseEvent& mouseEvent){
			std::stringstream ss;
			ss << "Mouse released " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemReleased>
				( "/configurationmenu/item/released", name, label);
		}
		void mouseClicked(gcn::MouseEvent& mouseEvent) {
			std::stringstream ss;
			ss << "Mouse clicked " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemClicked>
				( "/configurationmenu/item/clicked", name, label);
		}
		void mouseEntered(gcn::MouseEvent& mouseEvent){
			std::stringstream ss;
			ss << "Mouse entered " << name << ":" << mouseEvent.getX() << " " << mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemEntered>
				( "/configurationmenu/item/entered", name, label);
		}
		void mouseExited(gcn::MouseEvent& mouseEvent) {
			std::stringstream ss;
			ss << "Mouse exited " << name << ":" << mouseEvent.getX() << " "<< mouseEvent.getY();
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

			signalbroker.InvokeSignal
				<MenuItemExited>
				( "/configurationmenu/item/exited", name, label);
		}
	};

	class KeyboardBindingKeyListener : public gcn::KeyListener
	{
		public:
		typedef void(KeyPressed)(gcn::TextField& label, const std::string& name, int keyvalue);
		typedef void(KeyReleased)(gcn::TextField& label, const std::string& name, int keyvalue);
	private:
		const std::string name;
		gcn::TextField& textfield;
		SignalBroker& signalbroker;
	public:
		KeyboardBindingKeyListener(const std::string& name, gcn::TextField& textfield, SignalBroker& signalbroker):
			name(name),
			textfield(textfield),
			signalbroker(signalbroker){}

	    void keyPressed(gcn::KeyEvent& keyEvent)
	    {
	    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				"Key pressed: " + keyEvent.getKey().getValue());

	        signalbroker.InvokeSignal
			<KeyPressed>
			( "/configurationmenu/textfield/keypressed", textfield, name, keyEvent.getKey().getValue());
	    }

	    void keyReleased(gcn::KeyEvent& keyEvent)
	    {
	    	std::stringstream ss;
	    	ss << "Key released: " << keyEvent.getKey().getValue();
	    	signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
				ss.str());

	        signalbroker.InvokeSignal
			<KeyReleased>
			( "/configurationmenu/textfield/keyreleased", textfield, name, keyEvent.getKey().getValue());
	    }
	};

	SignalBroker& signalbroker;

	gcn::SDLInput* input;
	gcn::OpenGLGraphics* graphics;
	gcn::OpenGLSDLImageLoader* imageLoader;

	XmlGui *xmlgui;
	gcn::Gui* gui;
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

	bool menuinitialized;

public:
	TetrisConfigurationMenuController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "Configuration", "TetrisConfigurationMenuController"),
		signalbroker(signalbroker),
		xmlgui(new XmlGui()),
		gui(new gcn::Gui()),
		menuinitialized(false)
		{
		signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			"TetrisMainMenuController::TetrisConfigurationMenuController():this->signalnamespace==" + this->signalnamespace);


		signalbroker.ConnectToSignal
		<SceneGraphController::CreatedSceneGraphHandler>
		(	"/scenegraphcontroller/createdscenegraph",
			boost::bind(&TetrisConfigurationMenuController::CreateConfigurationMenu, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<SDLEventsView::SDLEventHandler>
		( 	"/sdlevent/event",
			boost::bind(&TetrisConfigurationMenuController::SDLEventHandler, this, _1));


		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		( 	"/clock/tick",
			boost::bind(&TetrisConfigurationMenuController::Logic, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemPressed>
		(	"/configurationmenu/item/pressed",
			boost::bind(&TetrisConfigurationMenuController::MenuItemPressed, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/configurationmenu/item/released",
			boost::bind(&TetrisConfigurationMenuController::MenuItemReleased, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/configurationmenu/item/clicked",
			boost::bind(&TetrisConfigurationMenuController::MenuItemClicked, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/configurationmenu/item/entered",
			boost::bind(&TetrisConfigurationMenuController::MenuItemEntered, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<MenuItemMouseListener::MenuItemReleased>
		(	"/configurationmenu/item/exited",
			boost::bind(&TetrisConfigurationMenuController::MenuItemExited, this, _1, _2));

		SignalSubscriber::ConnectToSignal
		<KeyboardBindingKeyListener::KeyPressed>
		(	"/configurationmenu/textfield/keypressed",
			boost::bind(&TetrisConfigurationMenuController::KeyBindingKeyPressed, this, _1, _2, _3));

		SignalSubscriber::ConnectToSignal
		<KeyboardBindingKeyListener::KeyReleased>
		(	"/configurationmenu/textfield/keyreleased",
			boost::bind(&TetrisConfigurationMenuController::KeyBindingKeyReleased, this, _1, _2, _3));

	}
	virtual ~TetrisConfigurationMenuController(){
		delete xmlgui;
		delete gui;

		if(menuinitialized){
			delete font;
			delete hoverfont;

			delete input;
			delete graphics;
			delete imageLoader;


			delete leftkeyboardbindingkeylistener;
			delete rightkeyboardbindingkeylistener;
			delete downkeyboardbindingkeylistener;
			delete rotatekeyboardbindingkeylistener;

			delete backmouselistener;
			delete applymouselistener;
		}
	}
protected:
	virtual void CreateConfigurationMenu(const std::string& name, boost::shared_ptr<SceneGraph> scenegraph){
		if(name == "Configuration"){
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

			leftkeyboardbindingkeylistener = new KeyboardBindingKeyListener("left", *lefttextfield, signalbroker);
			rightkeyboardbindingkeylistener = new KeyboardBindingKeyListener("right", *righttextfield, signalbroker);
			downkeyboardbindingkeylistener = new KeyboardBindingKeyListener("down", *downtextfield, signalbroker);
			rotatekeyboardbindingkeylistener = new KeyboardBindingKeyListener("rotate", *rotatetextfield, signalbroker);


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

			applymouselistener = new MenuItemMouseListener("apply", *applylabel, signalbroker);
			backmouselistener = new MenuItemMouseListener("back", *backlabel, signalbroker);

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Connecting Listeners");


			applylabel->addMouseListener(applymouselistener);
			backlabel->addMouseListener(backmouselistener);


			boost::shared_ptr<IRenderable> guichangui(new GuiChanGui(gui));
			scenegraph->GetRoot().AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(guichangui)));

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Loaded Tetris Main Menu");
			
			menuinitialized=true;

		}
	}
	void SDLEventHandler(SDL_Event event){
		if(menuinitialized){
			input->pushInput(event);
		}
	}
	void Logic(long dt, long t){
		if(menuinitialized){
			gui->logic();
		}
	}
	void MenuItemPressed(const std::string& name, gcn::Label& label){


	}
	void MenuItemReleased(const std::string& name, gcn::Label& label){

	}
	void MenuItemClicked(const std::string& name, gcn::Label& label){
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
	void MenuItemEntered(const std::string& name, gcn::Label& label){
		label.setFont(hoverfont);
	}
	void MenuItemExited(const std::string& name, gcn::Label& label){
		label.setFont(font);
	}
	void KeyBindingKeyPressed(gcn::TextField& textfield, const std::string& name, int keyvalue){
		TetrisDB tetrisdb;
		textfield.setText(tetrisdb.GetKeyNameByValue(keyvalue));

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
	void KeyBindingKeyReleased(gcn::TextField& textfield, const std::string& name, int keyvalue){

	}

};

#endif /* TETRISCONFIGURATIONMENUCONTROLLER_HPP_ */
