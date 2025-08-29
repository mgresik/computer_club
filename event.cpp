#include "event.hpp"

bool Time::operator<(const Time& other) const {
    return to_minutes() < other.to_minutes();
}

bool Time::operator<=(const Time& other) const {
    return to_minutes() <= other.to_minutes();
}

bool Time::operator>(const Time& other) const {
    return to_minutes() > other.to_minutes();
}

bool Time::operator>=(const Time& other) const {
    return to_minutes() >= other.to_minutes();
}

bool Time::operator==(const Time& other) const {
    return hours == other.hours && minutes == other.minutes;
}

int Time::to_minutes() const {
    return hours * 60 + minutes;
}

Time Time::from_minutes(int total_minutes) {
    return {total_minutes / 60, total_minutes % 60};
}

std::ostream& operator<<(std::ostream& os, const Time& time) {
    os << (time.hours < 10 ? "0" : "") << time.hours << ":"
       << (time.minutes < 10 ? "0" : "") << time.minutes;
    return os;
}

std::optional<Event> Event::parse(const std::string& line) {
    if (line.length() < 16) return std::nullopt;
    
    try {
        Time timestamp;
        timestamp.hours = std::stoi(line.substr(0, 2));
        timestamp.minutes = std::stoi(line.substr(3, 2));
        
        if (timestamp.hours < 0 || timestamp.hours > 23 || 
            timestamp.minutes < 0 || timestamp.minutes > 59) {
            return std::nullopt;
        }
        
        size_t space1 = line.find(' ', 5);
        if (space1 == std::string::npos) return std::nullopt;
        
        std::string event_id_str = line.substr(6, space1 - 6);
        int event_id = std::stoi(event_id_str);
        
        EventType type;
        switch (event_id) {
            case 1: type = EventType::CLIENT_ARRIVED; break;
            case 2: type = EventType::CLIENT_SAT; break;
            case 3: type = EventType::CLIENT_WAITING; break;
            case 4: type = EventType::CLIENT_LEFT; break;
            default: return std::nullopt;
        }
        
        size_t space2 = line.find(' ', space1 + 1);
        std::string client_name = line.substr(space1 + 1, 
            (space2 == std::string::npos ? line.length() : space2) - space1 - 1);
        
        std::optional<int> table_number;
        if (space2 != std::string::npos) {
            table_number = std::stoi(line.substr(space2 + 1));
        }
        
        return Event{timestamp, type, client_name, table_number};
    }
    catch (...) {
        return std::nullopt;
    }
}

std::ostream& operator<<(std::ostream& os, const Event& event) {
    os << event.timestamp << " " << static_cast<int>(event.type) << " " 
       << event.client_name;
    if (event.table_number.has_value()) {
        os << " " << event.table_number.value();
    }
    return os;
}