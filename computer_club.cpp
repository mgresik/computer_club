#include "computer_club.hpp"
#include <iostream>

ComputerClub::ComputerClub(Time open_time, Time close_time, int tables, int rate)
    : opening_time(open_time), closing_time(close_time), 
      tables_count(tables), hourly_rate(rate) {
    for (int i = 1; i <= tables_count; ++i) {
        tables_stats[i] = TableStats{};
    }
}

void ComputerClub::process_event(const Event& event) {
    current_time = event.timestamp;
    
    switch (event.type) {
        case EventType::CLIENT_ARRIVED:
            handle_client_arrived(event);
            break;
        case EventType::CLIENT_SAT:
            handle_client_sat(event);
            break;
        case EventType::CLIENT_WAITING:
            handle_client_waiting(event);
            break;
        case EventType::CLIENT_LEFT:
            handle_client_left(event);
            break;
        case EventType::ERROR:
            handle_error(event, "Unknown error");
            break;
    }
}

void ComputerClub::handle_client_arrived(const Event& event) {
    if (event.timestamp < opening_time || event.timestamp > closing_time) {
        handle_error(event, "NotOpenYet");
        return;
    }
    
    if (clients_inside.contains(event.client_name)) {
        handle_error(event, "YouShallNotPass");
        return;
    }
    
    clients_inside.insert(event.client_name);
}

void ComputerClub::handle_client_sat(const Event& event) {
    if (!clients_inside.contains(event.client_name)) {
        handle_error(event, "ClientUnknown");
        return;
    }
    
    if (!event.table_number.has_value() || 
        event.table_number.value() < 1 || 
        event.table_number.value() > tables_count) {
        handle_error(event, "Invalid table number");
        return;
    }
    
    int table = event.table_number.value();
    
    // Если клиент уже сидит за другим столом
    if (client_to_table.contains(event.client_name)) {
        int old_table = client_to_table[event.client_name];
        if (old_table == table) return; // Уже сидит за этим столом
        
        free_table(old_table, current_time);
    }
    
    // Если стол занят
    if (table_to_client.contains(table)) {
        handle_error(event, "PlaceIsBusy");
        return;
    }
    
    assign_table_to_client(event.client_name, table, current_time);
}

void ComputerClub::handle_client_waiting(const Event& event) {
    if (!clients_inside.contains(event.client_name)) {
        handle_error(event, "ClientUnknown");
        return;
    }
    
    // Если есть свободные столы
    if (table_to_client.size() < tables_count) {
        handle_error(event, "ICanWaitNoLonger!");
        return;
    }
    
    // Если очередь слишком большая
    if (waiting_queue.size() >= tables_count) {
        std::cout << current_time << " 11 " << event.client_name << std::endl;
        clients_inside.erase(event.client_name);
        return;
    }
    
    waiting_queue.push(event.client_name);
}

void ComputerClub::handle_client_left(const Event& event) {
    if (!clients_inside.contains(event.client_name)) {
        handle_error(event, "ClientUnknown");
        return;
    }
    
    // Если клиент сидел за столом
    if (client_to_table.contains(event.client_name)) {
        int table = client_to_table[event.client_name];
        free_table(table, current_time);
        
        // Освобождаем стол для следующего клиента из очереди
        if (!waiting_queue.empty()) {
            std::string next_client = waiting_queue.front();
            waiting_queue.pop();
            assign_table_to_client(next_client, table, current_time);
        }
    }
    
    clients_inside.erase(event.client_name);
}

void ComputerClub::handle_error(const Event& event, const std::string& message) {
    std::cout << event.timestamp << " 13 " << message << std::endl;
}

bool ComputerClub::assign_table_to_client(const std::string& client_name, int table_number, const Time& start_time) {
    table_to_client[table_number] = client_name;
    client_to_table[client_name] = table_number;
    table_start_time[table_number] = start_time;
    return true;
}

void ComputerClub::free_table(int table_number, const Time& end_time) {
    if (!table_start_time.contains(table_number)) return;
    
    Time start_time = table_start_time[table_number];
    int minutes_used = end_time.to_minutes() - start_time.to_minutes();
    
    // Расчет стоимости (округляем до часов в большую сторону)
    int hours_used = (minutes_used + 59) / 60;
    int cost = hours_used * hourly_rate;
    
    tables_stats[table_number].revenue += cost;
    tables_stats[table_number].total_usage = Time::from_minutes(
        tables_stats[table_number].total_usage.to_minutes() + minutes_used
    );
    
    std::string client = table_to_client[table_number];
    table_to_client.erase(table_number);
    client_to_table.erase(client);
    table_start_time.erase(table_number);
}

void ComputerClub::end_day() {
    // Освобождаем все столы и выгоняем оставшихся клиентов
    Time end_time = closing_time;
    
    // Сначала обрабатываем клиентов, сидящих за столами
    for (const auto& [table, client] : table_to_client) {
        free_table(table, end_time);
    }
    
    // Затем выводим оставшихся клиентов
    for (const auto& client : clients_inside) {
        std::cout << end_time << " 11 " << client << std::endl;
    }
    
    clients_inside.clear();
    table_to_client.clear();
    client_to_table.clear();
    table_start_time.clear();
    while (!waiting_queue.empty()) {
        waiting_queue.pop();
    }
}