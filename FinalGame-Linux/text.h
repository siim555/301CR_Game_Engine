#pragma once

#include <SFML/Graphics.hpp>
#include <string>


class OnScreenText{
public:
    OnScreenText(){
        
    }
    OnScreenText(std::string filename, int size){
        pFont.loadFromFile("UbuntuMono-R.ttf");
        pText.setFont(pFont);
        pText.setCharacterSize(size);
        pText.setFillColor(sf::Color::Red);
        
    }
    void setString(std::string textstring){
        pText.setString(textstring);
    }
    void drawText(sf::RenderWindow &window){
        window.draw(pText);
    }
private:
    sf::Font pFont;
    sf::Text pText;
};
