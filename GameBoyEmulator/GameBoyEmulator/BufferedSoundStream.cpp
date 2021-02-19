#include "BufferedSoundStream.h"

bool BufferedSoundStream::onGetData(Chunk & data) {

	if (buffer.size() == 0) {
		return false;
	}
	displayBuffer.clear();
	samples.resize(buffer.size());
	memcpy(&samples[0], &buffer[0], buffer.size());
	for (size_t i = 0; i < samples.size(); i++) {
		displayBuffer.push_back(samples[i]);
	}
	buffer.clear();

	data.samples = &samples[0];
	data.sampleCount = samples.size();

	return true;
}

void BufferedSoundStream::onSeek(sf::Time timeOffset) {
}

void BufferedSoundStream::Push(sf::Int16 sample) {

	buffer.push_back(sample);

	if (buffer.size() > BufferSize &&
		getStatus() == sf::SoundStream::Stopped) {

		//samples.resize(buffer.size());
		//memcpy(&samples[0], &buffer[0], buffer.size());
		//buffer.clear();
		play();
	}
}

void BufferedSoundStream::Start() {

	initialize(1, 4410);
}
