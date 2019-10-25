/*
 * file_reader.h
 *
 *  Created on: Nov 29, 2014
 *      Author: liao
 */

#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <string>
#include "inttypes.h"
#include "sys/stat.h"

class FileReader {

public:
    FileReader(std::string file_path, bool is_tail);

    int read(char *buffer, int size, int &read_size);

    int check_and_reset();

private:
    std::string _file_path;
    int64_t _offset;
    __ino_t _file_ino;
    bool is_tail;
};

#endif /* FILE_READER_H_ */
