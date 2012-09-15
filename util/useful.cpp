#include <cstdio>
#include <stdio.h>
#include "useful.h"

using namespace std;

namespace useful {
    void string_to_hex( char* str , int n)
    {
        for( int i = 0; i < n; i++ )
        {
            printf("%02X ", (unsigned char)str[i] );
        }
        printf("\n");
    }

    long filesize( const char* filename )
    {
        FILE* myfile;
        long size;

        myfile = fopen( filename, "rb" );
        if( myfile == NULL )
        {
            return -1;
        }
        else
        {
            fseek( myfile, 0, SEEK_END );
            size = ftell( myfile );
            fclose( myfile );
            return size;
        }
    }

    void strip_delim( string& in, stringstream& out, const char delim )
    {
        char tmp[ in.length() ];
        stringstream stream( in );
        while( !stream.eof() )
        {
            stream.getline( tmp, delim );
            out << tmp;
        }
    }
}
