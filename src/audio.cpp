#ifdef __cplusplus
	extern "C" {
#endif

#include "soloud.h"
#include "soloud_wav.h"

SoLoud::Soloud Soloud;
SoLoud::Wav paddleWav;
SoLoud::Wav blockWav;
SoLoud::Wav frameWav;

void startAudio() {
	Soloud.init();
}

void loadAudio(){
	paddleWav.load("app0:/pong_paddle.ogg");
	blockWav.load("app0:/pong_score.ogg");
	frameWav.load("app0:/pong_wall.ogg");
}

void paddleSound() {
	Soloud.play(paddleWav);
}

void blockSound() {
	Soloud.play(blockWav);
}

void frameSound() {
	Soloud.play(frameWav);
}

#ifdef __cplusplus
}
#endif