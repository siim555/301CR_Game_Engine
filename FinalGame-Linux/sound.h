#pragma once

#include <SFML/Audio.hpp> //audio system


class Sound{
public:
    Sound(){
        
    }
    Sound(std::string filename){
        pBuffer.loadFromFile(filename);
        pSound.setBuffer(pBuffer);
        
    }
    bool setLoop(bool state){
        pSound.setLoop(state);
    }
    void setVolume(float volume){
        pSound.setVolume(volume);
    }
    void play(){
        pSound.play();
    }
private:
    sf::SoundBuffer pBuffer;
    sf::Sound pSound;   
    
    
};
