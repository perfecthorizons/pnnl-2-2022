#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <limits.h>
#include <string.h>
#include <error.h>

class Logger {
	private:
		std::ofstream 	 logfile; 
		const char *	 logdir;
		char filename[PATH_MAX];


		/**
		 * Opens a new timestamped log file and places the output stream pointer in this->logfile
		 * 	@return	True if successful, false if error
		 */
		bool openLogFile() {
			if (logfile.is_open()) {
				std::cerr << "ERROR: Logger file stream is already in use." << std::endl;
				return false;
			}

			/* Get current timestamp */
			time_t t = time(0);
			struct tm *now = localtime(&t);
			

			char buffer[80];
			strftime(buffer, 80, "%Y-%m-%d_%H:%M", now);

			/* Generate new log file name */
			bzero(filename, sizeof(filename));
			snprintf(filename, PATH_MAX, "%s%s_log.csv", logdir, buffer);

			/* Open the log file */
			logfile.open(filename);

			std::cout << "New log file:" << filename << std::endl;

			if (!logfile.is_open()) {
				std::cerr << "ERROR: Unable to open file stream for logger: " << strerror(errno) << std::endl;
				return false;
			}

			return true;
		}

		/**
		 * Closes the current log file
		 */
		void closeLogFile() {
			if (!logfile.is_open()) {
				std::cerr << "ERROR: No log file stream to close." << std::endl;
				return;
			}

			logfile.close();

		}

	public:
		Logger(const char *path) {
			this->logdir = path;
			openLogFile();

		}
		/**
		 * Logs num values at the array pointed to by data to the log file, separated by commas
		 */
		void log(float *data, unsigned int num) {
			if (!data)
				return;

			logfile << data[0];

			for (unsigned int i = 1; i < num; i++) {
				logfile << "," << data[i];
			}

			logfile << std::endl;

		}

		/**
		 * Closes the current open log file and opens a new one with the current time stamp
		 * 	@return True if successful, false if not
		 */
		bool cycleLogFile() {
			std::cout << "Cycling log file" << std::endl;
			closeLogFile();

			return openLogFile();
		}

};
#endif
