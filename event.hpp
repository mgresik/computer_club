#pragma once
#include <string>
#include <chrono>
#include <variant>
#include <optional>
#include <iostream>

enum class EventType {
    CLIENT_ARRIVED,
    CLIENT_SAT,
    CLIENT_WAITING,
    CLIENT_LEFT,
    ERROR
};

struct Time {
    int hours;
    int minutes;
    
    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator>=(const Time& other) const;
    bool operator==(const Time& other) const;
    int to_minutes() const;
    static Time from_minutes(int total_minutes);
};

std::ostream& operator<<(std::ostream& os, const Time& time);

struct Event {
    Time timestamp;
    EventType type;
    std::string client_name;
    std::optional<int> table_number;
    
    static std::optional<Event> parse(const std::string& line);
};

std::ostream& operator<<(std::ostream& os, const Event& event);