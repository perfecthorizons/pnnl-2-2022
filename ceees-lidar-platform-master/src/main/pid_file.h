#include <string>

class PIDFile {
	public:
		PIDFile(std::string);
		~PIDFile();
	private:
		std::string filename;
};



