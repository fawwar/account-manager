#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/code_converter.hpp>

#include "settings.h"


Settings::Settings(const std::string& path)
    : server_ip_("0.0.0.0")
    , server_port_(8030)
    , server_username_("gorilla")
    , server_password_("gorilladev")
    , certificate_chain_file_path_("server.pem")
    , private_key_path_("server.pem")
    , dh_file_path_("dh2048.pem")
    , ivar_ip_("127.0.0.1")
    , ivar_port_(53333)
    , ivar_username_("controller")
    , ivar_password_("123456789")
    , enable_https_(0)
{
    LoadSettingsFromPath(path);
}

Settings::~Settings()
{
}

int Settings::LoadSettingsFromPath(const std::string& path)
{
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(path, pt);

        server_ip_        = pt.get("server.ip", server_ip_);
        server_port_      = pt.get("server.port", server_port_);
        server_username_  = pt.get("server.username", server_username_);
        server_password_  = pt.get("server.password", server_password_);
        certificate_chain_file_path_ =
            pt.get("server.certificate_chain_file_path",
            certificate_chain_file_path_);
        private_key_path_ = pt.get("server.private_key_path",
            private_key_path_);
        private_key_path_ = pt.get("server.dh_file_path", dh_file_path_);
        enable_https_     = pt.get("server.enable_https", enable_https_);

        ivar_ip_       = pt.get("ivar.ip", ivar_ip_);
        ivar_port_     = pt.get("ivar.port", ivar_port_);
        ivar_username_ = pt.get("ivar.username", ivar_username_);
        ivar_password_ = pt.get("ivar.password", ivar_password_);
    } catch (std::exception const& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Settings::SaveSettingsToPath(const std::string& path)
{
    try {
        boost::property_tree::ptree pt;

        pt.put("server.ip", server_ip_);
        pt.put("server.port", server_port_);
        pt.put("server.username", server_username_);
        pt.put("server.password", server_password_);
        pt.put("server.certificate_chain_file_path",
            certificate_chain_file_path_);
        pt.put("server.private_key_path", private_key_path_);
        pt.put("server.dh_file_path", dh_file_path_);
        pt.put("server.enable_https", enable_https_);

        pt.put("ivar.ip", ivar_ip_);
        pt.put("ivar.port", ivar_port_);
        pt.put("ivar.username", ivar_username_);
        pt.put("ivar.password", ivar_password_);

        boost::property_tree::ini_parser::write_ini(path, pt);
    } catch (std::exception const& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

