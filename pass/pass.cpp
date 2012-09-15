#include <iostream>
#include <crypto++/sha.h>
#include <openssl/rc4.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>
#include "util/useful.h"
#include "pass.h"

using namespace std;

namespace pass
{
    const char service_pass_delim = ':';
    const int max_string_size = 50;
    const string passwords_file( "/etc/pass/.passwords" );
    const string key_file( "/etc/pass/.pass.key" );
    byte digest[ digest_size ];
    map<string,string> passwords_map;

    void HandleFailedPass()
    {
        return;
    }

    void EncryptDecrypt( const string& start_str, stringstream& out )
    {
        RC4_KEY key;
        RC4_set_key( &key, digest_size, (unsigned char*)digest );
        char end_str[ start_str.length() ];
        RC4( &key, start_str.length(), (unsigned char*)start_str.c_str(),
                (unsigned char*)end_str );
        out.write( end_str, start_str.length() );
    }

    void LoadMapFromStream( stringstream& stream )
    {
        char token[ max_string_size + 1 ];
        stream.getline( token, max_string_size + 1, service_pass_delim );
        while( !stream.eof() )
        {
            string service( token );
            stream.getline( token, max_string_size + 1 );
            string pass( token );
            passwords_map[ service ] = pass;
            // get the next service
            stream.getline( token, max_string_size + 1, service_pass_delim );
        }
    }

    void WriteMapToStream( stringstream& stream )
    {
        for( map<string,string>::iterator iter = passwords_map.begin() ;
                iter != passwords_map.end() ; ++iter )
        {
            stream << iter->first << ':' << iter->second << endl;
        }
    }

    void LoadPasswords()
    {
        ifstream infile( passwords_file.c_str(), ifstream::in );
        if( !infile.good() )
        {
            return;
        }

        // file is good. Decrypt
        stringstream file_stream;
        file_stream << infile.rdbuf();
        stringstream decrypted_stream;
        EncryptDecrypt( file_stream.str(), decrypted_stream );

        // Load the passwords map
        LoadMapFromStream( decrypted_stream );

        return;
    }

    void ViewServices()
    {
        cout << "Here are your existing services:" << endl;
        for( map<string,string>::iterator iter = passwords_map.begin() ;
                iter != passwords_map.end() ; ++iter )
        {
            cout << iter->first << endl;
        }
        cout << endl;
        return;
    }

    void ViewPass()
    {
        ViewServices();
        cout << "View Password" << endl << "Your choice: ";

        // get and strip the service
        string service;
        getline( cin, service );
        stringstream stripped;
        useful::strip_delim( service, stripped, service_pass_delim );

        // check the passwords map
        map<string, string>::iterator iter = passwords_map.find(
                stripped.str() );
        if( iter == passwords_map.end() )
        {
            cout << "No entry exists for service " << service << endl << endl;
            return;
        }
        cout << service << ": " << passwords_map[ stripped.str() ]
             << endl << endl;
        return;
    }

    void AddPass()
    {
        cout << "Add/Edit Password" << endl
             << "Be sure that you know what services you already "
             << "have passwords for. Any passwords you enter here "
             << "for an existing service will overwrite that service"
             << endl << endl;
        ViewServices();
        
        // get and strip the service
        cout << "Enter your service: ";
        string service;
        getline( cin, service );
        stringstream stripped;
        useful::strip_delim( service, stripped, service_pass_delim );
        
        // get password
        int num_tries = 3;
        stringstream prompt;
        prompt << "Enter Password for " << service.c_str() << ": ";
        while( num_tries > 0 )
        {
            string pass1( getpass( prompt.str().c_str() ) );
            string pass2( getpass( "Enter Password again: " ) );
            if( !pass1.compare( pass2 ) )
            {
                passwords_map[ stripped.str() ] = pass1;
                cout << "New entry successful" << endl << endl;
                return;
            }
            num_tries--;
            cout << "The two passwords don't match." << endl << endl;
        }

        // if we got here, the person can't seem to type the same password
        // more than once.
        cout << "You keep typing your password wrong! Going back to main menu."
             << endl;

        return;
    }

    void DeletePass()
    {
        ViewServices();
        cout << "Delete Password" << endl << "Your choice: ";
        string service;
        getline( cin, service );
        stringstream stripped;
        useful::strip_delim( service, stripped, service_pass_delim );

        // check the passwords map
        map<string, string>::iterator iter = passwords_map.find(
                stripped.str() );
        if( iter == passwords_map.end() )
        {
            cout << "No entry exists for service " << service << endl << endl;
            return;
        }

        passwords_map.erase( iter );
        cout << "Number of Passwords is: " <<  passwords_map.size() << endl;
        cout << "Password deleted" << endl << endl;
    }

    void ViewAll()
    {
        cout << "View ALL Passwords" << endl
             << "WARNING: THIS WILL DISPLAY ALL YOUR PASSWORDS ON THE SCREEN "
             << "FOR THE FOLLOWING SERVICES: " << endl;
        ViewServices();

        cout << "ARE YOU SURE YOU WANT TO DO THIS? (Y)es ";
        string response;
        getline( cin, response );
        if( !response.compare("Y") )
        {
            for( map<string,string>::iterator iter = passwords_map.begin() ;
                    iter != passwords_map.end() ; ++iter )
            {
                cout << iter->first << ": " << iter->second << endl;
            }
            cout << endl;
        }
        return;
    }

    bool SaveAndQuit()
    {
        // encrypt the map
        stringstream stream;
        WriteMapToStream( stream );
        stringstream encrypted_stream;
        EncryptDecrypt( stream.str(), encrypted_stream );
        
        // create a unique tmp_file name
        stringstream tmp_file_stream;
        tmp_file_stream << passwords_file << ".tmp." << time(0);
        string tmp_file( tmp_file_stream.str() );

        // write the encrypted stream to the tmp_file and
        // then rename to passwords file
        ofstream outfile;
        outfile.exceptions( ofstream::failbit | ofstream::badbit );
        try
        {
            outfile.open( tmp_file.c_str() );
            outfile << encrypted_stream.str();
            outfile.close();
        }
        catch( ofstream::failure )
        {
            cout << "Exception writing the file: " << tmp_file << endl;
            return false;
        }

        // rename
        if( rename( tmp_file.c_str(), passwords_file.c_str() ) )
        {
            cout << "Could not rewrite the passwords file!" << endl
                 << "The passwords file is at: " << tmp_file << endl;
            return false;
        }
        return true;
    }

    bool ConfirmAndQuit()
    {
        cout << "Are you sure you want to quit without saving? (Y)es ";
        string response;
        getline( cin, response );
        if( !response.compare("Y") )
        {
            return true;
        }
        return false;
    }

    void RequestPass()
    {
        char* pass = getpass( "Enter Password:\n" );
        CryptoPP::SHA().CalculateDigest( digest, (byte*)pass, strlen( pass ) );
    }

    void CreateKeyFile()
    {
        RequestPass();
        ofstream outfile;
        outfile.exceptions( ofstream::failbit | ofstream::badbit );
        try
        {
            outfile.open( key_file.c_str() );
            outfile.write( (char*)digest, digest_size );
            outfile.close();
        }
        catch( ... )
        {
            cout << "Exception creating the key file" << endl;
            exit(1);
        }
        cout << "Key file successfully created!" << endl;
    }
}
