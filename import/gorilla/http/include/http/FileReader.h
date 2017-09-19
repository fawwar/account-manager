#pragma once
#include <functional>
#include <fstream>
#include <cstdint>

namespace gorilla 
{
namespace http
{
class FileReader
{
    std::ifstream* fin;
    char* buffer;
    std::string fileName;
    uint64_t position1;
    uint64_t position2; //position2=0 ignore    
    int _read(); //for internal use, count
    std::map<std::string, std::function<void(const char* buffer, int len)> > callbacks;
  public:
    FileReader();
    ~FileReader();
    void setFileName(const std::string& fileName);
    //position use int64_t because int will hit 2G size
    void setPosition1(uint64_t position); //start (included)
    void setPosition2(uint64_t position); //end (no included)
    
    //register event handle, "data", "close"
    void on(const std::string& evtName, std::function<void(const char* buffer, int len)> callback);
    void read();
};

} //http
} //gorilla