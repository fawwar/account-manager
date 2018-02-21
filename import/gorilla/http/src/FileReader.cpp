#include "JobManager.h"
#include "FileReader.h"

namespace gorilla
{
namespace http
{

FileReader::FileReader()
{
    fin = NULL;
    position1 = 0;
    position2 = 0;
    buffer = new char[1024];
}

FileReader::~FileReader()
{
    //puts("~FileReader");
    if(fin) fin->close();
    delete fin;
    delete[] buffer;
}

void FileReader::setFileName(const std::string& fileName)
{
    this->fileName = fileName;
}

void FileReader::setPosition1(uint64_t position)
{
    position1 = position;
}

void FileReader::setPosition2(uint64_t position)
{
    position2 = position;
}

int FileReader::_read()
{
    if(!fin)
    {
        fin = new std::ifstream(fileName, std::ios_base::in | std::ios_base::binary);
        fin->seekg(position1, fin->beg);
    }
    uint64_t currentPos = fin->tellg();
    if(position2 > 0 && currentPos >= position2)
    {
        return 0; //already sent all
    }
    if(fin->read(this->buffer, 1024))
    {
        if(position2 > 0 && currentPos + 1024 > position2)
        {
            return (int)(position2 - currentPos);
        }
        return 1024;
    }
    if(fin->eof())
    {
        //puts("EOF!");
        int readBytes = fin->gcount();
        if(position2 > 0 && currentPos + readBytes > position2)
        {
            return position2 - currentPos;
        }
        return readBytes;//if 0, means all data read
    }
    return -1;
}
void FileReader::read()
{
    JobManager& jm = JobManager::GetInstance();
    jm.doJob([this](){
        int len = this->_read();
        if(len > 0)
        {
            if(callbacks.find("data")!= callbacks.end())
            {
                callbacks["data"](this->buffer, len);
            }
        }
        else
        {
            if(callbacks.find("close")!= callbacks.end())
            {
                callbacks["close"](this->buffer, len);
            }
        }
    });
}
void FileReader::on(const std::string& evtName, std::function<void(const char* buffer, int len)> callback)
{
    callbacks[evtName] = callback;
}


} //http
} //gorilla