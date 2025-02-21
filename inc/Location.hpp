#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location {
	private:
		// std::vector<std::string>		_location_root;
		// std::vector<std::string>		_location_index;
		// std::vector<std::string>		_location_error_page;
		// std::vector<std::string>		_location_client_max_body_size;
		// std::vector<std::string>		_location_autoindex;
		// std::vector<std::string>		_location_auth_basic;
		// std::vector<std::string>		_location_auth_basic_user_file;
		// std::vector<std::string>		_location_allow_methods;
		// std::vector<std::string>		_location_cgi_extension;
		// std::vector<std::string>		_location_cgi_path;
		// std::vector<std::string>		_location_upload_enable;
		// std::vector<std::string>		_location_upload_path;
		// std::vector<std::string>		_location_upload_store;
		// std::vector<std::string>		_location_upload_format;
		// std::vector<std::string>		_location_upload_limit;
		// std::vector<std::string>		_location_upload_client_body_size;
		// std::vector<std::string>		_location_upload_max_file_size;
		// std::vector<std::string>		_location_upload_max_file_number;
	public:
		Location();
		~Location();
		Location(const Location &toCopy);
		Location& operator=(const Location &other);

};

#endif // LOCATION_HPP
