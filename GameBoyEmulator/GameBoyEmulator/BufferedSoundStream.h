#pragma once
#include <SFML/Audio.hpp>



class BufferedSoundStream : public sf::SoundStream {

private:

	const int BufferSize = 10000;

	std::vector<sf::Int16> samples;
	std::vector<sf::Int16> buffer;
	std::size_t m_currentSample;

	virtual bool onGetData(Chunk& data);
	virtual void onSeek(sf::Time timeOffset);

public:

	std::vector<float> displayBuffer;
	void Push(sf::Int16 sample);
	void Start();

};

