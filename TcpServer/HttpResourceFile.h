#ifndef HTTPRESOURCEFILE_H
#define HTTPRESOURCEFILE_H

#include <string>

class HttpResourceFile {
public:
    enum ContentTypeT {
        TEXT_HTML = 0,
        TEXT_CSS,
        TEXT_JAVASCRIPT,
        IMAGE_PNG,
        IMAGE_JPG,
    };

    HttpResourceFile(const char* fileName, ContentTypeT type);
    virtual ~HttpResourceFile();
    const char* getFileName();
    const char* getTypeStr();
    uint32_t getFileSize();
    static const char* convertTypeToStr(ContentTypeT type);

private:
    ContentTypeT _type;
    std::string _fileName;
    uint32_t _fileSize;
};

#endif
