#include "../myHeaders/LoggerImpl.h"

//////////////////Logger/////////////////
ILogger *ILogger::createLogger()
{
    return new LoggerImpl();
}

ILogger *ILogger::createLogger(const char *const &filename, bool overwrite)
{
    return new LoggerImpl(filename, false);
}

ILogger::~ILogger() = default;


//////////////////LoggerImpl/////////////////



LoggerImpl::LoggerImpl(const std::string &fn, bool overwrite)
{
    levels = {{ILogger::Level::INFO,    "INFO"},
              {ILogger::Level::SEVERE,   "SEVERE"},
              {ILogger::Level::WARNING, "WARNING"}};

    errors = {{RC::NULLPTR_ERROR,          "nullptr error"},
              {RC::ALLOCATION_ERROR,       "couldn't allocate"},
              {RC::MISMATCHING_DIMENSIONS, "dimensions do not match"},
              {RC::INFINITY_OVERFLOW,      "going beyond the boundaries of double"},
              {RC::INDEX_OUT_OF_BOUND,     "index out of bound"},
              {RC::FILE_NOT_FOUND,         "file not found"},
              {RC::INVALID_ARGUMENT,       "arguments are invalid "},
              {RC::NOT_NUMBER,             "it is not number"},
              {RC::UNKNOWN,                "unknown error"},
              {RC::VECTOR_NOT_FOUND,       "vector not found"},
              {RC::SUCCESS,                "complete"},
              {RC::MEMORY_INTERSECTION,    "found intersecting memory while copying instance"},
              {RC::IO_ERROR,               "input/output error"}};

    if (!_logFile.is_open())
    {
        overwrite ? _logFile.open(fn, std::ios::app) : _logFile.open(fn, std::ios::out);

        if (!_logFile.is_open())
        {
            std::cerr << "could not open log file";
        }

    }

}

RC LoggerImpl::log(RC code, ILogger::Level level) {

    return RC::INDEX_OUT_OF_BOUND;
}


RC LoggerImpl::log(RC code, Level level, const char *const &srcfile, const char *const &function, int line)
{
    if (!_logFile.is_open())
    {
        std::cerr << "The log file is not open";
        return RC::IO_ERROR;
    }

    std::string fullMsg = errors[code];
    std::string levelMsg = levels[level];

    _logFile << levelMsg << ": " << srcfile << " function : " << function << "| line: " << line << " | " << fullMsg
             << std::endl;

    return RC::SUCCESS;
}

LoggerImpl::~LoggerImpl()
{
    if (_logFile.is_open())
    {
        _logFile.flush();
        _logFile.close();
    }
}
