#include "computer_club.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 1;
    }
    
    // Чтение количества столов
    int tables_count;
    file >> tables_count;
    
    // Чтение времени работы
    std::string open_str, close_str;
    file >> open_str >> close_str;
    
    Time opening_time, closing_time;
    opening_time.hours = std::stoi(open_str.substr(0, 2));
    opening_time.minutes = std::stoi(open_str.substr(3, 2));
    closing_time.hours = std::stoi(close_str.substr(0, 2));
    closing_time.minutes = std::stoi(close_str.substr(3, 2));
    
    // Чтение стоимости часа
    int hourly_rate;
    file >> hourly_rate;
    
    ComputerClub club(opening_time, closing_time, tables_count, hourly_rate);
    
    std::cout << opening_time << std::endl;
    
    std::string line;
    std::getline(file, line); // Пропускаем остаток первой строки
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        auto event = Event::parse(line);
        if (!event.has_value()) {
            std::cerr << "Invalid event format: " << line << std::endl;
            continue;
        }
        
        std::cout << line << std::endl;
        club.process_event(event.value());
    }
    
    club.end_day();
    std::cout << closing_time << std::endl;
    
    // Вывод статистики по столам
    const auto& stats = club.get_tables_stats();
    for (const auto& [table, table_stats] : stats) {
        std::cout << table << " " << table_stats.revenue << " " 
                  << table_stats.total_usage << std::endl;
    }
    
    return 0;
}