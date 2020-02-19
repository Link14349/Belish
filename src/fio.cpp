#include "fio.h"

char* Belish::readFileCPTR(const string& filename, ULL& length) {
    std::ifstream t;
    char* buffer;
    t.open(filename);
    if (!t) {
        t.close();
        length = 0;
        return nullptr;
    }
    t.seekg(0, std::ios::end);
    length = t.tellg();
    t.seekg(0, std::ios::beg);
    buffer = new char[length];
    t.read(buffer, length);
    t.close();
    return buffer;
}
bool Belish::readFile(const string& filename, string& content) {
    std::fstream file(filename, std::ios::in | std::ios::out);
    if(!file)
    {
        file.close();
        return true;
    }
    std::stringstream buf;
    buf << file.rdbuf();
    content = buf.str();
    file.close();
    return false;
}
bool Belish::writeFile(const string& filename, const string& content) {
    std::ofstream fs;
    fs.open(filename, std::ios::out);
    if (!fs) return false;
    fs << content;
    fs.close();
    return true;
}