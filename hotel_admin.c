#include "includes.h"

int admin_fd;

void admin_operations(int socket_fd);
void add_room(int socket_fd);
void remove_room(int socket_fd);
void view_all_bookings(int socket_fd);

int main() {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((admin_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(admin_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");
    fflush(stdout);

    strcpy(buffer, "admin");
    send(admin_fd, buffer, strlen(buffer), 0);

    admin_operations(admin_fd);

    close(admin_fd);
    return 0;
}


void admin_operations(int socket_fd) {
    int choice;
    while (1) {
        printf("Admin Panel:\n[1] Add rooms\n[2] Remove rooms\n[3] View all bookings\n[4] Exit\nEnter choice: ");
        scanf("%d", &choice);

        send(socket_fd, &choice, sizeof(int), 0);

        switch (choice) {
            case 1:
                printf("__________\n");
                add_room(socket_fd);
                printf("__________\n\n");
                break;
            case 2:
                printf("__________\n");
                remove_room(socket_fd);
                printf("__________\n\n");
                break;
            case 3:
                printf("__________\n");
                view_all_bookings(socket_fd);
                printf("__________\n\n");
                break;
            case 4:
                return;
            default:
                printf("Invalid operation\n");
                break;
        }
    }
}

void add_room(int socket_fd) {
    Room room;

    printf("Enter price: ");
    scanf("%lf", &room.price);
    printf("Enter room type: ");
    scanf("%s", room.room_type);
    room.availability = 1;

    send(socket_fd, &room, sizeof(Room), 0);

    printf("Room added request sent.\n");
}

void remove_room(int socket_fd) {
    int room_number;
    int removal_status;

    printf("Enter room number to remove: ");
    scanf("%d", &room_number);

    send(socket_fd, &room_number, sizeof(int), 0);

    recv(socket_fd, &removal_status, sizeof(int), 0);

    if (removal_status) {
        printf("Room removed successfully.\n");
    } else {
        printf("Room removal failed. Room does not exist.\n");
    }
}

void view_all_bookings(int socket_fd) {
    int booking_count;
    Book booking;

    recv(socket_fd, &booking_count, sizeof(int), 0);
    for (int i = 0; i < booking_count; i++) {
        recv(socket_fd, &booking, sizeof(Book), 0);
        printf("Booking ID: %d\nRoom Number: %d\nClient: %s\nCheck-in: %s\nCheck-out: %s\n",
            booking.booking_id, booking.room_number, booking.client_name, booking.check_in, booking.check_out);
    }
}