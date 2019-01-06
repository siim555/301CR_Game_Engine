#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include "gui.h"



void settingsApplication(std::string settingChoice){
    sf::RenderWindow window{{200, 200}, "Settings"};
    tgui::Gui gui{window}; //Create the gui and attach it to the window
    window.setPosition(sf::Vector2i(750, 500));
    window.setFramerateLimit(60);
    
    loadWidgets(gui, window, settingChoice);
    
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
}
