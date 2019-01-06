The FinalGame folder is for Windows and contains both compiled exe and the VS2017 project (including the needed libs)
Compile is from VS2017
The FinalGame.7z is if github messes up the system settings.
Due to VS2017 weirdness not all ..cpp files are added to the project (still get read from folder)

The FinalGame-Linux is the non project code, has one extra feature (commented in code), but needs the G++ compiler.
To compile and running needs the following libraries installed sfml-2.5.2 and tgui-0.8.2.
The command to compile is this (on linux): g++ -std=c++14 main.cpp -ltgui -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio -o main


Bugs: On windows there is a lag and networking to check if client or server is disconnected does not work, due to unknown reasons (sfml documentation says it should work)
In code the getRemotePort part in the beginning of the loop in server.cpp and client.cpp