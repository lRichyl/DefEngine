#pragma once
#include "defines.h"
#include <iostream>
#include "AL/al.h"
#include "AL/alc.h"

namespace def{
	inline void print_error_ALC(int error, int line, CHR_STR_CON file){
		switch(error){
			case ALC_INVALID_DEVICE:{
				printf("OPENAL ERROR CODE : AL_INVALID_DEVICE\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case ALC_INVALID_CONTEXT:{
				printf("OPENAL ERROR CODE : AL_INVALID_CONTEXT\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case ALC_INVALID_ENUM:{
				printf("OPENAL ERROR CODE : AL_INVALID_ENUM\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case ALC_INVALID_VALUE:{
				printf("OPENAL ERROR CODE : AL_INVALID_VALUE\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case ALC_OUT_OF_MEMORY:{
				printf("OPENAL ERROR CODE : AL_OUT_OF_MEMORY\nLine: %d\nFile: %s", line, file);\
				break;
			}
			default:
				printf("OPENAL ERROR CODE : UNKNOWN\nLine: %d\nFile: %s", line, file);\
		}
	}

	inline void print_error_AL(int error, int line, CHR_STR_CON file){
		switch(error){
			case AL_INVALID_NAME:{
				printf("OPENAL ERROR CODE : AL_INVALID_NAME\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case AL_INVALID_ENUM:{
				printf("OPENAL ERROR CODE : AL_INVALID_ENUM\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case AL_INVALID_VALUE:{
				printf("OPENAL ERROR CODE : AL_INVALID_VALUE\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case AL_INVALID_OPERATION:{
				printf("OPENAL ERROR CODE : AL_INVALID_OPERATION\nLine: %d\nFile: %s", line, file);\
				break;
			}
			case AL_OUT_OF_MEMORY:{
				printf("OPENAL ERROR CODE : AL_OUT_OF_MEMORY\nLine: %d\nFile: %s", line, file);\
				break;
			}
			default:
				printf("OPENAL ERROR CODE : UNKNOWN\nLine: %d\nFile: %s", line, file);\
		}
	}

	#define ALCall(function) function;\
		{\
			int error = alGetError();\
			if(error != AL_NO_ERROR){\
				print_error_AL(error, __LINE__, __FILE__);\
				exit(1);\
			}\
		}
		
	#define ALCCall(function) function;\
		{\
			int error = alGetError();\
			if(error != AL_NO_ERROR){\
				print_error_ALC(error, __LINE__, __FILE__);\
				exit(1);\
			}\
		}
		
	// TODO: We should play the sounds and music in a different thread.
	struct SoundSource{
		unsigned int id = 0;
	};

	struct AudioInfo{
		int length = 0;
		int sample_rate = 0;
		int format = 0;
		int channels = 0;
		
		CHR_STR_CON filename;
	};
	struct Sound{
		unsigned int id = 0;
		SoundSource source;
		AudioInfo info;
		
		bool loops = false;
		bool is_playing = false;
	};

	static constexpr int NUM_BUFFERS = 4;
	static constexpr int NUM_SAMPLES_PER_BUFFER = 16000;

	// Do not use this.
	struct Music{
		unsigned int id = 0;
		SoundSource source;
		AudioInfo info;
		unsigned int buffers[NUM_BUFFERS];
		short *buffer = new short[NUM_SAMPLES_PER_BUFFER];
		void *v;
		
		bool loops = false;
		bool is_playing = false;
	};


	Sound make_sound(CHR_STR_CON filename);
	Music make_music(CHR_STR_CON filename);
	void play_music(Music *music);
	void stop_music(Music *music);
	void play_sound(Sound *sound);
	void stop_sound(Sound *sound);
	void print_sound_data(Sound *sound);
	void update_audio_streams();
}