/*
 * Copyright (C) 2016  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give 
 * permission to link the code of portions of this program with the 
 * Botan library under certain conditions as described in each 
 * individual source file, and distribute linked combinations 
 * including the two.
 *
 * You must obey the GNU General Public License in all respects for 
 * all of the code used other than Botan. If you modify file(s) with 
 * this exception, you may extend this exception to your version of the 
 * file(s), but you are not obligated to do so. If you do not wish to do 
 * so, delete this exception statement from your version. If you delete 
 * this exception statement from all source files in the program, then 
 * also delete it here.
 */

#include "sound/oal_manager.h"

#ifndef NO_OPENAL
ALfloat sourcePos[] = { 0.0, 0.0 , 0.0};
ALfloat sourceVel[] = { 0.0, 0.0 , 0.0};

ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };


// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };
#else
    typedef int ALuint;
#endif

namespace ncc {
namespace oal {
manager::manager()
{
#ifndef NO_OPENAL
	alutInit(0, NULL);
         alGetError();
		 
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
#endif
}
manager::~manager()
{
#ifndef NO_OPENAL
	clean_up();
	alutExit();
#endif
}

void delete_source(ALuint sound) 
{ 
#ifndef NO_OPENAL
    alDeleteSources(1, &sound); 
#endif
}
void delete_buffer(ALuint sound) 
{ 
#ifndef NO_OPENAL
    alDeleteBuffers(1, &sound); 
#endif
}
 
void manager::clean_up()
{
#ifndef NO_OPENAL

	std::for_each(sources.begin(), sources.end(), delete_source);
	std::for_each(buffers.begin(), buffers.end(), delete_buffer);
	
	sources.clear();
	buffers.clear();
	sounds.clear();
#endif
}

bool manager::load_wav(const std::string& file_name)
{
#ifndef NO_OPENAL
    ALuint new_buffer;	
    
    new_buffer = alutCreateBufferFromFile(file_name.c_str());
    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;
	 
    buffers.push_back(new_buffer);
    return AL_TRUE;	
#else
    return true;
#endif
}

bool manager::register_sound(const std::string& file, const std::string& name)
{
#ifndef NO_OPENAL
	if(sounds.find(name) != sounds.end())
		return false;
	
	if(!load_wav(file))
		return false;
	
	sounds[name] = buffers.size() - 1;
	 
	return true;
#else
    return true;
#endif
}

int manager::play(std::string name, double volume, bool loop)
{
#ifndef NO_OPENAL
	sound_map::iterator sound = sounds.find(name);
	
	if(sound == sounds.end())
		return -1;
	
	return play(sound->second, volume, loop);
#else
    return 0;
#endif
}
bool manager::stop(int source)
{
#ifndef NO_OPENAL
	ALint playing;
	alGetSourcei(sources[source], AL_SOURCE_STATE, &playing);
	
	if(playing == AL_PLAYING)
	 {
		alSourceStop(sources[source]);
		return true;
	 }
	return false;
#else
    return true;
#endif
}


int manager::play(int buffer, double volume, bool loop)
{
#ifndef NO_OPENAL
	ALint playing;
	ALuint new_source;
	
	bool found_source = false;
	int source_index = 0;
	source_library::iterator source = sources.begin();
	source_library::iterator end = sources.end();
	for(; source != end; ++source, ++source_index)
	 {
		alGetSourcei(*source,AL_SOURCE_STATE, &playing);
		if(playing != AL_PLAYING)
		 {
			new_source = *source;
			found_source = true;
			break;
		 }
	 }
	
	if(!found_source)
	 {
		alGenSources(1, &new_source);
		
		if (alGetError() != AL_NO_ERROR)
			return -1;
		sources.push_back(new_source);
	 }
	
	alSourcei (new_source, AL_BUFFER,   buffers[buffer]);
	
	if(!found_source)
	 {
		alSourcef (new_source, AL_PITCH,    1.0          );
		alSourcef (new_source, AL_GAIN,     volume          );
		alSourcefv(new_source, AL_POSITION, sourcePos);
		alSourcefv(new_source, AL_VELOCITY, sourceVel);
		loop ?  alSourcei (new_source, AL_LOOPING,  AL_TRUE) : alSourcei (new_source, AL_LOOPING,  AL_FALSE);
	 }
	
	alSourcePlay(new_source);
	
	return source_index;
#else
    return 0;
#endif
}

}//namespace oal
}//namespace ncc

