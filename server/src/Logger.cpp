#include "Logger.h"

namespace ent {
	namespace util {
		Logger* Logger::_instance = nullptr;

		Logger* Logger::getInstance() {
			if (_instance == nullptr) {
				_instance = new Logger();
			}
			return _instance;
		}

		void Logger::setLogToConsole(bool state) {
			logToConsole = state;
		}

		void Logger::setLogToFile(bool state) {
			logToFile = state;
		}

		void Logger::setFilePath(std::string& path) {
			filePath = path;
		}

		void Logger::setLogLevel(level logLevel) {
			this->logLevel = logLevel;
		}

		void Logger::addLog(std::string msg, level logLevel) {
			// Lock the mutex
    		std::lock_guard<std::mutex> lock(mtx);

			// Open file (for immediate action)
			file.open(filePath, std::ios::app);

			// Check if msg level is acceptable
			if (logLevel <= this->logLevel) {

				// Defining level text to output
				std::string levelText;
				switch (logLevel) {
					case level::INFO:
						levelText = "[INFO]: ";
						break;
					case level::WARN:
						levelText = "[WARN]: ";
						break;
					case level::ERROR:
						levelText = "[ERROR]: ";
						break;
					case level::FATAL:
						levelText = "[FATAL]: ";
						break;
					case level::DEBUG:
						levelText = "[DEBUG]: ";
						break;
					default:
						break;
				}

				// Output
				if (logToConsole) {
					std::cout << "[" << getDate() << " " << getTime() + "] " + levelText + msg + "\n";
				}
				if (logToFile) {
					file << "[" << getDate() << " " << getTime() + "] " + levelText + msg + "\n";
				}
			}

			file.close();
		}

		std::string Logger::getTime() {
			auto now = std::chrono::system_clock::now();
			auto time_t_now = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

			auto tm = *std::localtime(&time_t_now);

			std::ostringstream oss;
			oss << std::put_time(&tm, "%H:%M:%S") << ":" << std::setfill('0') << std::setw(3) << ms.count();

			return oss.str();
		}

		std::string Logger::getDate() {
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);

			std::ostringstream oss;
			oss << std::put_time(&tm, "%d-%m-%Y");

			return oss.str();
		}

		bool Logger::createDirectory(const std::string& path) {
			// Create directory using mkdir function
            int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (status == 0) {
                return true;
            } 
            return false;
		}

		Logger::Logger() {
			logToConsole = true;
			logToFile = true;
			defaultDirectory = "logs";
			directoryExists = createDirectory(defaultDirectory);
			filePath = defaultDirectory + "/" + getDate() + ".txt";
			logLevel = level::DEBUG;
		}
	}
}