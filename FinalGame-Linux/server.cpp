#include <iostream> //cout cin
#include <SFML/Graphics.hpp> //textures etc.
#include <SFML/Window.hpp> //keyboard, window
#include <SFML/Network.hpp> //tcp udp
#include <SFML/Audio.hpp> //audio system
#include <string> //string
#include <stdlib.h> //exit fstream
#include "object.h"
#include "fileloading.h"
#include "sound.h"
#include "text.h"

void serverApplication(){
    std::string paddletexture = getFromFileString("texture.txt", 1); //getting texture file name from texture.txt by selecting which value to take
    std::string balltexture = getFromFileString("texture.txt", 2); 
    char leftkey = getFromFileChar("KB.txt", 1);
    char rightkey = getFromFileChar("KB.txt", 2);
    int screen_size_x = getFromFileInt("screensize.txt", 1);
    int screen_size_y = getFromFileInt("screensize.txt", 2);
    int port = getFromFileInt("port.txt", 1);
    char currentlypressed;
    float movespeed = 6, ballspeedX = 3, ballspeedY = 3;
    sf::TcpSocket socket;
    sf::TcpListener listener;
    sf::Int32 sending, receiving; //using SFML data types so data would stay same on different pc-s
    int number_of_lines = linesInFile("tilelocations.txt"); //getting how many lines are in the file, needed for the tiled background
    int paddle_sizeX = 5, paddle_sizeY = 45;
    
    sf::RenderWindow window(sf::VideoMode(screen_size_x, screen_size_y), "The grand game"); 
    window.setFramerateLimit(60); //framerate is limited to make sure no extra gpu cycles are used and movement speed is consistent on different pc-s
    
    OnScreenText textDisplay("UbuntuMono-R.tff", 18);
    
    Sound music("music.ogg");
    Sound blip("blip.wav");
    music.setLoop(true);
    music.setVolume(50.f);
    
    window.clear(sf::Color(255,255,255,255)); //giving window colour
    textDisplay.setString("Waiting for client to connect"); //setting string for the text to be displayed
    textDisplay.drawText(window);
    window.display(); //updating the display to show text

    //network tcp server setup and error checking
    //if port binding fails the program could segfault, error checking negates that
    socket.setBlocking(false); //setting blocking to false, otherwise if there is no data input code stops until it resumes
    if (listener.listen(port) != sf::Socket::Done){ //setting up listening port and checking if it is not done
        std::cout << "Check if chosen port is free. If error appears just after running, wait for OS to unbind the port" <<std::endl;
        exit (EXIT_FAILURE); //if port bind fails exit program to avoid possible problems
    }
    if (listener.accept(socket) != sf::Socket::Done){ //setting up accepting data from the socket
        std::cout << "Error accepting socket" <<std::endl;
        exit (EXIT_FAILURE); //if accepting fails no reason to continue the program
    }
    
    //creating background tiles
    //each tile is its separate object in an array
    //SFML has texture tiling feature (where it auto tiles a bigger sprite with smaller texture files) which is better, this is for a data driven demo
    GameObject backgroundTiles[number_of_lines]; //creates a array of tile GameObjects, the amount depends on the amount of lines in the tile coordinates file
    for (int i = 0; i < number_of_lines; i++) {
        backgroundTiles[i].setTexture("background_tile.png", 32, 32); //adding texture to each of the tiles
    }
    GameObject myPlayer(paddletexture, paddle_sizeX, paddle_sizeY); //creates players and gets filename of the texture from the string and sets sprite size
    GameObject myPlayer2(paddletexture, paddle_sizeX, paddle_sizeY);
    GameObject ball(balltexture, 24, 23);
    myPlayer.setPosition(0, screen_size_y/2); //sets starting position
    myPlayer2.setPosition(screen_size_x-5, screen_size_y/2);
    ball.setPosition(screen_size_x/2, screen_size_y/2);
    
    music.play();
    
    while (window.isOpen()){
        if(socket.getRemotePort() == 0){ //as per SFML documentation if port is 0 the port is disconnected
            textDisplay.setString("Client disconnected, please restart server and client");
        }
        else{
            textDisplay.setString("Client connected"); //when connected this is displayed
        }
        sf::Event event; //activating event system (used for detecting window close)
        sf::Packet packet; //creating a packet to be used for data transmission
        while (window.pollEvent(event)){ //checking for events
            if (event.type == sf::Event::Closed){ //checking if the event detected is a signal for closing (pressing X on the window)
                window.close(); //closing window
            }
        }
        
        window.clear(sf::Color(255,255,255,255)); //clear command to begin a new frame without old object positions visible
        if (event.type == sf::Event::TextEntered){ //checking for event type TextEntered
            if (event.text.unicode < 128){ //check if key is ASCII, if not just ignore
                currentlypressed= static_cast<char>(event.text.unicode); //whatever was the last key pressed is cast into a variable
            }
            if(currentlypressed == leftkey){ //comparing the currently pressed key to the key given in the txt file
                myPlayer.moveObject('u', movespeed); //moving player the amount given in movespeed
                sending = 1; //setting the sending data to 1 if moving up
            }
            else if(currentlypressed == rightkey){
                myPlayer.moveObject('d', movespeed);
                sending = 0; //setting the sending data to 0 if moving down
            }
            else{
                sending = 2; //setting the sending data to 2 if a key not used for movement is pressed as there is no movement then
            }
        }
        else{
            sending = 2; //setting the sending data to 2 if nothing is pressed as there is no movement
        }
        packet << sending; //adding data into packet
        socket.send(packet); //sending packet
        
        
        //FloatRects need to be created every loop for them to update
        sf::FloatRect ballbounds = ball.getGlobalBounds(); //bounds are put into SFML FloatRect data type for comparision later
        sf::FloatRect myPlayerbounds = myPlayer.getGlobalBounds(); //bounds are put into SFML FloatRect data type for comparision later
        sf::FloatRect myPlayer2bounds = myPlayer2.getGlobalBounds(); //bounds are put into SFML FloatRect data type for comparision later
        
        if(ballbounds.intersects(myPlayerbounds)){ //checks is the bounds of the ball cover the bounds of the paddle aka. collision testing
            ballspeedX = -ballspeedX; //on collision reverse movement on the x axis
            blip.play();
        }
        if(ballbounds.intersects(myPlayer2bounds)){ //checks is the bounds of the ball cover the bounds of the paddle aka. collision testing
            ballspeedX = -ballspeedX;//on collision reverse movement on the x axis
            blip.play();
        }
        if(ball.location('x') < 0 || ball.location('x') > screen_size_x-25){ //checking the edge of the screen on the ends, to see is someone lost or not
            ball.setPosition(screen_size_x/2, screen_size_y/2); //resetting ball to centre
            ballspeedX = -ballspeedX; //changing direction away from loser to give them a edge
        }
        if(ball.location('y') < 0 || ball.location('y') > screen_size_y-25){ //checking location on the walls, so ball would bounce off
            ballspeedY = -ballspeedY; //bounce is easy to manipulate, just changing the direction the Y axis movement
        }
        
        ball.moveObject('l', ballspeedX); //moving ball in the X axis
        ball.moveObject('d', ballspeedY); //moving ball in the Y axis
        
        socket.receive(packet); //pulling data from the other side
        packet >> receiving; //taking data out from the packet
        if(receiving == 1){ //if incoming data is 1 then move player2 up
            myPlayer2.moveObject('u', movespeed);       
        }
        if(receiving == 0){ //if incoming data is 0 then move player2 down
            myPlayer2.moveObject('d', movespeed);
        }
        std::ifstream tileLocations("tilelocations.txt"); //reading from file
        for (int i = 0; i < number_of_lines; i++) { //runs the length of the file
            int a, b;
            while (tileLocations >> a >> b){ //loading coordinates from file (x,y)
                backgroundTiles[i].setPosition(a, b); //placing tile according to position info in file
                backgroundTiles[i].drawObject(window); //drawing tile
            }
        }
        myPlayer.restrictToWindow(screen_size_x, screen_size_y, paddle_sizeX, paddle_sizeY); //takes screen size and paddle size info and restricts it to window
        myPlayer2.restrictToWindow(screen_size_x, screen_size_y, paddle_sizeX, paddle_sizeY);
        myPlayer.drawObject(window); //drawing player 1
        myPlayer2.drawObject(window); //drawing player 2
        ball.drawObject(window); //drawing ball
        textDisplay.drawText(window);
        window.display(); //updating screen
        //the drawing is dependant on the order, the earlier things stay behind the later drawing things (tiles are below players)
    }
}
