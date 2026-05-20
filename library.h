#ifndef LIBRARY_H
#define LIBRARY_H

#define MAX_NAME 100
#define MAX_CONTACT 20
#define MAX_EMAIL 100
#define MAX_CATEGORY 50
#define MAX_BORROWED 5

typedef struct {
    int id;
    char name[MAX_NAME];
    char author[MAX_NAME];
    char category[MAX_CATEGORY];
    int quantity;
    int available_quantity;
    int issue_count;
} Book;

typedef struct {
    int id;
    char name[MAX_NAME];
    char contact[MAX_CONTACT];
    char email[MAX_EMAIL];
    int issued_book_ids[MAX_BORROWED];
    int issued_count;
} Member;

// Global dynamically-allocated arrays and counts
extern Book* books;
extern int book_count;
extern int book_capacity;

extern Member* members;
extern int member_count;
extern int member_capacity;

// Core setup and cleanup
void init_library(void);
void free_library(void);

// File Persistence
void load_data(void);
void save_data(void);

// Book Operations (CRUD & Search)
void add_book(void);
void view_books(void);
void update_book(void);
void delete_book(void);
void search_books(void);

// Member Operations
void add_member(void);
void view_members(void);

// Transactions (Issue / Return)
void issue_book(void);
void return_book(void);

// Analytics & Reports
void show_analytics(void);
void export_reports(void);

// Utilities & Data Validation
int validate_email(const char* email);
int validate_contact(const char* contact);
void get_masked_password(char* password, int max_len);
void clear_screen(void);
void get_string_input(char* buffer, int max_len, const char* prompt);
int get_int_input(const char* prompt);
void press_enter_to_continue(void);

#endif // LIBRARY_H
