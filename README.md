# Hotel-Management-System

A basic hotel management system implemented in C using sockets and multi-threading. This system allows administrators to manage room bookings and clients to search, book, and cancel room reservations.
Overview

The system consists of three components:

    Server: Handles client requests and admin operations.
    Admin Client: Allows administrators to add or remove rooms and view bookings.
    Client: Allows users to search for rooms, book rooms, cancel bookings, and view their bookings.

Features

    Admin Operations:
        Add a new room
        Remove an existing room
        View all bookings

    Client Operations:
        Search for available rooms
        Book a room
        Cancel a booking
        View personal bookings

Files

    includes.h: Header file containing common definitions and declarations.
    server.c: Contains the server implementation and logic.
    admin.c: Contains the admin client implementation.
    client.c: Contains the client implementation.

Compilation

To compile the code, use the following commands:

    gcc -o server server.c -lpthread
    gcc -o admin admin.c
    gcc -o client client.c

Running

Start the server:

    ./server
    

Run the admin client:

    ./admin
    

Run the user client:

    ./client
    

Usage

    Admin Client:
        Start the admin client.
        Use the menu to manage rooms and view bookings.

    User Client:
        Start the user client.
        Use the menu to search for rooms, book rooms, cancel bookings, and view your bookings.

Dependencies

    GCC compiler
    POSIX threads library
