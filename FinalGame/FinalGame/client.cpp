#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "fileloading.h"
#include "text.h"

void clientApplication(){
    char leftkey = getFromFileChar("KB.txt", 1);
    char rightkey = getFromFileChar("KB.txt", 2);
    char currentlypressed;
    float screen_size_x= 250, screen_size_y= 50;
    sf::TcpSocket socket;
    sf::Packet packet;
    sf::IpAddress ip = getFromFileString("ip.txt", 1);
    int port = getFromFileInt("port.txt", 1);
    sf::Int32 sending, receiving; //using SFML data types so data would stay same on different pc-s
    
    sf::RenderWindow window(sf::VideoMode(screen_size_x, screen_size_y), "Network controller"); //screen size and name
    window.setFramerateLimit(60);
    
    OnScreenText textDisplay("UbuntuMono-R.tff", 18);
    
    //setting up tcp client
    socket.setBlocking(false);
    textDisplay.setString("Start server before client");
    socket.connect(ip, port);
    
    while (window.isOpen()){
        if(socket.getRemotePort() == 0){ //remote port turns to 0 when not connected to server
            textDisplay.setString("Server disconnected");
        }
        sf::Event event;
        sf::Packet packet;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
            }
        if (event.type == sf::Event::TextEntered){
            if (event.text.unicode < 128){
                currentlypressed = static_cast<char>(event.text.unicode);
            }
            if(currentlypressed == leftkey){
                sending = 1;
            }
            if(currentlypressed == rightkey){
                sending = 0;
            }
        }
        else{
            sending = 2;
        }
        packet << sending;
        socket.send(packet);
        
        receiving = 3; //if no data is coming in it stays as 3
        socket.receive(packet);//get data from server
        packet >> receiving;
        if(receiving == 1){
            textDisplay.setString("Enemy moving up");
        }
        if(receiving == 0){
            textDisplay.setString("Enemy moving down");
        }
        if(receiving == 2){
            textDisplay.setString("Enemy not moving");
        }
        window.clear(sf::Color(255,255,255,255));
        textDisplay.drawText(window);
        window.display();
    }
    socket.disconnect(); //gracefully disconnecting when closing window
}
