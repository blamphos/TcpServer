#include "HttpResourceFile.h"

#include <sstream>
#include <sys/stat.h>

HttpResourceFile::HttpResourceFile(const char* fileName, ContentTypeT type) : 
    _type(type),
    _fileName(fileName)
{
    struct stat sb;
    if (stat(fileName, &sb) == -1) {
        perror("file error");
        return;
    }

    _fileSize = static_cast<uint32_t>(sb.st_size);
}

HttpResourceFile::~HttpResourceFile()
{

}

const char* HttpResourceFile::getFileName()
{
    return _fileName.c_str();
}

uint32_t HttpResourceFile::getFileSize()
{
    return _fileSize;
}

const char* HttpResourceFile::getTypeStr()
{
    return convertTypeToStr(_type);
}

const char* HttpResourceFile::convertTypeToStr(ContentTypeT type)
{
    switch (type) {
    case TEXT_HTML:
        return "text/html";
    case TEXT_CSS:
        return "text/css";
    case TEXT_JAVASCRIPT:
        return "text/javascript";
    case IMAGE_PNG:
        return "image/png";
    case IMAGE_JPG:
        return "image/jpg";
    }

    return "";
}
