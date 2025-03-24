// #include <iostream>
// #include <fstream>
// #include <unordered_map>

// class functions
// {
// 	private:
// 		std::unordered_map<std::string, std::string> m_mimetypes;
// 	public:
// 		functions( void );
// 		~functions( void );

// 		// @brief makes an unorderd map from the given mimetypesdoc
// 		void	loadMimeTypes( const std::string &filename );
// };

// functions::functions(void) {}
// functions::~functions(void) {}

// /**
//  * @brief makes an unorderd map from the given mimetypesdoc
//  * @param filename string with document mimetype
//  */
// void	functions::loadMimeTypes(const std::string &filename) {
// 	size_t delimiter;
// 	std::string	readline;
// 	std::string extension;
// 	std::string mimeType;
// 	std::ifstream file(filename);
// 	if (!file)
// 		throw std::runtime_error("Failed to open file: " + filename);

// 	while (std::getline(file, readline))
// 	{
// 		readline.erase(0, readline.find_first_not_of(" \t"));
// 		readline.erase(readline.find_last_not_of(" \t") + 1);
// 		if (readline.empty() || readline[0] == '#')
// 			continue;

// 		delimiter = readline.find('=');
// 		if (delimiter == std::string::npos && delimiter == readline.length() - 1)
// 			continue ;

// 		extension = readline.substr(0, delimiter);
// 		mimeType = readline.substr(delimiter + 1);
// 		extension.erase(0, extension.find_first_not_of(" \t"));
// 		extension.erase(extension.find_last_not_of(" \t") + 1);
// 		mimeType.erase(0, mimeType.find_first_not_of(" \t"));
// 		mimeType.erase(mimeType.find_last_not_of(" \t") + 1);
// 		m_mimetypes[extension] = mimeType;
// 	}
// }