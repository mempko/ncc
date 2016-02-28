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

#ifndef NCC_OPENALSOUND_H
#define NCC_OPENALSOUND_H

#ifndef NO_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#endif 
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <boost/utility.hpp>

namespace ncc {
namespace oal
{
    ///Uses the AL library to manage and play sounds.
    ///
    ///This manager manages the AL library. To play a sound you first 
    ///register it. Once it is registered, you can play it over an over again 
    ///with the play method. The play method returns an integer which you can 
    ///pass to stop if the sound is in a loop. The current limitation on this 
    ///sound manager is that it can only load uncompressed wav files at the 
    ///moment. Functionality such as Ogg Vorbis should be planned for the 
    ///future.
    class manager : boost::noncopyable
    {
        public:

            ///Loads a sound from a file and stores it.
            ///
            ///A sound must be registered before it can be used. This function 
            ///registers an uncompressed wav file from a file. You must specify
            ///a name for the file which will be used to refer to the sound. 
            ///@param file The file path of the wav file.
            ///@param name The name which the sound should be called. This name
            ///is used in the play method.
            ///@return True on success and false on failure.
            bool register_sound(const std::string& file, const std::string& name);

            ///Plays a sound with the specified volume.
            ///
            ///This method can be used to play a sound which was registered. The 
            ///name parameter is the same name which was used to register the sound.
            ///@param name The name of the sound to play.
            ///@param volume The volume which the sound should be played at. 
            ///@return An integer which can be used by the stop method to stop a 
            ///looping sound.
            int play(std::string name,double volume, bool loop);

            ///Stops a sound.
            ///
            ///If the sound is still playing or is in a loop, this method stops it.
            ///@param The int parameter which play returned.
            bool stop(int source);

            ///Stops and unregisters all sounds.
            void flush() {clean_up();}; 

        public:
            manager();
            virtual ~manager();
        private:
            int play(int buffer,double volume, bool loop);
            bool load_wav(const std::string& fileName);
            void clean_up();
        private:
#ifdef NO_OPENAL
            typedef int ALuint;
#endif

            typedef std::vector<ALuint> buffer_library;
            buffer_library buffers;

            typedef std::vector<ALuint> source_library;
            source_library sources;
            typedef std::map< std::string, int > sound_map;
            sound_map sounds; //maps a sound name to a buffer
    };
}//namespace oal
}//namespace ncc
#endif

