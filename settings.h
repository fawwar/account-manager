#pragma once

#include <string>


class Settings
{
public:
    std::string server_ip_;
    int         server_port_;
    std::string server_username_;
    std::string server_password_;
    std::string certificate_chain_file_path_;
    std::string private_key_path_;
    std::string dh_file_path_;
    int         enable_https_;

    std::string ivar_ip_;
    int         ivar_port_;
    std::string ivar_username_;
    std::string ivar_password_;

public:
    Settings(const std::string& path);
    ~Settings();

    int LoadSettingsFromPath(const std::string& path);
    int SaveSettingsToPath(const std::string& path);
};

