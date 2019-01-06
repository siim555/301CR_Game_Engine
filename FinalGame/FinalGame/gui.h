#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <fstream>

char connectionType;

void setServer(){
    connectionType = 's';
}
void setClient(){
    connectionType = 'c';
}
void setSettings0(){
    connectionType = 'x';
}
void setSettings1(){
    connectionType = 'p';
}

void coordinates(tgui::EditBox::Ptr x_axis, tgui::EditBox::Ptr y_axis)
{
    std::cout << "X-Axis size: " << x_axis->getText().toAnsiString() << std::endl;
    std::cout << "Y-Axis size: " << y_axis->getText().toAnsiString() << std::endl;
    std::ofstream file;
    file.open ("screensize.txt");
    file << x_axis->getText().toAnsiString() << " " << y_axis->getText().toAnsiString();
    file.close();
}

void portAndip(tgui::EditBox::Ptr ip, tgui::EditBox::Ptr port){
    std::cout << "IP: " << ip->getText().toAnsiString() << std::endl;
    std::cout << "Port: " << port->getText().toAnsiString() << std::endl;
    std::ofstream file;
    file.open ("ip.txt");
    file << ip->getText().toAnsiString();
    file.close();
    file.open ("port.txt");
    file << port->getText().toAnsiString();
    file.close();
}

void loadWidgets(tgui::Gui& gui, sf::RenderWindow& window, std::string settingChoice){
    auto upperbox = tgui::EditBox::create();
    upperbox->setSize({"66.67%", "12.5%"});
    upperbox->setPosition({"16.67%", "16.67%"});
    if(settingChoice == "screensize"){
        upperbox->setDefaultText("X-axis amount");
    }
    else if(settingChoice == "ipport"){
        upperbox->setDefaultText("IP address");
    }
    gui.add(upperbox);

    auto lowerbox = tgui::EditBox::copy(upperbox);
    lowerbox->setPosition({"16.67%", "41.6%"});
    if(settingChoice == "screensize"){
        upperbox->setDefaultText("Y-axis amount");
    }
    else if(settingChoice == "ipport"){
        upperbox->setDefaultText("Port");
    }
    gui.add(lowerbox);

    auto button = tgui::Button::create("Set");
    button->setSize({"50%", "16.67%"});
    button->setPosition({"25%", "70%"});
    gui.add(button);

    button->connect("pressed", coordinates, upperbox, lowerbox);
    button->connect("pressed", [&](){ window.close(); });
}

void loadStartButtons(tgui::Gui& gui, sf::RenderWindow& window){
    auto buttonS = tgui::Button::create();
    buttonS->setPosition({"33.3%", "15%"});
    buttonS->setSize({"33%", "15%"});
    buttonS->setText("Run as server");
    buttonS->connect("pressed", setServer);
    buttonS->connect("pressed", [&](){ window.close(); });
    gui.add(buttonS);
    
    auto buttonC = tgui::Button::copy(buttonS);
    buttonC->setPosition({"33.3%", "42.5%"});
    buttonC->setText("Run as client");
    buttonC->connect("pressed", setClient);
    buttonC->connect("pressed", [&](){ window.close(); });
    gui.add(buttonC);
    
    auto buttonX = tgui::Button::copy(buttonS);
    buttonX->setPosition({"10%", "70%"});
    buttonX->setText("Set screen size");
    buttonX->connect("pressed", setSettings0);
    buttonX->connect("pressed", [&](){ window.close(); });
    gui.add(buttonX);
    
    auto buttonP = tgui::Button::copy(buttonS);
    buttonP->setPosition({"57%", "70%"});
    buttonP->setText("Set IP and Port");
    buttonP->connect("pressed", setSettings1);
    buttonP->connect("pressed", [&](){ window.close(); });
    gui.add(buttonP);
}

