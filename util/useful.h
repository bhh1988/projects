#ifndef USEFUL_H
#define USEFUL_H

#include <sstream>
#include <string>

namespace useful {
    void string_to_hex( char* str , int n);
    // returns -1 if invalid file
    long filesize( const char* filename );
    void strip_delim( std::string& in, std::stringstream& out, const char delim );
}

#endif
