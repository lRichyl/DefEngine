#pragma once
#include <iostream>
#include "AL/al.h"
#include "AL/alc.h"

inline void print_error_ALC(int error, int line, const char *file){
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

inline void print_error_AL(int error, int line, const char *file){
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
	

struct SoundSource{
	unsigned int id = 0;
};


struct Sound{
	unsigned int id = 0;
	SoundSource source;
	int length = 0;
	int sample_rate = 0;
	int format = 0;
	int channels = 0;
	
	bool loops = false;
	bool is_playing = false;
};


Sound make_sound(const char *filename);
void play_sound(Sound *sound);
void stop_sound(Sound *sound);
void print_sound_data(Sound *sound);
