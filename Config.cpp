#include <fstream>
#include <iostream>
#include <string>
#include <json/json.h>
#include "Config.h"

#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"


using namespace gorilla::log;

Config::Config()
{
#ifdef TELSTRA
  permission ="CXsKCQoJICAgICAgIml2YXIiOiB7CgkgICAgICAgICJjaGFubmVscyI6IHsKCSAgICAgICAgICAiY2hhbm5lbCI6IFsKCSAgICAgICAgICAgIHsKCSAgICAgICAgICAgICAgImNoYW5uZWwiOiAwLAoJICAgICAgICAgICAgICAiYXNzaWduIiA6ICJTZXR0aW5nIiwKCSAgICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCgkgICAgICAgICAgICAgICJjb25kaXRpb24iIDogIlJlYWQiLAoJICAgICAgICAgICAgICAiaW5mb1NldHRpbmciIDogIlNldHRpbmciLAoJICAgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAoJICAgICAgICAgICAgICAicmVjb3JkU2V0dGluZyIgOiAiRGVueSIsCgkgICAgICAgICAgICAgICJzeXN0ZW1FdmVudCIgOiAiUmVhZCIsCgkgICAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKCSAgICAgICAgICAgICAgInZpc2libGUiIDogdHJ1ZQoJICAgICAgICAgICAgfSAgICAgICAgICAKCSAgICAgICAgICBdLAoJICAgICAgICAgICJ0ZW1wbGF0ZSI6IHsKCSAgICAgICAgICAgICJhc3NpZ24iIDogIlNldHRpbmciLAoJICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCgkgICAgICAgICAgICAiY29uZGl0aW9uIiA6ICJSZWFkIiwKCSAgICAgICAgICAgICJpbmZvU2V0dGluZyIgOiAiU2V0dGluZyIsCgkgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAoJICAgICAgICAgICAgInJlY29yZFNldHRpbmciIDogIlNldHRpbmciLAoJICAgICAgICAgICAgInN5c3RlbUV2ZW50IiA6ICJSZWFkIiwKCSAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKCSAgICAgICAgICAgICJ2aXNpYmxlIiA6IHRydWUKCSAgICAgICAgICB9CgkgICAgICAgIH0sCgkgICAgICAgICJldmVudHMiIDogewoJICAgICAgICAgICAgImVuYWJsZSIgOiAidHJ1ZSIsCgkgICAgICAgICAgICAiZXZlbnRNYW5hZ2VyU2V0dGluZyIgOiAiU2V0dGluZyIsCgkgICAgICAgICAgICAic3Vic2NyaXB0aW9uIiA6ICJSZWFkIgoJICAgICAgICAgfSwKCSAgICAgICAgICJpdmEiIDogewoJICAgICAgICAgICAgImFuYWx5c2lzIiA6ICJSZWFkIgoJICAgICAgICAgfSwKCSAgICAgICAgICJzeXN0ZW0iIDogewoJICAgICAgICAgICAgImFjY291bnQiIDogIkFjY291bnQiLAoJICAgICAgICAgICAgImJldGEiIDogIkRlbnkiLAoJICAgICAgICAgICAgImluZm8iIDogIlNldHRpbmciLAoJICAgICAgICAgICAgImxtcyIgOiAiU2V0dGluZyIsCgkgICAgICAgICAgICAibW1zIiA6ICJEZW55IiwKCSAgICAgICAgICAgICJyZWJvb3QiIDogIkFsbG93IiwKCSAgICAgICAgICAgICJzbWFydFJldGFpbFNlcnZpY2UiIDogIlNldHRpbmciLAoJICAgICAgICAgICAgInN0b3JhZ2UiIDogIkRlbnkiLAoJICAgICAgICAgICAgInRpbWUiIDogIlNldHRpbmciLAoJICAgICAgICAgICAgInVwZ3JhZGUiIDogIkFsbG93IgoJICAgICAgICAgfSwKCSAgICAgICAgICJ0dldhbGxzIiA6IHsKCSAgICAgICAgICAgICJlbmFibGUiIDogImZhbHNlIgoJICAgICAgICAgfQoJICAgICAgfSwKCSAgICAgICJ2bXMiOiB7CgkgICAgICAgICJ2aWV3IjogIlNldHRpbmciCgkgICAgICB9CgkgICAgCgkgIH0K";

account_regexpr = "(^[~!@\\$%\\^&\\*\\(\\)_\\{\\}\\|\\.a-zA-Z0-9]{1,32}$)";
password_regexpr = "(^[~!@\\$\\^&\\*\\(\\)_\\+\\{\\}\\|:\\\"<>\\?\'-=\\[\\];,\\.a-zA-Z0-9]{1,}$)";


#else
  permission = "ewoiaXZhciI6IHsKICAgICAgImNoYW5uZWxzIjogewogICAgICAgICJjaGFubmVsIjogWwogICAgICAgICAgewogICAgICAgICAgICAiY2hhbm5lbCI6IDAsCiAgICAgICAgICAgICJ2aXNpYmxlIjogdHJ1ZSwKICAgICAgICAgICAgImFzc2lnbiI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgInZpZGVvIjogIkxpdmUrUGxheWJhY2srQXJjaGl2ZSIsCiAgICAgICAgICAgICJjb25kaXRpb24iOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJjYW1lcmEiOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJpbmZvU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgInJlY29yZFNldHRpbmciOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJpdmFFdmVudCI6ICJSZWFkIiwKICAgICAgICAgICAgInN5c3RlbUV2ZW50IjogIlJlYWQiCiAgICAgICAgICB9CiAgICAgICAgXSwKICAgICAgICAidGVtcGxhdGUiOiB7CiAgICAgICAgICAidmlzaWJsZSI6IHRydWUsCiAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLAogICAgICAgICAgInZpZGVvIjogIkxpdmUrUGxheWJhY2srQXJjaGl2ZSIsCiAgICAgICAgICAiY29uZGl0aW9uIjogIlNldHRpbmciLAogICAgICAgICAgImNhbWVyYSI6ICJTZXR0aW5nIiwKICAgICAgICAgICJpbmZvU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogIlNldHRpbmciLAogICAgICAgICAgIml2YUV2ZW50IjogIlJlYWQiLAogICAgICAgICAgInN5c3RlbUV2ZW50IjogIlJlYWQiCiAgICAgICAgfQogICAgICB9LAogICAgICAidHZXYWxscyI6IHsKICAgICAgICAiZW5hYmxlIjogdHJ1ZQogICAgICB9LAogICAgICAiZXZlbnRzIjogewogICAgICAgICJlbmFibGUiOiB0cnVlLAogICAgICAgICJzdWJzY3JpcHRpb24iOiAiU2V0dGluZyIsCiAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciOiAiU2V0dGluZyIKICAgICAgfSwKICAgICAgIml2YSI6IHsKICAgICAgICAiYW5hbHlzaXMiOiAiU2V0dGluZyIKICAgICAgfSwKICAgICAgInN5c3RlbSI6IHsKICAgICAgICAiaW5mbyI6ICJTZXR0aW5nIiwKICAgICAgICAibW1zIjogIlNldHRpbmciLAogICAgICAgICJsbXMiOiAiU2V0dGluZyIsCiAgICAgICAgInN0b3JhZ2UiOiAiU2V0dGluZyIsCiAgICAgICAgInRpbWUiOiAiU2V0dGluZyIsCiAgICAgICAgImFjY291bnQiOiAiQWNjb3VudCtSb2xlIiwKICAgICAgICAicmVib290IjogIkFsbG93IiwKICAgICAgICAic21hcnRSZXRhaWxTZXJ2aWNlIjogIlNldHRpbmciLAogICAgICAgICJ1cGdyYWRlIjogIkFsbG93IiwKICAgICAgICAiYmV0YSI6ICJBbGxvdyIKICAgICAgfQogICAgfSwKICAgICJ2bXMiOiB7CiAgICAgICJ2aWV3IjogIlNldHRpbmciCiAgICB9Cn0=";

account_regexpr = "^[~\\\\!`@#\\$%\\^&\\*\\(\\)_\\+\\{\\}\\|:\\\"<>\\?\'-=\\[\\];,\\.a-zA-Z0-9]{1,32}$";
password_regexpr = "(^[~\\\\!`@#\\$%\\^&\\*\\(\\)_\\+\\{\\}\\|:\\\"<>\\?\'-=\\[\\];,\\.a-zA-Z0-9]{12,32}$)";


#endif
  try 
  {
    std::ifstream ifs("config.json");
    std::string text((std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()) );
    Json::Value root;
    Json::Reader reader;
    if(reader.parse(text, root))
    {
      if(root.isMember("permission"))
      {
        permission = root["permission"].asString();
      }
      
         
    }
  }
  catch(std::exception& e)
  {
    //file not exist or parse error
    std::cout << e.what() << std::endl;
  }
  DISABLE_PERMISSIONS = json::object_t {
    {"_", permission}
  };
  DISABLE_ACCESSRIGHT = json::object_t {
    {"accessRightName", ""},
    {"permissions", DISABLE_PERMISSIONS},
    {"description", ""}
  };
  DEFAULT_PERMISSIONS = json::object_t {
    {"_", permission}
  };
  DEFAULT_ACCESSRIGHT = json::object_t {
    {"accessRightName", ""},
    {"permissions", DEFAULT_PERMISSIONS},
    {"description", ""}
  };
}

Config& Config::getInstance()
{
  static Config m;
  return m;
}

