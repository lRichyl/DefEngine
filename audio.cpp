#include "audio.h"
#include "AL/al.h"
#include <assert.h>
#include <cmath>
#include <vector>

#include "stb_vorbis.c"

namespace def {
	void print_sound_data(Sound *sound){
		printf("%d\n", sound->id);
		printf("%d\n", sound->source.id);
		printf("%d\n", sound->info.length);
		printf("%d\n", sound->info.sample_rate);
		printf("%d\n", sound->info.format);
		printf("%d\n", sound->info.channels);
	}

	Sound make_sound(const char *filename){
		Sound sound;
		sound.info.filename = filename;
		short *buffer;
		sound.info.length = stb_vorbis_decode_filename(filename, &sound.info.channels, &sound.info.sample_rate, &buffer);
		if(!sound.info.length){
			printf("File could not be loaded\n");
			return sound;
		}
		
		// int format = -1;
		if(sound.info.channels == 1){
			sound.info.format = AL_FORMAT_MONO16;
		}else{
			sound.info.format = AL_FORMAT_STEREO16;
		}
		// print_sound_data(&sound);

		ALCall(alGenBuffers(1, &sound.id));
		ALCall(alBufferData(sound.id, sound.info.format, buffer, sound.info.length * sizeof(short) * sound.info.channels, sound.info.sample_rate));
		
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
		ALCall(alGetSourcei(sound->source.id, AL_SOURCE_STATE, &state));
		if(state == AL_STOPPED || state == AL_INITIAL){
			ALCall(alSourcePlay(sound->source.id));
			sound->is_playing = true;
		}
		
		
	}

	void stop_sound(Sound *sound){
		int state;
		ALCall(alGetSourcei(sound->source.id, AL_SOURCE_STATE, &state));
		if(sound->is_playing){
			ALCall(alSourceStop(sound->source.id));
			sound->is_playing = false;
		}
	}

	Music make_music(const char *filename){
		Music music;
		music.info.filename = filename;
		int error;
		
		stb_vorbis *v = stb_vorbis_open_filename(filename, &error, NULL);
		music.v = v;
		music.info.channels = v->channels;
		music.info.sample_rate = v->sample_rate;
		
		if(music.info.channels == 1){
			music.info.format = AL_FORMAT_MONO16;
		}else{
			music.info.format = AL_FORMAT_STEREO16;
		}
		// print_sound_data(&sound);
		
		
		ALCall(alGenBuffers(NUM_BUFFERS, music.buffers));
		
		
		ALCall(alGenSources(1, &music.source.id));
		ALCall(alSourcei(music.source.id, AL_LOOPING, music.loops));
		ALCall(alSource3f(music.source.id, AL_POSITION, 0,0,0));
		ALCall(alSourcef(music.source.id, AL_GAIN, 0.5f));
		
		
		// for(int i = 0; i < NUM_BUFFERS; i++){
			// short buffer[NUM_SAMPLES_PER_BUFFER];
			// int samples_taken = stb_vorbis_get_samples_short_interleaved(v, music.info.channels, music.buffer, NUM_SAMPLES_PER_BUFFER);
			// ALCall(alBufferData(music.buffers[i], music.info.format, music.buffer, samples_taken * (sizeof(short) * music.info.channels), music.info.sample_rate));
		// }
		// ALCall(alSourceQueueBuffers(music.source.id, NUM_BUFFERS, music.buffers));
		// ALCall(alSourcePlay(music.source.id));
		
		// printf("%d, %d", music.info.channels, music.info.sample_rate);
		// free(music.v);
		return music;
	}

	static void update_stream(Music *music){
		int error;
		
		stb_vorbis *v = (stb_vorbis *)music->v;
		
		int buffers_processed = 0;
		ALCall(alGetSourcei(music->source.id, AL_BUFFERS_PROCESSED, &buffers_processed));
		
		for(int i = 0; i < buffers_processed; i++){
			unsigned int buffer_id = 0;
			ALCall(alSourceUnqueueBuffers(music->source.id, 1, &buffer_id));
			
			// short buffer[NUM_SAMPLES_PER_BUFFER];
			int samples_taken = stb_vorbis_get_samples_short_interleaved(v, music->info.channels, music->buffer, NUM_SAMPLES_PER_BUFFER);
			ALCall(alBufferData(buffer_id, music->info.format, music->buffer, samples_taken * (sizeof(short) * music->info.channels), music->info.sample_rate));
			ALCall(alSourceQueueBuffers(music->source.id, 1, &buffer_id));
		}
	}

	static std::vector<Music*> streams_to_update;


	static void add_stream(Music *music){
		streams_to_update.push_back(music);
	}

	void update_audio_streams(){
		
		for (auto it = streams_to_update.begin(); it != streams_to_update.end(); ) {
			Music *music = *it;
			int state;
			ALCall(alGetSourcei(music->source.id, AL_SOURCE_STATE, &state));
			if(state == AL_PLAYING){
				update_stream(*it);
				++it;
			}else if(state == AL_STOPPED){
				int buffers_processed = 0;
				ALCall(alGetSourcei(music->source.id, AL_BUFFERS_PROCESSED, &buffers_processed));
				std::vector<unsigned int> b(buffers_processed);
				ALCall(alSourceUnqueueBuffers(music->source.id, buffers_processed, &b[0]));
				
				free((stb_vorbis*)music->v);
				int error;
				stb_vorbis *v = stb_vorbis_open_filename(music->info.filename, &error, NULL);
				music->v = v;
				// ALCall(alSourceStop(music->source.id));
				music->is_playing = false;
				streams_to_update.erase(it);
			}
		}
	}

	void play_music(Music *music){
		int state;
		ALCall(alGetSourcei(music->source.id, AL_SOURCE_STATE, &state));
			if(state == AL_STOPPED || state == AL_INITIAL){
				for(int i = 0; i < NUM_BUFFERS; i++){
				// short buffer[NUM_SAMPLES_PER_BUFFER];
				int samples_taken = stb_vorbis_get_samples_short_interleaved((stb_vorbis*)music->v, music->info.channels, music->buffer, NUM_SAMPLES_PER_BUFFER);
				ALCall(alBufferData(music->buffers[i], music->info.format, music->buffer, samples_taken * (sizeof(short) * music->info.channels), music->info.sample_rate));
			}
			ALCall(alSourceQueueBuffers(music->source.id, NUM_BUFFERS, music->buffers));
			
			ALCall(alSourcePlay(music->source.id));
			music->is_playing = true;
			
			add_stream(music);
			
		}
		
	}

	void stop_music(Music *music){
		int state;
		ALCall(alGetSourcei(music->source.id, AL_SOURCE_STATE, &state));
		if(state == AL_PLAYING){
			ALCall(alSourceStop(music->source.id));
			// music->is_playing = false;
			
			// int error;
			// free((stb_vorbis*)music->v);
			// stb_vorbis *v = stb_vorbis_open_filename(music->info.filename, &error, NULL);
			// music->v = v;
		}
	}
}
