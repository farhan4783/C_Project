#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"


int is_admin_logged_in = 0;

void login_admin(void) {
    clear_screen();
    printf("\033[1;36m===========================================\033[0m\n");
    printf("\033[1;36m               ADMIN LOGIN                 \033[0m\n");
    printf("\033[1;36m===========================================\033[0m\n");
    
    char username[50];
    char password[50];
    
    get_string_input(username, sizeof(username), "Enter Admin Username: ");
    
    printf("Enter Admin Password: ");
    get_masked_password(password, sizeof(password));
    
   
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        is_admin_logged_in = 1;
        printf("\n\033[1;32mAccess Granted! Welcome, Administrator.\033[0m\n");
    } else {
        is_admin_logged_in = 0;
        printf("\n\033[1;31mAccess Denied! Invalid Username or Password.\033[0m\n");
    }
    
    press_enter_to_continue();
}

void show_guest_menu(void) {
    while (1) {
        clear_screen();
        printf("\033[1;35m===========================================\033[0m\n");
        printf("\033[1;35m        LIBRARY MANAGEMENT SYSTEM          \033[0m\n");
        printf("\033[1;35m               (Guest Mode)                \033[0m\n");
        printf("\033[1;35m===========================================\033[0m\n");
        printf("1. View Books (with Sorting)\n");
        printf("2. Search Book by ID, Name, or Author\n");
        printf("3. Administrator Login\n");
        printf("4. Exit Application\n");
        printf("\033[1;35m-------------------------------------------\033[0m\n");
        
        int choice = get_int_input("Select an option (1-4): ");
        switch (choice) {
            case 1:
                view_books();
                break;
            case 2:
                search_books();
                break;
            case 3:
                login_admin();
                if (is_admin_logged_in) {
                    return;
                }
                break;
            case 4:
                clear_screen();
                printf("\nThank you for using the Library Management System. Goodbye!\n");
                free_library();
                exit(0);
            default:
                printf("\033[1;31mInvalid choice. Please select between 1 and 4.\033[0m\n");
                press_enter_to_continue();
                break;
        }
    }
}

void show_admin_menu(void) {
    while (1) {
        clear_screen();
        printf("\033[1;36m===========================================\033[0m\n");
        printf("\033[1;36m        LIBRARY MANAGEMENT SYSTEM          \033[0m\n");
        printf("\033[1;36m            (Administrator Mode)           \033[0m\n");
        printf("\033[1;36m===========================================\033[0m\n");
        printf("1.  Add New Book\n");
        printf("2.  View Book Inventory\n");
        printf("3.  Update Book Details\n");
        printf("4.  Delete Book from System\n");
        printf("5.  Search Book by ID, Name, or Author\n");
        printf("6.  Register New Library Member\n");
        printf("7.  View Registered Members\n");
        printf("8.  Issue Book to Member\n");
        printf("9.  Return Book from Member\n");
        printf("10. Show Analytics Dashboard\n");
        printf("11. Export Inventory & Member Reports\n");
        printf("12. Logout to Guest Mode\n");
        printf("13. Exit Application\n");
        printf("\033[1;36m-------------------------------------------\033[0m\n");
        
        int choice = get_int_input("Select an option (1-13): ");
        switch (choice) {
            case 1:
                add_book();
                break;
            case 2:
                view_books();
                break;
            case 3:
                update_book();
                break;
            case 4:
                delete_book();
                break;
            case 5:
                search_books();
                break;
            case 6:
                add_member();
                break;
            case 7:
                view_members();
                break;
            case 8:
                issue_book();
                break;
            case 9:
                return_book();
                break;
            case 10:
                show_analytics();
                break;
            case 11:
                export_reports();
                break;
            case 12:
                is_admin_logged_in = 0;
                printf("\n\033[1;32mLogged out successfully!\033[0m\n");
                press_enter_to_continue();
                return; 
            case 13:
                clear_screen();
                printf("\nThank you for using the Library Management System. Goodbye!\n");
                free_library();
                exit(0);
            default:
                printf("\033[1;31mInvalid choice. Please select between 1 and 13.\033[0m\n");
                press_enter_to_continue();
                break;
        }
    }
}

int main(void) {
    
    
    init_library();
    load_data();
    
    while (1) {
        if (is_admin_logged_in) {
            show_admin_menu();
        } else {
            show_guest_menu();
        }
    }
    
    free_library();
    return 0;
}
