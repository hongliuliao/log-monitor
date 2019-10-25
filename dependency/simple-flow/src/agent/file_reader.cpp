/*
 * file_reader.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */
#include <fstream>
#include <cerrno>
#include <cstring>
#include "simple_log.h"
#include "file_reader.h"

FileReader::FileReader(std::string file_path, bool is_tail) {
    this->_file_path = file_path;
    _offset = 0;
    if (is_tail) {
        std::fstream fs(_file_path.c_str(), std::fstream::in);
        fs.seekg(0, fs.end);
        _offset = fs.tellg();
        fs.close();
    }
    _file_ino = 0;
    struct stat tmp_stat;
    if(stat(_file_path.c_str(), &tmp_stat) != 0){
        LOG_WARN("Oops  stats file failed [path:%s][errno:%s]", file_path.c_str(), strerror(errno));
    } else {
        _file_ino = tmp_stat.st_ino;
    }

}

int FileReader::read(char *buffer, int size, int &read_size) {
    std::fstream fs(_file_path.c_str(), std::fstream::in);
    if(!fs) {
        LOG_WARN("open file failed which path:%s", _file_path.c_str());
        return -1;
    }

    fs.seekg(_offset, fs.beg);
    fs.read(buffer, size);

    read_size = fs.gcount();

    int ret = 0;
    if(read_size == 0) {
        LOG_DEBUG("no more new data which offset:%ld", _offset);
        ret = 1; // no more new data
    }

    _offset += read_size;
    fs.close();
    return ret;
}

int FileReader::check_and_reset() {
    struct stat tmp_stat;
    if(stat(_file_path.c_str(), &tmp_stat) != 0){
        LOG_WARN("Oops  stats file failed [path:%s][errno:%s]", _file_path.c_str(), strerror(errno));
        return -1;
    }
    if(tmp_stat.st_ino == _file_ino) {
        return 0;
    }
    LOG_INFO("file change which file_path:%s, old ino:%d, new ino:%d", _file_path.c_str(), _file_ino, tmp_stat.st_ino);
    _file_ino = tmp_stat.st_ino;
    _offset = 0;
    return 0;
}
