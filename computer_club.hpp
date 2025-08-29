#pragma once
#include "event.hpp"
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <optional>

struct TableStats {
    int revenue = 0;
    Time total_usage{0, 0};
};

class ComputerClub {
private:
    Time opening_time;
    Time closing_time;
    int hourly_rate;
    int tables_count;
    
    std::set<std::string> clients_inside;
    std::map<int, std::string> table_to_client;
    std::map<std::string, int> client_to_table;
    std::queue<std::string> waiting_queue;
    
    std::map<int, TableStats> tables_stats;
    std::map<int, Time> table_start_time;
    
    Time current_time;
    
    void handle_client_arrived(const Event& event);
    void handle_client_sat(const Event& event);
    void handle_client_waiting(const Event& event);
    void handle_client_left(const Event& event);
    void handle_error(const Event& event, const std::string& message);
    
    void free_table(int table_number, const Time& end_time);
    bool assign_table_to_client(const std::string& client_name, int table_number, const Time& start_time);
    void process_waiting_queue(const Time& time);
    
public:
    ComputerClub(Time open_time, Time close_time, int tables, int rate);
    
    void process_event(const Event& event);
    void end_day();
    
    const auto& get_tables_stats() const { return tables_stats; }
    Time get_opening_time() const { return opening_time; }
    Time get_closing_time() const { return closing_time; }
};