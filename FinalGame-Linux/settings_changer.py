import tkinter



cont=True

while cont == True:
    print("What parameter do you wish to change:")
    print("1.Keyboard keys")
    print("2.Texture filenames")
    print("3.IP address")
    print("4.Port")
    print("5.Screensize\n")
    print("Type the number of the selection :")
    selection = input()
    if selection == "1":
        txtfile=open("KB.txt", "r")
        print("Current keys are:")
        print(txtfile.read())
        txtfile.close()
        print("Do you wish to make a change? (Y/N)")
        change = input()
        if change == "y" or change == "Y":
            print("Please enter the new keys, separate them by a space: ")
            towrite=input()
            txtfile=open("KB.txt", "w")
            txtfile.write(towrite)
            txtfile.close()
    if selection == "2":
        txtfile=open("texture.txt", "r")
        print("Current filenames are:")
        print(txtfile.read())
        txtfile.close()
        print("Do you wish to make a change? (Y/N)")
        change = input()
        if change == "y" or change == "Y":
            print("Please enter the new filenames, separate them by a space: ")
            towrite=input()
            txtfile=open("texture.txt", "w")
            txtfile.write(towrite)
            txtfile.close()
    if selection == "3":
        txtfile=open("ip.txt", "r")
        print("Current IP is:")
        print(txtfile.read())
        txtfile.close()
        print("Do you wish to make a change? (Y/N)")
        change = input()
        if change == "y" or change == "Y":
            print("Please enter the new IP (localhost is also a valid address): ")
            towrite=input()
            txtfile=open("ip.txt", "w")
            txtfile.write(towrite)
            txtfile.close()
    if selection == "4":
        txtfile=open("port.txt", "r")
        print("Current port is:")
        print(txtfile.read())
        txtfile.close()
        print("Do you wish to make a change? (Y/N)")
        change = input()
        if change == "y" or change == "Y":
            print("Please enter the new port (not between 0 - 1024, 0 is not valid and else are reserved): ")
            towrite=input()
            txtfile=open("port.txt", "w")
            txtfile.write(towrite)
            txtfile.close()
    if selection == "5":
        txtfile=open("screensize.txt", "r")
        print("Current resolution is:")
        print(txtfile.read())
        txtfile.close()
        print("Do you wish to make a change? (Y/N)")
        change = input()
        if change == "y" or change == "Y":
            print("Please enter the new X and Y sizes: ")
            towrite=input()
            txtfile=open("screensize.txt", "w")
            txtfile.write(towrite)
            txtfile.close()
    print("Do you wish to change anything else (Y/N)")
    continuing = input()
    if continuing == 'N' or continuing == 'n':
        cont = False
