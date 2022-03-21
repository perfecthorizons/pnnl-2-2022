
#include "pid_file.h"
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <stdexcept>

PIDFile::PIDFile(std::string filename) : filename(filename) {
	std::fstream fs(filename, std::fstream::out);
	if (!fs.good()) {
		throw("Could not write pid file");
	}
	fs << getpid();
	fs.close();
}

PIDFile::~PIDFile() {
	std::remove(filename.c_str());
}


