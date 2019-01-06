#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include "server.cpp"
#include "client.cpp"
#include "settings.cpp"
#include "gui.h"


int main(){
    sf::RenderWindow window{{400, 200}, "Selection screen"};
    tgui::Gui gui{window}; //Create the gui and attach it to the window
    window.setPosition(sf::Vector2i(750, 500));
    window.setFramerateLimit(60);
    
    loadStartButtons(gui, window);
    
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }
            else if (event.type == sf::Event::Resized){
                window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));
                gui.setView(window.getView());
            }
            gui.handleEvent(event); // Pass the event to the widgets
        }
        window.clear(sf::Color(255,255,255,255));
        gui.draw(); // Draw all widgets
        window.display();
    }
    if(connectionType == 's'){
        serverApplication();
    }
    else if(connectionType == 'c'){
        clientApplication();
    }
    else if(connectionType == 'x'){
        settingsApplication("screensize");
    }
    else if(connectionType == 'p'){
        settingsApplication("ipport");
    }
    else{
        std::cout << "Nothing was selected" << std::endl;
    }
    return 0;
}
