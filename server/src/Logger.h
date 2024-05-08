/*
VowelVoxel
Copyright (C) 2023 ExPikaPaka, Nick Nagirnyi <nick.nagirnyi@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


/*
    Logger Class

    Purpose:
    The Logger class provides a simple yet flexible logging mechanism for applications 
		written in C++. It allows developers to log messages of different severity levels 
		to both the console and a log file. The class is designed as a singleton to ensure 
		that only one instance of the logger exists throughout the application, providing 
		centralized logging functionality.

    Features:
    - Supports logging messages with different severity levels: INFO, WARN, ERROR, FATAL, 
		DEBUG.
    - Allows logging to both the console and a log file simultaneously, with the 
		option to enable/disable each output separately.
    - Automatically timestamps each log message with the current date and time.
    - Offers thread-safe logging functionality to handle multiple threads simultaneously.
    - Provides customizable log file paths and log levels.

    Usage:
    1. Include the "Logger.h" header file in your source code.
    2. Obtain an instance of the Logger using the static method Logger::getInstance().
    3. Optionally, configure logging settings such as enabling/disabling console/file 
		logging, setting log file paths, and setting log levels using appropriate setter methods.
    4. Log messages using the addLog() method, specifying the message content and sever ity level.
    5. The Logger class manages its own resources and requires no explicit cleanup. However, 
		if necessary, you can manually release the logger instance using delete or let it be cleaned up automatically when it goes out of scope if using smart pointers.

    Example:

    #include "Logger.h"

    int main() {
        // Obtain a logger instance
        ent::util::Logger* logger = ent::util::Logger::getInstance();

        // Enable logging to console and file
        logger->setLogToConsole(true);
        logger->setLogToFile(true);

        // Log a message with DEBUG severity level
        logger->addLog("Application started.", ent::util::level::DEBUG);

        // Other application logic...

        // Logger instance is automatically cleaned up when no longer needed

        return 0;
    }
*/


#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <mutex>
#include <sys/stat.h>

namespace ent {
	// Utility namespace
	namespace util {
		// Log levels
		enum class level {
			INFO,
			WARN,
			ERROR,
			FATAL,
			DEBUG

		};

		
		// Singleton instance logger. Logs to cmd or/and file
		class Logger {
		public:
			// Returns current instance to the main object, or creates it, if none exists
			static Logger* getInstance();
			// Enable or disable logging to console
			void setLogToConsole(bool state);
			// Enable or disable logging to file
			void setLogToFile(bool state);
			// Set log filepath. (By default it's "logs/DATE.txt")
			void setFilePath(std::string& path);

			// Sets log level up to desired
			// @param level: INFO, WARN, ERROR, FATAL, DEBUG
			void setLogLevel(level logLevel);

			// Add log message with different levels
			void addLog(std::string msg, level logLevel);


			~Logger() {};
		private:
			// Output bools
			bool logToConsole;
			bool logToFile;

			// File log directory boolean
			bool directoryExists;

			// log level
			level logLevel;

			// Mutex for log synchronization
            std::mutex mtx;

			// Filepath
			std::string filePath;
			std::ofstream file;
			std::string defaultDirectory;

			// Returns current time
			std::string getTime();
			// Returns current date
			std::string getDate();

			// Create directory
			bool createDirectory(const std::string& path);

			// Singleton
			Logger();
			static Logger* _instance;
		};

	}
}