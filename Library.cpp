#include "Library.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace {
std::vector<std::string> splitTab(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, '\t')) {
        fields.push_back(field);
    }
    return fields;
}

std::string lowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}
}  // namespace

Library::Library(std::filesystem::path dataDirectory)
    : dataDirectory_(std::move(dataDirectory)) {}

bool Library::load() {
    books_.clear();
    users_.clear();
    borrowRecords_.clear();
    std::filesystem::create_directories(dataDirectory_);

    std::ifstream booksFile(dataDirectory_ / "books.tsv");
    std::string line;
    while (std::getline(booksFile, line)) {
        const auto fields = splitTab(line);
        if (fields.size() != 5) {
            continue;
        }
        try {
            Book book{fields[0], fields[1], fields[2], std::stoi(fields[3]), std::stoi(fields[4])};
            books_[book.id] = book;
        } catch (const std::exception&) {
            continue;
        }
    }

    std::ifstream usersFile(dataDirectory_ / "users.txt");
    while (std::getline(usersFile, line)) {
        if (!line.empty()) {
            users_.insert(line);
        }
    }

    std::ifstream recordsFile(dataDirectory_ / "borrows.tsv");
    while (std::getline(recordsFile, line)) {
        const auto fields = splitTab(line);
        if (fields.size() == 2) {
            borrowRecords_.push_back({fields[0], fields[1]});
        }
    }
    return true;
}

bool Library::save() const {
    std::filesystem::create_directories(dataDirectory_);

    std::ofstream booksFile(dataDirectory_ / "books.tsv", std::ios::trunc);
    for (const auto& [id, book] : books_) {
        booksFile << id << '\t' << book.title << '\t' << book.author << '\t'
                  << book.totalCopies << '\t' << book.availableCopies << '\n';
    }

    std::ofstream usersFile(dataDirectory_ / "users.txt", std::ios::trunc);
    for (const auto& user : users_) {
        usersFile << user << '\n';
    }

    std::ofstream recordsFile(dataDirectory_ / "borrows.tsv", std::ios::trunc);
    for (const auto& record : borrowRecords_) {
        recordsFile << record.userId << '\t' << record.bookId << '\n';
    }

    return booksFile.good() && usersFile.good() && recordsFile.good();
}

bool Library::addBook(const Book& book, std::string& error) {
    if (book.id.empty() || book.title.empty() || book.totalCopies <= 0) {
        error = "Book id, title and a positive stock are required.";
        return false;
    }
    if (books_.count(book.id) != 0) {
        error = "A book with this id already exists.";
        return false;
    }
    Book stored = book;
    stored.availableCopies = book.totalCopies;
    books_[stored.id] = stored;
    return true;
}

bool Library::addUser(const std::string& userId, std::string& error) {
    if (userId.empty()) {
        error = "User id cannot be empty.";
        return false;
    }
    if (!users_.insert(userId).second) {
        error = "A user with this id already exists.";
        return false;
    }
    return true;
}

bool Library::borrowBook(
    const std::string& userId,
    const std::string& bookId,
    std::string& error
) {
    if (users_.count(userId) == 0) {
        error = "User not found.";
        return false;
    }
    auto book = books_.find(bookId);
    if (book == books_.end()) {
        error = "Book not found.";
        return false;
    }
    if (book->second.availableCopies <= 0) {
        error = "No copy is currently available.";
        return false;
    }
    if (hasBorrowed(userId, bookId)) {
        error = "The user has already borrowed this book.";
        return false;
    }

    --book->second.availableCopies;
    borrowRecords_.push_back({userId, bookId});
    return true;
}

bool Library::returnBook(
    const std::string& userId,
    const std::string& bookId,
    std::string& error
) {
    const auto record = std::find_if(
        borrowRecords_.begin(),
        borrowRecords_.end(),
        [&](const BorrowRecord& item) {
            return item.userId == userId && item.bookId == bookId;
        }
    );
    if (record == borrowRecords_.end()) {
        error = "No matching borrow record was found.";
        return false;
    }

    auto book = books_.find(bookId);
    if (book == books_.end()) {
        error = "Book data is missing.";
        return false;
    }

    ++book->second.availableCopies;
    borrowRecords_.erase(record);
    return true;
}

std::vector<Book> Library::searchBooks(const std::string& keyword) const {
    const std::string normalizedKeyword = lowerCopy(keyword);
    std::vector<Book> results;
    for (const auto& [id, book] : books_) {
        const std::string searchable = lowerCopy(id + " " + book.title + " " + book.author);
        if (searchable.find(normalizedKeyword) != std::string::npos) {
            results.push_back(book);
        }
    }
    std::sort(results.begin(), results.end(), [](const Book& left, const Book& right) {
        return left.id < right.id;
    });
    return results;
}

std::vector<Book> Library::listBooks() const {
    return searchBooks("");
}

std::vector<BorrowRecord> Library::listBorrowRecords() const {
    return borrowRecords_;
}

bool Library::hasBorrowed(const std::string& userId, const std::string& bookId) const {
    return std::any_of(
        borrowRecords_.begin(),
        borrowRecords_.end(),
        [&](const BorrowRecord& record) {
            return record.userId == userId && record.bookId == bookId;
        }
    );
}
