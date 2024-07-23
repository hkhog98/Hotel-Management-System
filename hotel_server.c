#include "includes.h"

pthread_t thread_id;

int server_fd;

pthread_mutex_t room_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t booking_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_socket(void *sock);
void handle_admin_operations(int admin_socket);
void handle_client_operations(int cient_socket);

void add_room(int admin_socket);
void remove_room(int admin_socket);
void view_all_bookings(int admin_socket);
void search_rooms(int client_socket);
void book_room(int client_socket);
void cancel_booking(int client_socket);
void view_my_bookings(int client_socket);

int main() {
    int active_clients;
    int addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server initialized. Listening on port %d\n", PORT);

    while ((socket_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len)) >= 0) {
        if (pthread_create(&thread_id, NULL, handle_socket, (void *)&socket_fd) != 0) {
            perror("Thread creation failed");
            close(socket_fd);
        }
        pthread_detach(thread_id);
    }

    if (socket_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    close(server_fd);
    return 0;
}

void *handle_socket(void *socket_fd) {
    int socket = *(int *)socket_fd;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';

        if (strcmp(buffer, "admin") == 0) {
            handle_admin_operations(socket);
        }
        else if (strcmp(buffer, "client") == 0) {
            handle_client_operations(socket);
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (read_size == -1) {
        perror("Receive failed");
    }

    close(socket);
    return NULL;
}

void handle_admin_operations(int admin_socket) {
    char buffer[BUFFER_SIZE];
    while(1) {
        int choice;
        memset(buffer, 0, BUFFER_SIZE);
        recv(admin_socket, &choice, sizeof(int), 0);    

        switch (choice) {
            case 1:
                add_room(admin_socket);
                break;
            case 2:
                remove_room(admin_socket);
                break;
            case 3:
                view_all_bookings(admin_socket);
                break;
            case 4:
                close(admin_socket);
                return;
            default:
                printf("Invalid operation\n");
                break;
        }
    }
}

void handle_client_operations(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int choice;
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, &choice, sizeof(int), 0);

        switch (choice) {
            case 1:
                search_rooms(client_socket);
                break;
            case 2:
                book_room(client_socket);
                break;
            case 3:
                cancel_booking(client_socket);
                break;
            case 4:
                view_my_bookings(client_socket);
                break;
            case 5:
                close(client_socket);
                return;
            default:
                printf("Invalid operation\n");
                break;
        }
    }
}

void add_room(int admin_socket) {
    Room room;
    recv(admin_socket, &room, sizeof(Room), 0);

    pthread_mutex_lock(&room_mutex);

    if (room_count == 0) {
        room_capacity = 10;
        rooms = (Room *)malloc(room_capacity * sizeof(Room));
    } else if (room_count >= room_capacity) {
        room_capacity *= 2;
        rooms = (Room *)realloc(rooms, room_capacity * sizeof(Room));
    }

    room.room_number = room_count + 1;
    rooms[room_count] = room;
    room_count++;

    pthread_mutex_unlock(&room_mutex);

    printf("Room added: %d\n", room.room_number);
}

void remove_room(int admin_socket) {
    int room_number;
    recv(admin_socket, &room_number, sizeof(int), 0);

    pthread_mutex_lock(&room_mutex);

    int room_found = 0;
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_number == room_number) {
            room_found = 1;
            rooms[i] = rooms[room_count - 1];
            room_count--;
            printf("Room removed: %d\n", room_number);
            break;
        }
    }

    pthread_mutex_unlock(&room_mutex);

    send(admin_socket, &room_found, sizeof(int), 0);
}


void view_all_bookings(int admin_socket) {
    pthread_mutex_lock(&booking_mutex);

    send(admin_socket, &booking_count, sizeof(int), 0);
    for (int i = 0; i < booking_count; i++) {
        send(admin_socket, &bookings[i], sizeof(Book), 0);
    }

    pthread_mutex_unlock(&booking_mutex);
}

void search_rooms(int client_socket) {
    pthread_mutex_lock(&room_mutex);

    int available_room_count = 0;
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].availability == 1) {
            available_room_count++;
        }
    }

    send(client_socket, &available_room_count, sizeof(int), 0);

    for (int i = 0; i < room_count; i++) {
        if (rooms[i].availability == 1) {
            send(client_socket, &rooms[i], sizeof(Room), 0);
        }
    }

    pthread_mutex_unlock(&room_mutex);
}

void book_room(int client_socket) {

    Book booking;
    recv(client_socket, &booking, sizeof(Book), 0);

    pthread_mutex_lock(&room_mutex);

    int room_found = 0;
    for (int i = 0; i < room_count; i++) {
        if (rooms[i].room_number == booking.room_number && rooms[i].availability == 1) {
            room_found = 1;
            rooms[i].availability = 0;
            break;
        }
    }
    
    pthread_mutex_unlock(&room_mutex);

    if (!room_found) {
        booking.booking_status = 0;
        send(client_socket, &booking, sizeof(Book), 0);
        pthread_mutex_unlock(&room_mutex);
        pthread_mutex_unlock(&booking_mutex);
        return;
    }

    pthread_mutex_lock(&booking_mutex);

    if (booking_count == 0) {
        booking_capacity = 10;
        bookings = (Book *)malloc(booking_capacity * sizeof(Book));
    } else if (booking_count >= booking_capacity) {
        booking_capacity *= 2;
        bookings = (Book *)realloc(bookings, booking_capacity * sizeof(Book));
    }

    booking.booking_status = 1;
    booking.booking_id = booking_count + 1;
    bookings[booking_count] = booking;
    booking_count++;

    pthread_mutex_unlock(&booking_mutex);

    send(client_socket, &booking, sizeof(Book), 0);

    printf("Room booked: %d by %s\n", booking.room_number, booking.client_name);
}

void cancel_booking(int client_socket) {

    int booking_id;
    recv(client_socket, &booking_id, sizeof(int), 0);

    pthread_mutex_lock(&booking_mutex);

    int booking_found = 0;
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].booking_id == booking_id) {
            booking_found = 1;
            int room_number = bookings[i].room_number;

            pthread_mutex_lock(&room_mutex);
            for (int j = 0; j < room_count; j++) {
                if (rooms[j].room_number == room_number) {
                    rooms[j].availability = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&room_mutex);

            bookings[i] = bookings[booking_count - 1];
            booking_count--;
            printf("Booking cancelled: %d\n", booking_id);
            break;
        }
    }

    pthread_mutex_unlock(&booking_mutex);

    send(client_socket, &booking_found, sizeof(int), 0);
}


void view_my_bookings(int client_socket) {

    char client_name[10];
    recv(client_socket, client_name, sizeof(client_name), 0);

    pthread_mutex_lock(&booking_mutex);

    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].client_name, client_name) == 0) {
            send(client_socket, &bookings[i], sizeof(Book), 0);
        }
    }

    Book end_of_bookings;
    end_of_bookings.booking_id = -1;
    send(client_socket, &end_of_bookings, sizeof(Book), 0);

    pthread_mutex_unlock(&booking_mutex);
}