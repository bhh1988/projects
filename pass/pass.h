#ifndef PASS_H
#define PASS_H

#include <crypto++/sha.h>
#include <string>
#include <map>

namespace pass
{
    extern const std::string key_file;
    const int digest_size = CryptoPP::SHA::DIGESTSIZE;
    extern byte digest[ digest_size ];
    extern std::map<std::string,std::string> passwords_map;

    void HandleFailedPass();

    void EncryptDecrypt( const std::string& start_str, std::stringstream& out );
    
    void LoadMapFromStream( std::stringstream& stream );

    void WriteMapToStream( std::stringstream& stream );

    void LoadPasswords();

    void ViewServices();

    void ViewPass();

    void AddPass();

    void DeletePass();

    void ViewAll();

    bool SaveAndQuit();
    
    bool ConfirmAndQuit();

    void RequestPass();

    void CreateKeyFile();
}

#endif
