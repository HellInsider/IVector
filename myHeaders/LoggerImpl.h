#include "../include/ILogger.h"
#include <fstream>
#include <iostream>
#include "map"

namespace
{
    class LoggerImpl : public ILogger
    {
    private:
        std::fstream _logFile;
        std::map<RC, const std::string> errors;
        std::map<Level, const std::string> levels;

    public:
        LoggerImpl(const std::string &fn = "logger.log", bool overwrite = false);

        RC log(RC code, Level level) override;

        RC log(RC code, Level level, const char *const &srcfile, const char *const &function, int line) override;

        ~LoggerImpl() override;
    };

}
