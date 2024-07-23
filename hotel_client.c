#include "includes.h"

int client_fd;

void client_operations(int socket_fd);
void search_rooms(int socket_fd);
void book_room(int socket_fd);
void cancel_booking(int socket_fd);
void view_my_bookings(int socket_fd);

int main() {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");
    fflush(stdout);

    strcpy(buffer, "client");
    send(client_fd, buffer, strlen(buffer), 0);

    client_operations(client_fd);

    close(client_fd);
    return 0;
}


void client_operations(int socket_fd) {
    int choice;
    while (1) {
        printf("Client Panel:\n[1] Search for rooms\n[2] Book room\n[3] Cancel booking\n[4] View my bookings\n[5] Exit\nEnter choice: ");
        scanf("%d", &choice);

        send(socket_fd, &choice, sizeof(int), 0);

        switch (choice) {
            case 1:
                printf("__________\n");
                search_rooms(socket_fd);
                printf("__________\n\n");
                break;
            case 2:
                printf("__________\n");
                book_room(socket_fd);
                printf("__________\n\n");
                break;
            case 3:
                printf("__________\n");
                cancel_booking(socket_fd);
                printf("__________\n\n");
                break;
            case 4:
                printf("__________\n");
                view_my_bookings(socket_fd);
                printf("__________\n\n");
                break;
            case 5:
                return;
            default:
                printf("Invalid operation\n");
                break;
        }
    }
}

void search_rooms(int socket_fd) {
    int available_room_count;
    Room room;

    recv(socket_fd, &available_room_count, sizeof(int), 0);
    for (int i = 0; i < available_room_count; i++) {
        recv(socket_fd, &room, sizeof(Room), 0);
        printf("Room Number: %d\nType: %s\nPrice: %.2f\n",
            room.room_number, room.room_type, room.price);
    }
}

void book_room(int socket_fd) {
    Book booking;

    printf("Enter room number: ");
    scanf("%d", &booking.room_number);
    printf("Enter client name: ");
    scanf("%s", booking.client_name);
    printf("Enter check-in date (dd/mm): ");
    scanf("%s", booking.check_in);
    printf("Enter check-out date (dd/mm): ");
    scanf("%s", booking.check_out);

    booking.booking_status = 1;

    send(socket_fd, &booking, sizeof(Book), 0);

    recv(socket_fd, &booking, sizeof(Book), 0);

    if (booking.booking_status == 1) {
        printf("Room booking successful.\nBooking ID: %d\n", booking.booking_id);
    } else {
        printf("Room booking failed. Room might be unavailable.\n");
    }
}



void cancel_booking(int socket_fd) {
    int booking_id;
    int cancellation_status;

    printf("Enter booking ID to cancel: ");
    scanf("%d", &booking_id);

    send(socket_fd, &booking_id, sizeof(int), 0);

    recv(socket_fd, &cancellation_status, sizeof(int), 0);

    if (cancellation_status) {
        printf("Booking cancellation successful.\n");
    } else {
        printf("Booking cancellation failed. Booking ID not found.\n");
    }
}


void view_my_bookings(int socket_fd) {
    char client_name[10];
    Book booking;

    printf("Enter your name: ");
    scanf("%s", client_name);

    send(socket_fd, client_name, sizeof(client_name), 0);

    while (1) {
        recv(socket_fd, &booking, sizeof(Book), 0);
        if (booking.booking_id == -1) {
            break;
        }
        printf("Booking ID: %d\nRoom Number: %d\nCheck-in: %s\nCheck-out: %s\n",
            booking.booking_id, booking.room_number, booking.check_in, booking.check_out);
    }
}
