#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <io.h>
#include "library.h"

// Global variables defined
Book* books = NULL;
int book_count = 0;
int book_capacity = 0;

Member* members = NULL;
int member_count = 0;
int member_capacity = 0;

// Setup and Cleanup
void init_library(void) {
    books = NULL;
    book_count = 0;
    book_capacity = 0;
    
    members = NULL;
    member_count = 0;
    member_capacity = 0;
}

void free_library(void) {
    if (books) {
        free(books);
        books = NULL;
    }
    book_count = 0;
    book_capacity = 0;
    
    if (members) {
        free(members);
        members = NULL;
    }
    member_count = 0;
    member_capacity = 0;
}

// Helpers for dynamic arrays
void add_book_to_array(Book b) {
    if (book_count >= book_capacity) {
        book_capacity = book_capacity == 0 ? 10 : book_capacity * 2;
        Book* temp = realloc(books, book_capacity * sizeof(Book));
        if (!temp) {
            printf("\033[1;31mMemory reallocation failed for books database!\033[0m\n");
            return;
        }
        books = temp;
    }
    books[book_count++] = b;
}

void add_member_to_array(Member m) {
    if (member_count >= member_capacity) {
        member_capacity = member_capacity == 0 ? 10 : member_capacity * 2;
        Member* temp = realloc(members, member_capacity * sizeof(Member));
        if (!temp) {
            printf("\033[1;31mMemory reallocation failed for members database!\033[0m\n");
            return;
        }
        members = temp;
    }
    members[member_count++] = m;
}

// Robust Delimited String Parser
int get_next_field(const char* src, int* pos, char* dest, int max_len, char delim) {
    if (src[*pos] == '\0') {
        return 0;
    }
    int dest_idx = 0;
    while (src[*pos] != '\0' && src[*pos] != delim && src[*pos] != '\n' && src[*pos] != '\r') {
        if (dest_idx < max_len - 1) {
            dest[dest_idx++] = src[*pos];
        }
        (*pos)++;
    }
    dest[dest_idx] = '\0';
    if (src[*pos] == delim) {
        (*pos)++;
    }
    return 1;
}

void parse_book_line(const char* line, Book* b) {
    int pos = 0;
    char field[150];
    
    // ID
    if (!get_next_field(line, &pos, field, sizeof(field), '|')) return;
    b->id = atoi(field);
    
    // Name
    if (!get_next_field(line, &pos, b->name, MAX_NAME, '|')) return;
    
    // Author
    if (!get_next_field(line, &pos, b->author, MAX_NAME, '|')) return;
    
    // Category
    if (!get_next_field(line, &pos, b->category, MAX_CATEGORY, '|')) return;
    
    // Quantity
    if (!get_next_field(line, &pos, field, sizeof(field), '|')) return;
    b->quantity = atoi(field);
    
    // Available Quantity
    if (!get_next_field(line, &pos, field, sizeof(field), '|')) return;
    b->available_quantity = atoi(field);
    
    // Issue Count
    if (!get_next_field(line, &pos, field, sizeof(field), '|')) return;
    b->issue_count = atoi(field);
}

void parse_member_line(const char* line, Member* m) {
    int pos = 0;
    char field[150];
    
    // ID
    if (!get_next_field(line, &pos, field, sizeof(field), '|')) return;
    m->id = atoi(field);
    
    // Name
    if (!get_next_field(line, &pos, m->name, MAX_NAME, '|')) return;
    
    // Contact
    if (!get_next_field(line, &pos, m->contact, MAX_CONTACT, '|')) return;
    
    // Email
    if (!get_next_field(line, &pos, m->email, MAX_EMAIL, '|')) return;
    
    // Issued Count
    if (!get_next_field(line, &pos, field, sizeof(field), '|')) return;
    m->issued_count = atoi(field);
    
    // Book IDs
    for (int i = 0; i < MAX_BORROWED; i++) {
        m->issued_book_ids[i] = -1;
    }
    for (int i = 0; i < m->issued_count && i < MAX_BORROWED; i++) {
        if (get_next_field(line, &pos, field, sizeof(field), '|')) {
            m->issued_book_ids[i] = atoi(field);
        }
    }
}

// File loading & saving
void load_data(void) {
    FILE* fb = fopen("books.txt", "r");
    if (fb) {
        char line[512];
        while (fgets(line, sizeof(line), fb)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            Book b;
            memset(&b, 0, sizeof(Book));
            parse_book_line(line, &b);
            add_book_to_array(b);
        }
        fclose(fb);
    }
    
    FILE* fm = fopen("members.txt", "r");
    if (fm) {
        char line[512];
        while (fgets(line, sizeof(line), fm)) {
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;
            Member m;
            memset(&m, 0, sizeof(Member));
            parse_member_line(line, &m);
            add_member_to_array(m);
        }
        fclose(fm);
    }
}

void save_data(void) {
    FILE* fb = fopen("books.txt", "w");
    if (fb) {
        for (int i = 0; i < book_count; i++) {
            fprintf(fb, "%d|%s|%s|%s|%d|%d|%d\n",
                    books[i].id,
                    books[i].name,
                    books[i].author,
                    books[i].category,
                    books[i].quantity,
                    books[i].available_quantity,
                    books[i].issue_count);
        }
        fclose(fb);
    }
    
    FILE* fm = fopen("members.txt", "w");
    if (fm) {
        for (int i = 0; i < member_count; i++) {
            fprintf(fm, "%d|%s|%s|%s|%d",
                    members[i].id,
                    members[i].name,
                    members[i].contact,
                    members[i].email,
                    members[i].issued_count);
            for (int j = 0; j < members[i].issued_count; j++) {
                fprintf(fm, "|%d", members[i].issued_book_ids[j]);
            }
            fprintf(fm, "\n");
        }
        fclose(fm);
    }
}

// Utility Validation Functions
int validate_email(const char* email) {
    int len = strlen(email);
    if (len < 5) return 0;
    const char* at = strchr(email, '@');
    if (!at || at == email || at == email + len - 1) return 0;
    if (strchr(at + 1, '@')) return 0;
    const char* dot = strchr(at + 1, '.');
    if (!dot || dot == at + 1 || dot == email + len - 1) return 0;
    for (int i = 0; i < len; i++) {
        if (isspace((unsigned char)email[i])) return 0;
    }
    return 1;
}

int validate_contact(const char* contact) {
    int len = strlen(contact);
    if (len < 10 || len > 15) return 0;
    int start = 0;
    if (contact[0] == '+') {
        if (len < 11) return 0;
        start = 1;
    }
    for (int i = start; i < len; i++) {
        if (!isdigit((unsigned char)contact[i])) return 0;
    }
    return 1;
}

int case_insensitive_compare(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);
        if (c1 != c2) {
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int case_insensitive_strstr(const char* str, const char* substr) {
    char str_lower[MAX_NAME];
    char sub_lower[MAX_NAME];
    
    int i = 0;
    while (str[i] && i < MAX_NAME - 1) {
        str_lower[i] = tolower((unsigned char)str[i]);
        i++;
    }
    str_lower[i] = '\0';
    
    i = 0;
    while (substr[i] && i < MAX_NAME - 1) {
        sub_lower[i] = tolower((unsigned char)substr[i]);
        i++;
    }
    sub_lower[i] = '\0';
    
    return strstr(str_lower, sub_lower) != NULL;
}

// Binary Search Algorithm
int binary_search_books_by_id(int id) {
    // Sort actual array first by ID (keeps library sorted)
    for (int i = 1; i < book_count; i++) {
        Book key = books[i];
        int j = i - 1;
        while (j >= 0 && books[j].id > key.id) {
            books[j + 1] = books[j];
            j--;
        }
        books[j + 1] = key;
    }
    
    int low = 0, high = book_count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (books[mid].id == id) {
            return mid;
        }
        if (books[mid].id < id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

// Console and Input Helpers
void clear_screen(void) {
    system("cls");
}

void get_string_input(char* buffer, int max_len, const char* prompt) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin)) {
        // remove trailing newline
        int len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
        // Trim leading space if any
        int start = 0;
        while (buffer[start] != '\0' && isspace((unsigned char)buffer[start])) {
            start++;
        }
        if (start > 0) {
            memmove(buffer, buffer + start, len - start + 1);
        }
    } else {
        buffer[0] = '\0';
    }
}

int get_int_input(const char* prompt) {
    char buffer[50];
    int value;
    while (1) {
        get_string_input(buffer, sizeof(buffer), prompt);
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        printf("\033[1;31mInvalid input. Please enter a valid number.\033[0m\n");
    }
}

void get_masked_password(char* password, int max_len) {
    if (!_isatty(0)) {
        // Stdin is redirected (e.g. automated test piping). Read standard.
        if (fgets(password, max_len, stdin)) {
            int len = strlen(password);
            while (len > 0 && (password[len - 1] == '\n' || password[len - 1] == '\r')) {
                password[len - 1] = '\0';
                len--;
            }
        }
        return;
    }
    
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == 13 || ch == 10) { // Enter key
            password[i] = '\0';
            printf("\n");
            break;
        } else if (ch == 8) { // Backspace key
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (isprint((unsigned char)ch)) {
            if (i < max_len - 1) {
                password[i++] = ch;
                printf("*");
            }
        }
    }
}

void press_enter_to_continue(void) {
    if (!_isatty(0)) {
        // Redirected input: do not pause, as it breaks pipes
        return;
    }
    printf("\nPress Enter to continue...");
    char temp[10];
    fgets(temp, sizeof(temp), stdin);
}

// Book Operations (CRUD)
void add_book(void) {
    clear_screen();
    printf("===========================================\n");
    printf("               ADD NEW BOOK                \n");
    printf("===========================================\n");
    
    Book b;
    memset(&b, 0, sizeof(Book));
    
    int next_id = 1001;
    for (int i = 0; i < book_count; i++) {
        if (books[i].id >= next_id) {
            next_id = books[i].id + 1;
        }
    }
    b.id = next_id;
    printf("Generated Book ID: %d\n\n", b.id);
    
    while (1) {
        get_string_input(b.name, MAX_NAME, "Enter Book Title: ");
        if (strlen(b.name) > 0) break;
        printf("\033[1;31mBook title cannot be empty.\033[0m\n");
    }
    
    while (1) {
        get_string_input(b.author, MAX_NAME, "Enter Author Name: ");
        if (strlen(b.author) > 0) break;
        printf("\033[1;31mAuthor name cannot be empty.\033[0m\n");
    }
    
    while (1) {
        get_string_input(b.category, MAX_CATEGORY, "Enter Category: ");
        if (strlen(b.category) > 0) break;
        printf("\033[1;31mCategory cannot be empty.\033[0m\n");
    }
    
    while (1) {
        b.quantity = get_int_input("Enter Quantity: ");
        if (b.quantity > 0) {
            break;
        }
        printf("\033[1;31mQuantity must be a positive integer.\033[0m\n");
    }
    b.available_quantity = b.quantity;
    b.issue_count = 0;
    
    add_book_to_array(b);
    save_data();
    
    printf("\n\033[1;32mBook '%s' added successfully with ID %d!\033[0m\n", b.name, b.id);
    press_enter_to_continue();
}

void view_books(void) {
    clear_screen();
    printf("========================================================================================\n");
    printf("                                      ALL BOOKS                                         \n");
    printf("========================================================================================\n");
    
    if (book_count == 0) {
        printf("\nNo books registered in the system.\n");
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    printf("Sort books by:\n");
    printf("1. Book ID (Ascending)\n");
    printf("2. Book Title (A-Z)\n");
    printf("3. Author Name (A-Z)\n");
    printf("4. None (As Loaded)\n");
    int choice = get_int_input("Enter choice (1-4): ");
    
    // Copy books array for sorting
    Book* temp_books = malloc(book_count * sizeof(Book));
    if (!temp_books) {
        printf("Memory allocation failed!\n");
        return;
    }
    memcpy(temp_books, books, book_count * sizeof(Book));
    
    if (choice == 1) {
        // Insertion Sort by ID
        for (int i = 1; i < book_count; i++) {
            Book key = temp_books[i];
            int j = i - 1;
            while (j >= 0 && temp_books[j].id > key.id) {
                temp_books[j + 1] = temp_books[j];
                j--;
            }
            temp_books[j + 1] = key;
        }
    } else if (choice == 2) {
        // Insertion Sort by Title (A-Z)
        for (int i = 1; i < book_count; i++) {
            Book key = temp_books[i];
            int j = i - 1;
            while (j >= 0 && case_insensitive_compare(temp_books[j].name, key.name) > 0) {
                temp_books[j + 1] = temp_books[j];
                j--;
            }
            temp_books[j + 1] = key;
        }
    } else if (choice == 3) {
        // Insertion Sort by Author Name (A-Z)
        for (int i = 1; i < book_count; i++) {
            Book key = temp_books[i];
            int j = i - 1;
            while (j >= 0 && case_insensitive_compare(temp_books[j].author, key.author) > 0) {
                temp_books[j + 1] = temp_books[j];
                j--;
            }
            temp_books[j + 1] = key;
        }
    }
    
    clear_screen();
    printf("========================================================================================\n");
    printf("                                      BOOK RECORDS                                      \n");
    printf("========================================================================================\n");
    printf("%-6s | %-30s | %-20s | %-12s | %-5s | %-5s | %-8s\n", 
           "ID", "Title", "Author", "Category", "Total", "Avail", "Status");
    printf("----------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < book_count; i++) {
        const char* status = temp_books[i].available_quantity > 0 ? "AVAILABLE" : "OUT";
        printf("%-6d | %-30.30s | %-20.20s | %-12.12s | %-5d | %-5d | %-8s\n",
               temp_books[i].id,
               temp_books[i].name,
               temp_books[i].author,
               temp_books[i].category,
               temp_books[i].quantity,
               temp_books[i].available_quantity,
               status);
    }
    printf("========================================================================================\n");
    printf("Total Book Titles: %d\n", book_count);
    
    free(temp_books);
    
    press_enter_to_continue();
}

void update_book(void) {
    clear_screen();
    printf("===========================================\n");
    printf("              UPDATE BOOK DETAILS           \n");
    printf("===========================================\n");
    
    int id = get_int_input("Enter Book ID to update: ");
    int index = -1;
    for (int i = 0; i < book_count; i++) {
        if (books[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf("\n\033[1;31mBook ID %d not found.\033[0m\n", id);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    printf("\nCurrent details:\n");
    printf("Title: %s\n", books[index].name);
    printf("Author: %s\n", books[index].author);
    printf("Category: %s\n", books[index].category);
    printf("Total Quantity: %d\n", books[index].quantity);
    printf("Available Quantity: %d\n", books[index].available_quantity);
    printf("-------------------------------------------\n\n");
    
    char buffer[100];
    printf("Enter new details (press Enter to keep current):\n");
    
    get_string_input(buffer, MAX_NAME, "Enter New Title: ");
    if (strlen(buffer) > 0) {
        strcpy(books[index].name, buffer);
    }
    
    get_string_input(buffer, MAX_NAME, "Enter New Author: ");
    if (strlen(buffer) > 0) {
        strcpy(books[index].author, buffer);
    }
    
    get_string_input(buffer, MAX_CATEGORY, "Enter New Category: ");
    if (strlen(buffer) > 0) {
        strcpy(books[index].category, buffer);
    }
    
    // Update Quantity
    int currently_issued = books[index].quantity - books[index].available_quantity;
    while (1) {
        printf("Enter New Total Quantity (Current: %d, Must be at least %d): ", 
               books[index].quantity, currently_issued);
        char qty_str[50];
        get_string_input(qty_str, sizeof(qty_str), "");
        if (strlen(qty_str) == 0) {
            break;
        }
        int new_qty = atoi(qty_str);
        if (new_qty < currently_issued) {
            printf("\033[1;31mError: Cannot reduce total quantity below currently issued copies (%d).\033[0m\n", currently_issued);
        } else if (new_qty <= 0) {
            printf("\033[1;31mError: Quantity must be greater than 0.\033[0m\n");
        } else {
            books[index].available_quantity = new_qty - currently_issued;
            books[index].quantity = new_qty;
            break;
        }
    }
    
    save_data();
    printf("\n\033[1;32mBook details updated successfully!\033[0m\n");
    press_enter_to_continue();
}

void delete_book(void) {
    clear_screen();
    printf("===========================================\n");
    printf("                DELETE BOOK                \n");
    printf("===========================================\n");
    
    int id = get_int_input("Enter Book ID to delete: ");
    int index = -1;
    for (int i = 0; i < book_count; i++) {
        if (books[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf("\n\033[1;31mBook ID %d not found.\033[0m\n", id);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    int currently_issued = books[index].quantity - books[index].available_quantity;
    if (currently_issued > 0) {
        printf("\n\033[1;31mError: Cannot delete book. %d copies are currently issued to members.\033[0m\n", currently_issued);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    printf("\nBook details to delete:\n");
    printf("Title: %s\n", books[index].name);
    printf("Author: %s\n", books[index].author);
    printf("Category: %s\n", books[index].category);
    printf("-------------------------------------------\n");
    
    char confirm[10];
    get_string_input(confirm, sizeof(confirm), "Are you sure you want to delete this book? (y/n): ");
    if (tolower((unsigned char)confirm[0]) == 'y') {
        for (int i = index; i < book_count - 1; i++) {
            books[i] = books[i + 1];
        }
        book_count--;
        save_data();
        printf("\n\033[1;32mBook deleted successfully!\033[0m\n");
    } else {
        printf("\nDeletion cancelled.\n");
    }
    
    press_enter_to_continue();
}

void search_books(void) {
    clear_screen();
    printf("===========================================\n");
    printf("               SEARCH BOOKS                \n");
    printf("===========================================\n");
    
    if (book_count == 0) {
        printf("\nNo books registered in the system.\n");
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    printf("Search by:\n");
    printf("1. Book ID\n");
    printf("2. Book Title\n");
    printf("3. Author Name\n");
    int choice = get_int_input("Enter choice (1-3): ");
    
    if (choice == 1) {
        int id = get_int_input("Enter Book ID: ");
        int idx = binary_search_books_by_id(id);
        if (idx != -1) {
            printf("\n\033[1;32mBook Found:\033[0m\n");
            printf("ID:                  %d\n", books[idx].id);
            printf("Title:               %s\n", books[idx].name);
            printf("Author:              %s\n", books[idx].author);
            printf("Category:            %s\n", books[idx].category);
            printf("Total Copies:        %d\n", books[idx].quantity);
            printf("Available Copies:    %d\n", books[idx].available_quantity);
            printf("Availability Status: %s\n", books[idx].available_quantity > 0 ? "AVAILABLE" : "OUT OF STOCK");
        } else {
            printf("\n\033[1;31mBook ID %d not found.\033[0m\n", id);
        }
    } else if (choice == 2) {
        char name_query[MAX_NAME];
        get_string_input(name_query, MAX_NAME, "Enter Book Title (partial or full): ");
        printf("\n%-6s | %-30s | %-20s | %-12s | %-5s | %-5s\n", 
               "ID", "Title", "Author", "Category", "Total", "Avail");
        printf("----------------------------------------------------------------------------------------\n");
        int found = 0;
        for (int i = 0; i < book_count; i++) {
            if (case_insensitive_strstr(books[i].name, name_query)) {
                printf("%-6d | %-30.30s | %-20.20s | %-12.12s | %-5d | %-5d\n",
                       books[i].id, books[i].name, books[i].author, books[i].category,
                       books[i].quantity, books[i].available_quantity);
                found++;
            }
        }
        if (found == 0) {
            printf("\nNo books matching '%s' found.\n", name_query);
        } else {
            printf("\nFound %d matching book(s).\n", found);
        }
    } else if (choice == 3) {
        char author_query[MAX_NAME];
        get_string_input(author_query, MAX_NAME, "Enter Author Name (partial or full): ");
        printf("\n%-6s | %-30s | %-20s | %-12s | %-5s | %-5s\n", 
               "ID", "Title", "Author", "Category", "Total", "Avail");
        printf("----------------------------------------------------------------------------------------\n");
        int found = 0;
        for (int i = 0; i < book_count; i++) {
            if (case_insensitive_strstr(books[i].author, author_query)) {
                printf("%-6d | %-30.30s | %-20.20s | %-12.12s | %-5d | %-5d\n",
                       books[i].id, books[i].name, books[i].author, books[i].category,
                       books[i].quantity, books[i].available_quantity);
                found++;
            }
        }
        if (found == 0) {
            printf("\nNo books by '%s' found.\n", author_query);
        } else {
            printf("\nFound %d matching book(s).\n", found);
        }
    } else {
        printf("\nInvalid choice.\n");
    }
    
    press_enter_to_continue();
}

// Member Operations
void add_member(void) {
    clear_screen();
    printf("===========================================\n");
    printf("               ADD NEW MEMBER               \n");
    printf("===========================================\n");
    
    Member m;
    memset(&m, 0, sizeof(Member));
    
    int next_id = 2001;
    for (int i = 0; i < member_count; i++) {
        if (members[i].id >= next_id) {
            next_id = members[i].id + 1;
        }
    }
    m.id = next_id;
    printf("Generated Member ID: %d\n\n", m.id);
    
    while (1) {
        get_string_input(m.name, MAX_NAME, "Enter Full Name: ");
        if (strlen(m.name) > 0) break;
        printf("\033[1;31mFull name cannot be empty.\033[0m\n");
    }
    
    while (1) {
        get_string_input(m.contact, MAX_CONTACT, "Enter Contact Number: ");
        if (validate_contact(m.contact)) {
            break;
        }
        printf("\033[1;31mInvalid Contact number. Must be 10-15 digits only.\033[0m\n");
    }
    
    while (1) {
        get_string_input(m.email, MAX_EMAIL, "Enter Email Address: ");
        if (validate_email(m.email)) {
            break;
        }
        printf("\033[1;31mInvalid Email address format (example: user@domain.com).\033[0m\n");
    }
    
    m.issued_count = 0;
    for (int i = 0; i < MAX_BORROWED; i++) {
        m.issued_book_ids[i] = -1;
    }
    
    add_member_to_array(m);
    save_data();
    
    printf("\n\033[1;32mMember '%s' registered successfully with ID %d!\033[0m\n", m.name, m.id);
    press_enter_to_continue();
}

void view_members(void) {
    clear_screen();
    printf("========================================================================================\n");
    printf("                                      ALL MEMBERS                                       \n");
    printf("========================================================================================\n");
    
    if (member_count == 0) {
        printf("\nNo members registered in the system.\n");
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    printf("%-6s | %-20s | %-15s | %-25s | %-15s\n", 
           "ID", "Name", "Contact", "Email", "Issued Book IDs");
    printf("----------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < member_count; i++) {
        char issued_str[100] = "";
        if (members[i].issued_count == 0) {
            strcpy(issued_str, "None");
        } else {
            char val_str[15];
            for (int j = 0; j < members[i].issued_count; j++) {
                sprintf(val_str, "%d", members[i].issued_book_ids[j]);
                strcat(issued_str, val_str);
                if (j < members[i].issued_count - 1) {
                    strcat(issued_str, ", ");
                }
            }
        }
        printf("%-6d | %-20.20s | %-15.15s | %-25.25s | %-15s\n",
               members[i].id,
               members[i].name,
               members[i].contact,
               members[i].email,
               issued_str);
    }
    printf("========================================================================================\n");
    printf("Total Members: %d\n", member_count);
    
    press_enter_to_continue();
}

// Transactions
void issue_book(void) {
    clear_screen();
    printf("===========================================\n");
    printf("                ISSUE BOOK                 \n");
    printf("===========================================\n");
    
    if (member_count == 0) {
        printf("\nNo members registered in the system. Add members first.\n");
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    if (book_count == 0) {
        printf("\nNo books in the system database. Add books first.\n");
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    int member_id = get_int_input("Enter Member ID: ");
    int m_idx = -1;
    for (int i = 0; i < member_count; i++) {
        if (members[i].id == member_id) {
            m_idx = i;
            break;
        }
    }
    if (m_idx == -1) {
        printf("\n\033[1;31mError: Member ID %d not found.\033[0m\n", member_id);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    int book_id = get_int_input("Enter Book ID to issue: ");
    int b_idx = -1;
    for (int i = 0; i < book_count; i++) {
        if (books[i].id == book_id) {
            b_idx = i;
            break;
        }
    }
    if (b_idx == -1) {
        printf("\n\033[1;31mError: Book ID %d not found.\033[0m\n", book_id);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    // Check if member already has this book
    for (int i = 0; i < members[m_idx].issued_count; i++) {
        if (members[m_idx].issued_book_ids[i] == book_id) {
            printf("\n\033[1;31mError: Member '%s' already has this book issued.\033[0m\n", members[m_idx].name);
            printf("\nPress Enter to continue...");
            char temp[10];
            fgets(temp, sizeof(temp), stdin);
            return;
        }
    }
    
    // Check borrow limit
    if (members[m_idx].issued_count >= MAX_BORROWED) {
        printf("\n\033[1;31mError: Member '%s' has reached the limit of %d borrowed books.\033[0m\n", 
               members[m_idx].name, MAX_BORROWED);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    // Check book availability
    if (books[b_idx].available_quantity <= 0) {
        printf("\n\033[1;31mError: Book '%s' is currently out of stock.\033[0m\n", books[b_idx].name);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    // Perform transaction
    books[b_idx].available_quantity--;
    books[b_idx].issue_count++;
    
    members[m_idx].issued_book_ids[members[m_idx].issued_count] = book_id;
    members[m_idx].issued_count++;
    
    save_data();
    
    printf("\n\033[1;32mBook '%s' successfully issued to '%s'!\033[0m\n", books[b_idx].name, members[m_idx].name);
    press_enter_to_continue();
}

void return_book(void) {
    clear_screen();
    printf("===========================================\n");
    printf("                RETURN BOOK                \n");
    printf("===========================================\n");
    
    if (member_count == 0) {
        printf("\nNo members registered in the system.\n");
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    int member_id = get_int_input("Enter Member ID: ");
    int m_idx = -1;
    for (int i = 0; i < member_count; i++) {
        if (members[i].id == member_id) {
            m_idx = i;
            break;
        }
    }
    if (m_idx == -1) {
        printf("\n\033[1;31mError: Member ID %d not found.\033[0m\n", member_id);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    if (members[m_idx].issued_count == 0) {
        printf("\nMember '%s' does not have any books issued.\n", members[m_idx].name);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    int book_id = get_int_input("Enter Book ID to return: ");
    int issued_idx = -1;
    for (int i = 0; i < members[m_idx].issued_count; i++) {
        if (members[m_idx].issued_book_ids[i] == book_id) {
            issued_idx = i;
            break;
        }
    }
    if (issued_idx == -1) {
        printf("\n\033[1;31mError: Member '%s' does not have Book ID %d issued.\033[0m\n", members[m_idx].name, book_id);
        printf("\nPress Enter to continue...");
        char temp[10];
        fgets(temp, sizeof(temp), stdin);
        return;
    }
    
    // Find book
    int b_idx = -1;
    for (int i = 0; i < book_count; i++) {
        if (books[i].id == book_id) {
            b_idx = i;
            break;
        }
    }
    
    if (b_idx != -1) {
        books[b_idx].available_quantity++;
    }
    
    // Shift issued arrays
    for (int i = issued_idx; i < members[m_idx].issued_count - 1; i++) {
        members[m_idx].issued_book_ids[i] = members[m_idx].issued_book_ids[i + 1];
    }
    members[m_idx].issued_book_ids[members[m_idx].issued_count - 1] = -1;
    members[m_idx].issued_count--;
    
    save_data();
    
    printf("\n\033[1;32mBook ID %d successfully returned from '%s'!\033[0m\n", book_id, members[m_idx].name);
    press_enter_to_continue();
}

// Analytics & Reports
void show_analytics(void) {
    clear_screen();
    printf("===========================================\n");
    printf("            ANALYTICS DASHBOARD            \n");
    printf("===========================================\n");
    
    int total_books_available = 0;
    int total_copies_total = 0;
    int total_copies_issued = 0;
    
    for (int i = 0; i < book_count; i++) {
        total_books_available += books[i].available_quantity;
        total_copies_total += books[i].quantity;
    }
    total_copies_issued = total_copies_total - total_books_available;
    
    int max_idx = -1;
    int max_issues = 0;
    for (int i = 0; i < book_count; i++) {
        if (books[i].issue_count > max_issues) {
            max_issues = books[i].issue_count;
            max_idx = i;
        }
    }
    
    printf("1. Total Book Titles:           %d\n", book_count);
    printf("2. Total Copies in Library:     %d\n", total_copies_total);
    printf("3. Copies Currently Available:  %d\n", total_books_available);
    printf("4. Copies Currently Issued:     %d\n", total_copies_issued);
    printf("5. Total Registered Members:    %d\n", member_count);
    printf("-------------------------------------------\n");
    
    if (max_idx != -1) {
        printf("Most Issued Book:               '%s' (Issued %d times)\n", 
               books[max_idx].name, books[max_idx].issue_count);
    } else {
        printf("Most Issued Book:               None yet\n");
    }
    
    printf("-------------------------------------------\n");
    printf("Issued vs Available copies report:\n");
    if (total_copies_total > 0) {
        double issued_pct = ((double)total_copies_issued / total_copies_total) * 100.0;
        double avail_pct = 100.0 - issued_pct;
        printf("  Issued:    %.1f%%\n", issued_pct);
        printf("  Available: %.1f%%\n", avail_pct);
        
        printf("\n  Status Bar: ");
        int bar_width = 20;
        int fill_width = (int)((issued_pct / 100.0) * bar_width);
        printf("[");
        for (int i = 0; i < bar_width; i++) {
            if (i < fill_width) {
                printf("#");
            } else {
                printf("-");
            }
        }
        printf("] %.0f%% copies issued\n", issued_pct);
    } else {
        printf("  No books in library to calculate percentages.\n");
    }
    printf("===========================================\n");
    
    press_enter_to_continue();
}

void export_reports(void) {
    clear_screen();
    printf("===========================================\n");
    printf("              EXPORT REPORTS               \n");
    printf("===========================================\n");
    
    FILE* fb = fopen("books_report.txt", "w");
    if (!fb) {
        printf("\033[1;31mError exporting books status report!\033[0m\n");
        return;
    }
    fprintf(fb, "=========================================================================\n");
    fprintf(fb, "                       LIBRARY BOOKS STATUS REPORT                       \n");
    fprintf(fb, "=========================================================================\n");
    fprintf(fb, "%-6s | %-30s | %-20s | %-12s | %-5s | %-5s | %-6s\n", 
            "ID", "Title", "Author", "Category", "Total", "Avail", "Issues");
    fprintf(fb, "-------------------------------------------------------------------------\n");
    
    int total_copies_total = 0;
    int total_books_available = 0;
    for (int i = 0; i < book_count; i++) {
        fprintf(fb, "%-6d | %-30.30s | %-20.20s | %-12.12s | %-5d | %-5d | %-6d\n",
                books[i].id, books[i].name, books[i].author, books[i].category,
                books[i].quantity, books[i].available_quantity, books[i].issue_count);
        total_copies_total += books[i].quantity;
        total_books_available += books[i].available_quantity;
    }
    fprintf(fb, "=========================================================================\n");
    fprintf(fb, "Total Book Titles: %d\n", book_count);
    fprintf(fb, "Total Copies in Library: %d\n", total_copies_total);
    fprintf(fb, "Copies Currently Available: %d\n", total_books_available);
    fprintf(fb, "Copies Currently Issued: %d\n", total_copies_total - total_books_available);
    fclose(fb);
    
    FILE* fm = fopen("members_report.txt", "w");
    if (!fm) {
        printf("\033[1;31mError exporting members status report!\033[0m\n");
        return;
    }
    fprintf(fm, "=================================================================================\n");
    fprintf(fm, "                          LIBRARY MEMBERS STATUS REPORT                          \n");
    fprintf(fm, "=================================================================================\n");
    fprintf(fm, "%-6s | %-20s | %-15s | %-25s | %-12s\n", 
            "ID", "Name", "Contact", "Email", "Issued Books Count");
    fprintf(fm, "---------------------------------------------------------------------------------\n");
    for (int i = 0; i < member_count; i++) {
        char issued_str[100] = "";
        if (members[i].issued_count == 0) {
            strcpy(issued_str, "None");
        } else {
            char val_str[15];
            for (int j = 0; j < members[i].issued_count; j++) {
                sprintf(val_str, "%d", members[i].issued_book_ids[j]);
                strcat(issued_str, val_str);
                if (j < members[i].issued_count - 1) {
                    strcat(issued_str, ", ");
                }
            }
        }
        fprintf(fm, "%-6d | %-20.20s | %-15.15s | %-25.25s | %-12d (IDs: %s)\n",
                members[i].id, members[i].name, members[i].contact, members[i].email,
                members[i].issued_count, issued_str);
    }
    fprintf(fm, "=================================================================================\n");
    fprintf(fm, "Total Registered Members: %d\n", member_count);
    fclose(fm);
    
    printf("\n\033[1;32mReports exported successfully to:\033[0m\n");
    printf("1. 'books_report.txt'\n");
    printf("2. 'members_report.txt'\n");
    
    press_enter_to_continue();
}
