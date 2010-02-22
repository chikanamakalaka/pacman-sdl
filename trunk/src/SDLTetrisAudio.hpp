/*
 * SDLTetrisAudio.hpp
 *
 *  Created on: Jun 26, 2009
 *      Author: asantos
 */

#ifndef SDLTETRISAUDIO_HPP_
#define SDLTETRISAUDIO_HPP_


class SDLTetrisAudio{
public:
	typedef void(RefreshVolumeHandler)();
private:

	SignalBroker& signalbroker;

	int numchannels;
	int soundvolume;
	int musicvolume;
	Mix_Music* music;
	Mix_Chunk* newgamechunk;
	Mix_Chunk* scorechangedchunk;
	Mix_Chunk* levelchangedchunk;
	Mix_Chunk* unabletomovechunk;
	Mix_Chunk* gameoverchunk;
	Mix_Chunk* unabletorotatechunk;
	Mix_Chunk* abletorotatechunk;
	Mix_Chunk* abletomovechunk;
	Mix_Chunk* currentpiecestoppedchunk;

	std::set<int> usedchannels;
public:
	SDLTetrisAudio(SignalBroker& signalbroker):
		signalbroker(signalbroker){
		if(SDL_InitSubSystem(SDL_INIT_AUDIO) == -1){
		    // SDL Audio subsystem could not be started
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Could not init SDL_INIT_AUDIO");
			throw std::exception();
		}
		if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
		    // SDL_Mixer could not be started
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "could not initialize SDL_mixer");
			throw std::exception();
		}

		numchannels = Mix_AllocateChannels(16);

		Initialize();

		music = Mix_LoadMUS(FileSystem::MakeUsrLocalPath("/music/music.ogg").c_str());
		if(!music){
			 // music.ogg could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",Mix_GetError());
			//throw std::exception();
		}
		Mix_FadeInMusic(music, -1, 4000);

		newgamechunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/newgame.wav").c_str());
		if(!newgamechunk){
			 // ping.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",Mix_GetError());
			//throw std::exception();
		}

		levelchangedchunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/levelchanged.wav").c_str());
		if(!levelchangedchunk){
			 // levelchanged.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		scorechangedchunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/scorechanged.wav").c_str());
		if(!scorechangedchunk){
			 // scorechanged.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		unabletomovechunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/unabletomove.wav").c_str());
		if(!unabletomovechunk){
			 // unabletomove.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}
		gameoverchunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/gameover.wav").c_str());
		if(!gameoverchunk){
			 // gameoverchunk.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		unabletorotatechunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/unabletorotate.wav").c_str());
		if(!unabletorotatechunk){
			 // unabletorotate.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		abletorotatechunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/abletorotate.wav").c_str());
		if(!abletorotatechunk){
			 // abletorotate.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		abletomovechunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/abletomove.wav").c_str());
		if(!abletomovechunk){
			 // abletomove.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		currentpiecestoppedchunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath("/sounds/currentpiecestopped.wav").c_str());
		if(!currentpiecestoppedchunk){
			 // currentpiecestopped.wav could not be loaded
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
			//throw std::exception();
		}

		signalbroker.ConnectToSignal
			<TetrisLogic::LoadInitialStateHandler>
			("/logic/loadinitialstate",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->newgamechunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::ScoreChangedHandler>
			("/tetris/scorechanged",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->scorechangedchunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::LevelChangedHandler>
			("/tetris/levelchanged",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->levelchangedchunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::AbleToMoveHandler>
			("/tetrislogic/abletomove",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->abletomovechunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::UnableToMoveHandler>
			("/tetrislogic/unabletomove",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->unabletomovechunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::GameOverHandler>
			("/tetrislogic/gameover",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->gameoverchunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::AbleToRotateHandler>
			("/tetrislogic/abletorotate",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->abletorotatechunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::UnableToRotateHandler>
			("/tetrislogic/unabletorotate",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->unabletorotatechunk));

		signalbroker.ConnectToSignal
			<TetrisLogic::CurrentPieceStoppedHandler>
			("/tetrislogic/currentpiecestopped",
			boost::bind(&SDLTetrisAudio::PlayChunk, this, this->currentpiecestoppedchunk));

		signalbroker.ConnectToSignal
			<SDLTetrisAudio::RefreshVolumeHandler>
			("/tetrisaudio/refreshvolume",
			boost::bind(&SDLTetrisAudio::Initialize, this));

	}
	~SDLTetrisAudio(){
		Mix_HaltMusic();
		Mix_FreeMusic(music);

		//wait for all channels to be finished before freeing chunks
		std::set<int>::const_iterator itr = usedchannels.begin();
		for(;itr!=usedchannels.end(); itr++){
			while(Mix_Playing(*itr) != 0);
		}
		Mix_FreeMusic(music);
		Mix_FreeChunk(newgamechunk);
		Mix_FreeChunk(scorechangedchunk);
		Mix_FreeChunk(levelchangedchunk);
		Mix_FreeChunk(unabletomovechunk);
		Mix_FreeChunk(gameoverchunk);
		Mix_FreeChunk(unabletorotatechunk);
		Mix_FreeChunk(abletorotatechunk);
		Mix_FreeChunk(abletomovechunk);
		Mix_FreeChunk(currentpiecestoppedchunk);

		Mix_CloseAudio();
		/* Uninitialization */
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
	void PlayChunk(Mix_Chunk* mixchunk){
		if(mixchunk){
			int channel = Mix_PlayChannel(-1, mixchunk, 0);
			if(channel==-1) {
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
				if(Mix_Playing(-1)==numchannels){
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Halting all channels. All of them are playing. This shouldn't be happening.");

					Mix_HaltChannel(-1);
					if(Mix_Playing(-1)==numchannels){
						signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Weird.  Tried halting all channels, but they are all still playing.");
						signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", Mix_GetError());
					}else{
						PlayChunk(mixchunk);
					}
				}
			}else{
				std::stringstream ss;
				ss<<"played chunk on channel"<<channel;
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", (ss).str());
				usedchannels.insert(channel);
			}
		}
	}
	void Initialize(){
		TetrisDB tetrisdb;
		musicvolume = tetrisdb.GetVolumeValueByName("Music");
		soundvolume = tetrisdb.GetVolumeValueByName("Effects");
		for(int channel=0; channel<16; channel++){
			Mix_Volume(channel, std::min(soundvolume, MIX_MAX_VOLUME));
		}
		Mix_VolumeMusic(std::min(musicvolume, MIX_MAX_VOLUME));


	}
};

#endif /* SDLTETRISAUDIO_HPP_ */
