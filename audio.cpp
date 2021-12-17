#include "audio.h"
#include "AL/al.h"
#include <assert.h>
#include <cmath>

#include "stb_vorbis.c"

void print_sound_data(Sound *sound){
	printf("%d\n", sound->id);
	printf("%d\n", sound->source.id);
	printf("%d\n", sound->length);
	printf("%d\n", sound->sample_rate);
	printf("%d\n", sound->format);
	printf("%d\n", sound->channels);
}

Sound make_sound(const char *filename){
	Sound sound;
	short *buffer;
	sound.length = stb_vorbis_decode_filename(filename, &sound.channels, &sound.sample_rate, &buffer);
	if(!sound.length){
		printf("File could not be loaded\n");
		return sound;
	}
	
	int format = -1;
	if(sound.channels == 1){
		sound.format = AL_FORMAT_MONO16;
	}else{
		sound.format = AL_FORMAT_STEREO16;
	}
	// print_sound_data(&sound);

	ALCall(alGenBuffers(1, &sound.id));
	ALCall(alBufferData(sound.id, sound.format, buffer, sound.length * (sizeof(short)), sound.sample_rate));
	
	ALCall(alGenSources(1, &sound.source.id));
	ALCall(alSourcei(sound.source.id, AL_BUFFER, sound.id));
	ALCall(alSourcei(sound.source.id, AL_LOOPING, sound.loops));
	ALCall(alSource3f(sound.source.id, AL_POSITION, 0,0,0));
	ALCall(alSourcef(sound.source.id, AL_GAIN, 1.0f));
	// ALCall(alSourcePlay(sound.source.id));

	free(buffer);
	return sound;
	
}



void play_sound(Sound *sound){
	int state;
	// printf("%d\n", sound->source.id);
	// print_sound_data(sound);
	if(!sound->is_playing){
		ALCall(alGetSourcei(sound->source.id, AL_SOURCE_STATE, &state));
		if(state == AL_STOPPED){
			sound->is_playing = false;
			ALCall(alSource3f(sound->source.id, AL_POSITION, 0,0,0));
		}
		
		if(!sound->is_playing){
			ALCall(alSourcePlay(sound->source.id));
			sound->is_playing = true;
		}
		
	}
	
	
}

void stop_sound(Sound *sound){
	if(sound->is_playing){
		ALCall(alSourceStop(sound->source.id));
		sound->is_playing = false;
	}
}
