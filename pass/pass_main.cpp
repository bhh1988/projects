#include <iostream>
#include <crypto++/sha.h>
#include <openssl/rc4.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <map>
#include "util/useful.h"
#include "pass.h"

using namespace std;
using namespace pass;

int main(void)
{
    ifstream infile( key_file.c_str(), ifstream::in );
    // no key file: create a key file
    if( !infile.good() )
    {
        infile.close();
        cout << "Looks like this is your first time! Let's create a key file "
             << "for you." << endl;
        CreateKeyFile();
    }
    // corrupt key file: create a key file
    else if( useful::filesize( key_file.c_str() ) != digest_size )
    {
        infile.close();
        cout << "You have a key file, but it looks corrupt. Let's create a "
             << "new one." << endl;
        CreateKeyFile();
    }

    // key file exists. Validate
    else
    {
        char stored_key[ digest_size ];
        infile.read( stored_key, digest_size );
        RequestPass();
        int retries = 0;
        while( memcmp( (char*)digest, stored_key, digest_size ) )
        {
            if( retries == 3 )
            {
                HandleFailedPass();
                return 0;
            }
            cout << "Wrong Password. Try again." << endl;
            RequestPass();
            retries++;
        }
    }

    // Validated key file. Now give options
    LoadPasswords();
    while( true )
    {
        string input;
        int opt;
        cout << "Choose from the following options:" << endl
             << "1) View your services" << endl
             << "2) View a particular password" << endl
             << "3) Add/Edit a password" << endl
             << "4) Delete a password" << endl
             << "5) View all your passwords" << endl
             << "6) Save and Quit" << endl
             << "7) Don't save and Quit" << endl
             << "Your choice (Enter a number): ";
        getline( cin, input );
        stringstream( input ) >> opt;
        
        switch( opt )
        {
            case 1:
                ViewServices();
                break;
            case 2:
                ViewPass();
                break;
            case 3:
                AddPass();
                break;
            case 4:
                DeletePass();
                break;
            case 5:
                ViewAll();
                break;
            case 6:
                if( SaveAndQuit() )
                {
                    return 0;
                }
                break;
            case 7:
                if( ConfirmAndQuit() )
                {
                    return 0;
                }
                break;
            default:
                cout << "Sorry, I didn't get that." << endl << endl;
                break;
        }
    }
}
