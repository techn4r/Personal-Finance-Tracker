#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <map>

enum class TransactionType {
    INCOME,
    EXPENSE
};

enum class Category {
    FOOD,
    TRANSPORT,
    HOUSING,
    ENTERTAINMENT,
    UTILITIES,
    SALARY,
    GIFT,
    OTHER
};

std::string categoryToString(Category category) {
    switch (category) {
        case Category::FOOD: return "Еда";
        case Category::TRANSPORT: return "Транспорт";
        case Category::HOUSING: return "Жильё";
        case Category::ENTERTAINMENT: return "Развлечения";
        case Category::UTILITIES: return "Коммунальные услуги";
        case Category::SALARY: return "Зарплата";
        case Category::GIFT: return "Подарок";
        case Category::OTHER: return "Другое";
        default: return "Неизвестно";
    }
}

class Transaction {
private:
    TransactionType type;
    double amount;
    std::string description;
    Category category;
    std::time_t date;

public:
    Transaction(TransactionType type, double amount, const std::string &description,
                Category category, std::time_t date)
        : type(type), amount(amount), description(description), category(category), date(date) {
    }

    TransactionType getType() const { return type; }
    double getAmount() const { return amount; }
    std::string getDescription() const { return description; }
    Category getCategory() const { return category; }
    std::time_t getDate() const { return date; }

    void display() const {
        std::tm *tm_date = std::localtime(&date);
        char date_str[11];
        std::strftime(date_str, sizeof(date_str), "%d.%m.%Y", tm_date);

        std::cout << std::left
                << std::setw(12) << date_str
                << std::setw(15) << (type == TransactionType::INCOME ? "Доход" : "Расход")
                << std::setw(15) << categoryToString(category)
                << std::setw(25) << description
                << std::right << std::setw(10) << std::fixed << std::setprecision(2) << amount
                << std::endl;
    }

    void saveToFile(std::ofstream &file) const {
        file << static_cast<int>(type) << " "
                << amount << " "
                << static_cast<int>(category) << " "
                << date << " "
                << description << std::endl;
    }
};

class FinanceManager {
private:
    std::vector<Transaction> transactions;
    std::string username;

public:
    FinanceManager(const std::string &name) : username(name) {
    }

    void addTransaction(TransactionType type, double amount, const std::string &description,
                        Category category, std::time_t date = std::time(nullptr)) {
        transactions.emplace_back(type, amount, description, category, date);
        std::cout << "Транзакция успешно добавлена." << std::endl;
    }

    void displayAllTransactions() const {
        if (transactions.empty()) {
            std::cout << "Транзакций нет." << std::endl;
            return;
        }

        std::cout << "\n" << std::string(80, '-') << std::endl;
        std::cout << std::left
                << std::setw(12) << "Дата"
                << std::setw(15) << "Тип"
                << std::setw(15) << "Категория"
                << std::setw(25) << "Описание"
                << std::right << std::setw(10) << "Сумма"
                << std::endl;
        std::cout << std::string(80, '-') << std::endl;

        for (const auto &transaction: transactions) {
            transaction.display();
        }

        std::cout << std::string(80, '-') << std::endl;
    }

    double calculateBalance() const {
        double balance = 0.0;
        for (const auto &transaction: transactions) {
            if (transaction.getType() == TransactionType::INCOME) {
                balance += transaction.getAmount();
            } else {
                balance -= transaction.getAmount();
            }
        }
        return balance;
    }

    void generateCategoryReport() const {
        if (transactions.empty()) {
            std::cout << "Транзакций нет, отчет недоступен." << std::endl;
            return;
        }

        std::map<Category, double> incomeByCategory;
        std::map<Category, double> expenseByCategory;

        for (const auto &transaction: transactions) {
            if (transaction.getType() == TransactionType::INCOME) {
                incomeByCategory[transaction.getCategory()] += transaction.getAmount();
            } else {
                expenseByCategory[transaction.getCategory()] += transaction.getAmount();
            }
        }

        std::cout << "\n=== Отчет по доходам ===" << std::endl;
        double totalIncome = 0.0;
        for (const auto &[category, amount]: incomeByCategory) {
            std::cout << std::left << std::setw(20) << categoryToString(category)
                    << std::right << std::setw(10) << std::fixed << std::setprecision(2) << amount << std::endl;
            totalIncome += amount;
        }
        std::cout << std::string(30, '-') << std::endl;
        std::cout << std::left << std::setw(20) << "Всего доходов:"
                << std::right << std::setw(10) << std::fixed << std::setprecision(2) << totalIncome << std::endl;

        std::cout << "\n=== Отчет по расходам ===" << std::endl;
        double totalExpense = 0.0;
        for (const auto &[category, amount]: expenseByCategory) {
            std::cout << std::left << std::setw(20) << categoryToString(category)
                    << std::right << std::setw(10) << std::fixed << std::setprecision(2) << amount << std::endl;
            totalExpense += amount;
        }
        std::cout << std::string(30, '-') << std::endl;
        std::cout << std::left << std::setw(20) << "Всего расходов:"
                << std::right << std::setw(10) << std::fixed << std::setprecision(2) << totalExpense << std::endl;

        std::cout << "\n=== Итоговый баланс ===" << std::endl;
        std::cout << std::left << std::setw(20) << "Баланс:"
                << std::right << std::setw(10) << std::fixed << std::setprecision(2) << (totalIncome - totalExpense) <<
                std::endl;
    }

    bool saveToFile(const std::string &filename) const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Ошибка открытия файла для записи." << std::endl;
            return false;
        }

        file << username << std::endl;
        file << transactions.size() << std::endl;

        for (const auto &transaction: transactions) {
            transaction.saveToFile(file);
        }

        std::cout << "Данные успешно сохранены в файл " << filename << std::endl;
        return true;
    }

    bool loadFromFile(const std::string &filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Ошибка открытия файла для чтения." << std::endl;
            return false;
        }

        transactions.clear();

        std::getline(file, username);

        size_t count;
        file >> count;
        file.ignore();

        for (size_t i = 0; i < count; ++i) {
            int type, category;
            double amount;
            std::time_t date;
            std::string description;

            file >> type >> amount >> category >> date;
            file.ignore();
            std::getline(file, description);

            transactions.emplace_back(
                static_cast<TransactionType>(type),
                amount,
                description,
                static_cast<Category>(category),
                date
            );
        }

        std::cout << "Данные успешно загружены из файла " << filename << std::endl;
        return true;
    }
};

void displayMenu() {
    std::cout << "\n=== Управление личными финансами ===" << std::endl;
    std::cout << "1. Добавить доход" << std::endl;
    std::cout << "2. Добавить расход" << std::endl;
    std::cout << "3. Показать все транзакции" << std::endl;
    std::cout << "4. Показать текущий баланс" << std::endl;
    std::cout << "5. Сгенерировать отчёт по категориям" << std::endl;
    std::cout << "6. Сохранить данные в файл" << std::endl;
    std::cout << "7. Загрузить данные из файла" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "Выберите опцию: ";
}

int main() {
    std::string username;
    std::cout << "Введите ваше имя: ";
    std::getline(std::cin, username);

    FinanceManager manager(username);

    int choice;
    do {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: {
                double amount;
                std::string description;
                int categoryChoice;

                std::cout << "Введите сумму дохода: ";
                std::cin >> amount;
                std::cin.ignore();

                std::cout << "Введите описание: ";
                std::getline(std::cin, description);

                std::cout << "Выберите категорию:" << std::endl;
                std::cout << "1. Зарплата" << std::endl;
                std::cout << "2. Подарок" << std::endl;
                std::cout << "3. Другое" << std::endl;
                std::cout << "Категория: ";
                std::cin >> categoryChoice;
                std::cin.ignore();

                Category category;
                switch (categoryChoice) {
                    case 1: category = Category::SALARY;
                        break;
                    case 2: category = Category::GIFT;
                        break;
                    default: category = Category::OTHER;
                        break;
                }

                manager.addTransaction(TransactionType::INCOME, amount, description, category);
                break;
            }
            case 2: {
                double amount;
                std::string description;
                int categoryChoice;

                std::cout << "Введите сумму расхода: ";
                std::cin >> amount;
                std::cin.ignore();

                std::cout << "Введите описание: ";
                std::getline(std::cin, description);

                std::cout << "Выберите категорию:" << std::endl;
                std::cout << "1. Еда" << std::endl;
                std::cout << "2. Транспорт" << std::endl;
                std::cout << "3. Жильё" << std::endl;
                std::cout << "4. Развлечения" << std::endl;
                std::cout << "5. Коммунальные услуги" << std::endl;
                std::cout << "6. Другое" << std::endl;
                std::cout << "Категория: ";
                std::cin >> categoryChoice;
                std::cin.ignore();

                Category category;
                switch (categoryChoice) {
                    case 1: category = Category::FOOD;
                        break;
                    case 2: category = Category::TRANSPORT;
                        break;
                    case 3: category = Category::HOUSING;
                        break;
                    case 4: category = Category::ENTERTAINMENT;
                        break;
                    case 5: category = Category::UTILITIES;
                        break;
                    default: category = Category::OTHER;
                        break;
                }

                manager.addTransaction(TransactionType::EXPENSE, amount, description, category);
                break;
            }
            case 3:
                manager.displayAllTransactions();
                break;
            case 4: {
                double balance = manager.calculateBalance();
                std::cout << "Текущий баланс: " << std::fixed << std::setprecision(2) << balance << std::endl;
                break;
            }
            case 5:
                manager.generateCategoryReport();
                break;
            case 6: {
                std::string filename;
                std::cout << "Введите имя файла для сохранения: ";
                std::getline(std::cin, filename);
                manager.saveToFile(filename);
                break;
            }
            case 7: {
                std::string filename;
                std::cout << "Введите имя файла для загрузки: ";
                std::getline(std::cin, filename);
                manager.loadFromFile(filename);
                break;
            }
            case 0:
                std::cout << "Спасибо за использование программы. До свидания!" << std::endl;
                break;
            default:
                std::cout << "Неверный выбор. Пожалуйста, попробуйте снова." << std::endl;
        }
    } while (choice != 0);

    return 0;
}
