    /*

    Copyright (C) 1999-2000 Stefan Westerfeld
                            stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#ifndef SIMPLESOUNDSERVER_IMPL_H
#define SIMPLESOUNDSERVER_IMPL_H

#include "soundserver.h"
#include "artsflow.h"
#include <list>

namespace Arts {

class AttachedProducer {
protected:
	ByteSoundProducer _sender;
	ByteStreamToAudio _receiver;

public:
	AttachedProducer(ByteSoundProducer sender, ByteStreamToAudio receiver);
	ByteSoundProducer sender();
	ByteStreamToAudio receiver();
};

class SimpleSoundServer_impl : virtual public SimpleSoundServer_skel,
										public TimeNotify
{
protected:
	Synth_PLAY playSound;
	Synth_MULTI_ADD addLeft, addRight;
	std::list<Synth_PLAY_WAV> activeWavs;
	std::list<ByteStreamToAudio> activeConverters;
	std::list<AttachedProducer *> activeProducers;
	StereoEffectStack _outstack;
	long asCount;

public:
	SimpleSoundServer_impl();
	~SimpleSoundServer_impl();

	void notifyTime();

	// streaming audio
	float minStreamBufferTime();
	float serverBufferTime();
	void attach(ByteSoundProducer bsp);
	void detach(ByteSoundProducer bsp);

	// simple soundserver interface
	long play(const std::string& s);

	// kmedia2
	PlayObject createPlayObject(const std::string& filename);
	StereoEffectStack outstack();
	Object createObject(const std::string& name);
};

};

#endif /* SIMPLESOUNDSERVER_IMPL_H */
