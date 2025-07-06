#pragma once

#include "../constants/Constants.h"

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

class SoundManager {
private:
	sf::SoundBuffer moveSoundBuffer, winSoundBuffer, selectSoundBuffer;
	sf::Sound moveSound, winSound, selectSound;
	sf::Music gameMusic;

	bool winSoundPlayed;

public:
	SoundManager() : moveSoundBuffer(AUDIO_PATH + "move.mp3"), winSoundBuffer(AUDIO_PATH + "win.wav"), selectSoundBuffer(AUDIO_PATH + "select.wav"), gameMusic(AUDIO_PATH + "music.mp3"),
  				     moveSound(moveSoundBuffer), winSound(winSoundBuffer), selectSound(selectSoundBuffer) {
		gameMusic.setLooping(true);
		gameMusic.setVolume(5);
		winSoundPlayed = false;
	}

	void playStartSounds() {
		selectSound.play();
		gameMusic.play();
	}

	void playMoveSound() {
		moveSound.play();
	}

	void playWinSound() {
		if (!winSoundPlayed) {
			winSound.play();
		}
		winSoundPlayed = true;
	}
};