#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class GameObject{
public:
    GameObject(){
        
    }
    
    GameObject(std::string imgDirectory, int size_x, int size_y){
        if(!pTexture.loadFromFile(imgDirectory, sf::IntRect(0, 0, size_x, size_y))){ //loading file and limiting image size if a larger image is used
            std::cerr << "Error\n";
        }
        pSprite.setTexture(pTexture);
    }
    
    void setTexture(std::string imgDirectory, int size_x, int size_y){
        if(!pTexture.loadFromFile(imgDirectory, sf::IntRect(0, 0, size_x, size_y))){ //loading file and limiting image size if a larger image is used
            std::cerr << "Error\n";
        }
        pSprite.setTexture(pTexture);
    }
    
    void drawObject(sf::RenderWindow &window){
        window.draw(pSprite);
    }
    
    void moveObject(char direction, float moveSpeed){
        if(direction == 'u'){
            pSprite.move(0, -moveSpeed);
        }
        if(direction == 'd'){
            pSprite.move(0, moveSpeed);
        }
        if(direction == 'l'){
            pSprite.move(-moveSpeed, 0);
        }
        if(direction == 'r'){
            pSprite.move(moveSpeed, 0);
        }
    }
    
    sf::FloatRect getGlobalBounds(){
        return pSprite.getGlobalBounds();
    }
    
    float location(char letter){
        if(letter == 'y'){
            return pSprite.getPosition().y;
        }
        if(letter == 'x'){
            return pSprite.getPosition().x;
        }
    }
    
    void setPosition(float x, float y){
        pSprite.setPosition(x, y);
    }
    
    void restrictToWindow(int screen_size_x, int screen_size_y, int sizeX, int sizeY){
        if(pSprite.getPosition().y >= screen_size_y-sizeY){
            pSprite.setPosition(pSprite.getPosition().x, screen_size_y-sizeY);
        }
        if(pSprite.getPosition().y <= 0){
            pSprite.setPosition(pSprite.getPosition().x, 0);
        }
            
        if(pSprite.getPosition().x >= screen_size_x-sizeX){
            pSprite.setPosition(screen_size_x-sizeX, pSprite.getPosition().y);
            
        }
        if(pSprite.getPosition().x <= 0){
            pSprite.setPosition(0, pSprite.getPosition().y);
            
        }
    }
private:
    sf::Texture pTexture;
    sf::Sprite pSprite;
    
};
