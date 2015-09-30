#include <stdlib.h>
#include "MakeMidiSound.h"

//Structure
struct Joints
{
	float x;
	float y;
	float z;
};

string ZoneSeperate(Joints input,float horizontal,string s1,string s2,
	string s3,string s4,string s5,string s6,string s7, string s8){
	if(input.y>horizontal){
		if(input.x<-400){
			return s1;
		}else if(input.x<-200){
			return s2;
		}else if(input.x<0){
			return s3;
		}else if(input.x<200){
			return s4;
		}else{
			return "0";
		}
	}else{
		if(input.x<-400){
			return s5;
		}else if(input.x<-200){
			return s6;
		}else if(input.x<0){
			return s7;
		}else if(input.x<200){
			return s8;
		}else{
			return "0";
		}
	}
}


void Play(int Bass,int channel, int velocity){
	if(Bass >= 1 && Bass <= 12){//Major Chords
		playMusic(channel, 59 + Bass, velocity);
		playMusic(channel, 63 + Bass, velocity);
		playMusic(channel, 66 + Bass, velocity);
	}else if (Bass >= 13 && Bass <= 24){//Minor Chords
		Bass = Bass - 12;
		playMusic(channel, 59 + Bass, velocity);
		playMusic(channel, 62 + Bass, velocity);
		playMusic(channel, 66 + Bass, velocity);
	}else if (Bass >= 25 && Bass <= 36){//7 Chords
		Bass = Bass - 24;
		playMusic(channel, 59 + Bass, velocity);
		playMusic(channel, 63 + Bass, velocity);
		playMusic(channel, 66 + Bass, velocity);
		playMusic(channel, 70 + Bass, velocity);
	}else if (Bass >= 37 && Bass <= 48){//m7 Chords
		Bass = Bass - 36;
		playMusic(channel, 59 + Bass, velocity);
		playMusic(channel, 62 + Bass, velocity);
		playMusic(channel, 66 + Bass, velocity);
		playMusic(channel, 69 + Bass, velocity);
	}else if (Bass >= 49 && Bass <= 60){//sus2 Chords
		Bass = Bass - 48;
		playMusic(channel, 59 + Bass, velocity);
		playMusic(channel, 61 + Bass, velocity);
		playMusic(channel, 66 + Bass, velocity);
	}else if (Bass >= 61 && Bass <= 72){//sus4 Chords
		Bass = Bass - 60;
		playMusic(channel, 59 + Bass, velocity);
		playMusic(channel, 64 + Bass, velocity);
		playMusic(channel, 66 + Bass, velocity);
	}
}	

void PlayBass(int Bass,int channel, int velocity){//Bass
		playMusic(channel, 23 + Bass, velocity);
}	
