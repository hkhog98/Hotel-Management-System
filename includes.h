#ifndef INLUDES_H
#define INLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CONNECTIONS 100
#define BUFFER_SIZE 1024

typedef struct {
    int room_number;
    int availability;
    double price;
    char room_type[20];
} Room;

typedef struct {
    int room_number;
    int booking_status;
    int booking_id;
    char client_name[10];
    char check_in[10];
    char check_out[10];
} Book;

int client_fd, socket_fd;

Room *rooms;
int room_count;
int room_capacity;

Book *bookings;
int booking_count;
int booking_capacity;

#endif