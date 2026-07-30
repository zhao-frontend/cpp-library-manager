#include "Library.h"

#include <iostream>
#include <string>

namespace {
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int readPositiveNumber(const std::string& prompt) {
    while (true) {
        const std::string value = readLine(prompt);
        try {
            const int number = std::stoi(value);
            if (number > 0) {
                return number;
            }
        } catch (const std::exception&) {
        }
        std::cout << "Please enter a positive integer.\n";
    }
}

void printBooks(const std::vector<Book>& books) {
    if (books.empty()) {
        std::cout << "No books found.\n";
        return;
    }
    for (const auto& book : books) {
        std::cout << book.id << " | " << book.title << " | " << book.author
                  << " | available " << book.availableCopies << "/"
                  << book.totalCopies << '\n';
    }
}

void runDemo(Library& library) {
    std::string error;
    library.addUser("U001", error);
    library.addBook({"B001", "Effective C++", "Scott Meyers", 2, 2}, error);
    if (!library.borrowBook("U001", "B001", error)
            && error != "The user has already borrowed this book.") {
        std::cerr << error << '\n';
    }
    printBooks(library.listBooks());
    library.save();
}
}  // namespace

int main(int argc, char* argv[]) {
    Library library(argc > 2 ? argv[2] : "data");
    library.load();

    if (argc > 1 && std::string(argv[1]) == "--demo") {
        runDemo(library);
        return 0;
    }

    while (true) {
        std::cout << "\n1. Add book  2. Add user  3. Borrow  4. Return\n"
                  << "5. Search    6. List records  0. Save and exit\n";
        const std::string choice = readLine("Select: ");
        std::string error;

        if (choice == "0") {
            if (!library.save()) {
                std::cerr << "Failed to save data.\n";
                return 1;
            }
            return 0;
        }
        if (choice == "1") {
            Book book;
            book.id = readLine("Book id: ");
            book.title = readLine("Title: ");
            book.author = readLine("Author: ");
            book.totalCopies = readPositiveNumber("Stock: ");
            std::cout << (library.addBook(book, error) ? "Book added.\n" : error + "\n");
        } else if (choice == "2") {
            const std::string userId = readLine("User id: ");
            std::cout << (library.addUser(userId, error) ? "User added.\n" : error + "\n");
        } else if (choice == "3") {
            const std::string userId = readLine("User id: ");
            const std::string bookId = readLine("Book id: ");
            std::cout << (library.borrowBook(userId, bookId, error) ? "Borrowed.\n" : error + "\n");
        } else if (choice == "4") {
            const std::string userId = readLine("User id: ");
            const std::string bookId = readLine("Book id: ");
            std::cout << (library.returnBook(userId, bookId, error) ? "Returned.\n" : error + "\n");
        } else if (choice == "5") {
            printBooks(library.searchBooks(readLine("Keyword: ")));
        } else if (choice == "6") {
            for (const auto& record : library.listBorrowRecords()) {
                std::cout << record.userId << " borrowed " << record.bookId << '\n';
            }
        } else {
            std::cout << "Unknown option.\n";
        }
    }
}
