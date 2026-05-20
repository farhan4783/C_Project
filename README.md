# 📚 Library Management System in C

A modular, robust, and premium command-line Library Management System built in standard C (C99). The system incorporates advanced C programming concepts including dynamic memory allocation, data validations, custom sorting and binary search, transaction integrity, and persistent file I/O using custom delimiters.

---

<video src="Recording.mp4" width="600" controlsList="nodownload"></video>


## ✨ Features

### 📖 Book Management (CRUD & Search)
- **Add Book**: Registers new titles. Automatically generates unique incrementing IDs starting at `1001`.
- **View Books**: Displays books in a styled tabular layout. Includes sorting options:
  - Sort by Book ID (Ascending)
  - Sort by Book Title (Case-Insensitive A-Z)
  - Sort by Author Name (Case-Insensitive A-Z)
  - No Sorting (as loaded from file)
- **Update Details**: Modify Title, Author, Category, and Stock. Stock cannot be reduced below the number of copies currently issued.
- **Delete Book**: Removes a book from inventory. Restricts deletion if any copy is currently checked out to a member.
- **Search Books**: Highly optimized search options:
  - Search by Book ID using a **Binary Search Algorithm**.
  - Partial string search by Book Title (Case-Insensitive).
  - Partial string search by Author Name (Case-Insensitive).

### 👥 Member Management
- **Add Member**: Registers new users. Automatically generates unique IDs starting at `2001`.
- **Validation**: Automatically validates contact number formats (10-15 digits only) and email patterns (must contain `@` and a following `.`).
- **View Members**: Tables registered library users alongside their details and active borrowed book lists.

### 🔄 Transaction Processing (Issue & Return)
- **Issue Book**: Checks borrower limits (`MAX_BORROWED = 5`), book availability (`available_quantity > 0`), and prevents a member from checking out duplicate copies of the same book. Decrements available stock and increments issue counters dynamically.
- **Return Book**: Restores available book stock and clears the book ID from the member's checked-out list, shifting remaining items to fill the slot.

### 📊 Analytics & Reporting
- **Analytics Dashboard**: Computes total book titles, total library copies, total members, available/issued quantities, and tracks the **Most Issued Book**. Includes a dynamic, text-based visual status bar showing current issued-to-available stock percentages.
- **Report Exporters**: Writes beautifully aligned summaries of inventories and members directly to `books_report.txt` and `members_report.txt`.

### 🔐 Administration Security
- **Admin Gateway**: Hardcoded defaults (`admin` / `admin123`).
- **Masked Password Field**: Standard keyboard entry masks inputs with `*` using low-level console inputs (`_getch()`).
- **Graceful Stream Redirection**: Automatically detects standard input redirection (using `_isatty(0)` in `<io.h>`). Pauses are bypassed and password masking degrades gracefully to standard stream reading, allowing the system to run automated tests with piped inputs.

---

## 📂 File Layout

* **`library.h`**: Core headers, constant definitions, struct templates (`Book`, `Member`), database globals, and procedural signatures.
* **`library.c`**: Implementation files containing CRUD logic, validators, array reallocation, delimited parsing, and transactions.
* **`main.c`**: Console router, session trackers, and interactive Guest and Administrator menus.
* **`books.txt`**: Persistent database storing book rows separated by `|` delimiters.
* **`members.txt`**: Persistent database storing member rows and active checked-out IDs.
* **`inputs.txt`**: Simulated keyboard stream used to test and seed the application.

---

## 💾 Storage Layouts

### `books.txt`
`ID|Title|Author|Category|TotalCopies|AvailableCopies|TimesIssued`
```text
1001|The C Programming Language|Dennis Ritchie|Computer Science|5|4|1
1002|Clean Code|Robert Martin|Software Engineering|3|3|1
```

### `members.txt`
`ID|Name|Contact|Email|ActiveIssuedCount|IssuedID1|IssuedID2|...`
```text
2001|John Doe|03117654321|john@gmail.com|1|1001
2002|Faraz Khan|03001234567|faraz@email.com|0
```


## To run this project 

first run 

```powershell
gcc -Wall -Wextra -std=c99 main.c library.c -o library_system.exe
```

then 

```powershell
.\library_system.exe
```
