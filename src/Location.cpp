#include "../inc/Location.hpp"

Location::Location() {
}

Location::~Location() {
}

Location::Location(const Location &toCopy) {
	*this = toCopy;
}

Location& Location::operator=(const Location &other) {
	if (this != &other) {
	}
	return *this;
}
