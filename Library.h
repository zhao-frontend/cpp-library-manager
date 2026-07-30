#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Book {
    std::string id;
    std::string title;
    std::string author;
    int totalCopies = 0;
    int availableCopies = 0;
};

struct BorrowRecord {
    std::string userId;
    std::string bookId;
};

class Library {
public:
    explicit Library(std::filesystem::path dataDirectory);

    bool load();
    bool save() const;

    bool addBook(const Book& book, std::string& error);
    bool addUser(const std::string& userId, std::string& error);
    bool borrowBook(const std::string& userId, const std::string& bookId, std::string& error);
    bool returnBook(const std::string& userId, const std::string& bookId, std::string& error);

    std::vector<Book> searchBooks(const std::string& keyword) const;
    std::vector<Book> listBooks() const;
    std::vector<BorrowRecord> listBorrowRecords() const;

private:
    std::filesystem::path dataDirectory_;
    std::unordered_map<std::string, Book> books_;
    std::unordered_set<std::string> users_;
    std::vector<BorrowRecord> borrowRecords_;

    bool hasBorrowed(const std::string& userId, const std::string& bookId) const;
};
