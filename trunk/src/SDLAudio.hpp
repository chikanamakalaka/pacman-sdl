/*
 * SDLAudio.hpp
 *
 *  Created on: Jun 26, 2009
 *      Author: asantos
 */

#ifndef SDAUDIO_HPP_
#define SDLAUDIO_HPP_


class SDLAudio{
public:
	typedef void(RefreshVolumeHandler)();
private:

	SignalBroker& signalbroker;

	int numchannels;
	int soundvolume;
	int musicvolume;

	//filename -> mix*
	std::map<std::string, Mix_Chunk*> soundchunks;
	std::map<std::string, Mix_Music*> musicchunks;

	std::map<Mix_Music*, MusicBinding> musicbindings;

	std::set<int> usedchannels;
public:
	SDLAudio(SignalBroker& signalbroker):
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

		PacmanDB pacmandb;
		std::vector<SoundBinding> soundbindings = pacmandb.GetSoundBindings();
		std::vector<MusicBinding> musicbindings = pacmandb.GetMusicBindings();

		for(std::vector<SoundBinding>::const_iterator itr = soundbindings.begin(); itr!=soundbindings.end(); itr++){
			Mix_Chunk* mixchunk;
			std::map<std::string, Mix_Chunk*>::const_iterator chunkitr = soundchunks.find(itr->GetFilename());
			if(chunkitr == soundchunks.end()){
				mixchunk = Mix_LoadWAV(FileSystem::MakeUsrLocalPath(itr->GetFilename()).c_str());
				if(!mixchunk){
					 // chunk could not be loaded
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",Mix_GetError());
					//throw std::exception();
					continue;
				}
				soundchunks.insert(std::map<std::string, Mix_Chunk*>::value_type(itr->GetFilename(), mixchunk));
			}else{
				mixchunk = chunkitr->second;
			}
			signalbroker.ConnectToSignal
				<SignalBroker::GenericHandler>
				(itr->GetEvent(),
				boost::bind(&SDLAudio::PlayChunk, this, mixchunk));
		}

		for(std::vector<MusicBinding>::const_iterator itr = musicbindings.begin(); itr!=musicbindings.end(); itr++){
			Mix_Music* music;
			std::map<std::string, Mix_Music*>::const_iterator musicitr = musicchunks.find(itr->GetFilename());
			if(musicitr != musicchunks.end()){
				music = Mix_LoadMUS(FileSystem::MakeUsrLocalPath(itr->GetFilename()).c_str());
				if(!music){
					 // music.ogg could not be loaded
					signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",Mix_GetError());
					//throw std::exception();
					continue;
				}
				musicchunks.insert(std::map<std::string, Mix_Music*>::value_type(itr->GetFilename(), music));
			}else{
				music = musicitr->second;
			}

			signalbroker.ConnectToSignal
				<SignalBroker::GenericHandler>
				(itr->GetEvent(),
				boost::bind(&SDLAudio::PlayMusic, this, music));
		}
		signalbroker.ConnectToSignal
			<SDLAudio::RefreshVolumeHandler>
			("/sdlaudio/refreshvolume",
			boost::bind(&SDLAudio::Initialize, this));

	}
	~SDLAudio(){
		Mix_HaltMusic();

		//wait for all channels to be finished before freeing chunks
		std::set<int>::const_iterator itr = usedchannels.begin();
		for(;itr!=usedchannels.end(); itr++){
			while(Mix_Playing(*itr) != 0);
		}
		for(std::map<std::string, Mix_Chunk*>::const_iterator itr = soundchunks.begin(); itr != soundchunks.end(); itr++){
			Mix_FreeChunk(itr->second);
		}
		for(std::map<std::string, Mix_Music*>::const_iterator itr = musicchunks.begin(); itr != musicchunks.end(); itr++){
			Mix_FreeMusic(itr->second);
		}
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
	void PlayMusic(Mix_Music* music){
		Mix_FadeOutMusic(4000);
		if(music){
			Mix_FadeInMusic(music, -1, 4000);
		}
	}
	void Initialize(){
		PacmanDB pacmandb;
		musicvolume = pacmandb.GetVolumeValueByName("Music");
		soundvolume = pacmandb.GetVolumeValueByName("Effects");
		for(int channel=0; channel<16; channel++){
			Mix_Volume(channel, std::min(soundvolume, MIX_MAX_VOLUME));
		}
		Mix_VolumeMusic(std::min(musicvolume, MIX_MAX_VOLUME));


	}
};

#endif /* SDLAUDIO_HPP_ */
