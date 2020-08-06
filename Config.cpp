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
  permission = "ewoKICAgICAgIml2YXIiOiB7CiAgICAgICAgImNoYW5uZWxzIjogewogICAgICAgICAgImNoYW5uZWwiOiBbCiAgICAgICAgICAgIHsKICAgICAgICAgICAgICAiY2hhbm5lbCI6IDAsCiAgICAgICAgICAgICAgImFzc2lnbiIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImNvbmRpdGlvbiIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImluZm9TZXR0aW5nIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIiA6ICJEZW55IiwKICAgICAgICAgICAgICAic3lzdGVtRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgICAidmlzaWJsZSIgOiB0cnVlCiAgICAgICAgICAgIH0gICAgICAgICAgCiAgICAgICAgICBdLAogICAgICAgICAgInRlbXBsYXRlIjogewogICAgICAgICAgICAiYXNzaWduIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCiAgICAgICAgICAgICJjb25kaXRpb24iIDogIlJlYWQiLAogICAgICAgICAgICAiaW5mb1NldHRpbmciIDogIlNldHRpbmciLAogICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAicmVjb3JkU2V0dGluZyIgOiAiRGVueSIsCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCIgOiAiUmVhZCIsCiAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgInZpc2libGUiIDogdHJ1ZQogICAgICAgICAgfQogICAgICAgIH0sCiAgICAgICAgImV2ZW50cyIgOiB7CiAgICAgICAgICAgICJlbmFibGUiIDogInRydWUiLAogICAgICAgICAgICAiZXZlbnRNYW5hZ2VyU2V0dGluZyIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJzdWJzY3JpcHRpb24iIDogIlJlYWQiCiAgICAgICAgIH0sCiAgICAgICAgICJpdmEiIDogewogICAgICAgICAgICAiYW5hbHlzaXMiIDogIlJlYWQiCiAgICAgICAgIH0sCiAgICAgICAgICJzeXN0ZW0iIDogewogICAgICAgICAgICAiYWNjb3VudCIgOiAiQWNjb3VudCIsCiAgICAgICAgICAgICJiZXRhIiA6ICJEZW55IiwKICAgICAgICAgICAgImluZm8iIDogIlNldHRpbmciLAogICAgICAgICAgICAibG1zIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgIm1tcyIgOiAiRGVueSIsCiAgICAgICAgICAgICJyZWJvb3QiIDogIkFsbG93IiwKICAgICAgICAgICAgInNtYXJ0UmV0YWlsU2VydmljZSIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJzdG9yYWdlIiA6ICJEZW55IiwKICAgICAgICAgICAgInRpbWUiIDogIlNldHRpbmciLAogICAgICAgICAgICAidXBncmFkZSIgOiAiQWxsb3ciCiAgICAgICAgIH0sCiAgICAgICAgICJ0dldhbGxzIiA6IHsKICAgICAgICAgICAgImVuYWJsZSIgOiAiZmFsc2UiCiAgICAgICAgIH0KICAgICAgfSwKICAgICAgInZtcyI6IHsKICAgICAgICAidmlldyI6ICJTZXR0aW5nIgogICAgICB9CiAgICAKICB9";

account_regexpr = "(^[~!@\\$%\\^&\\*\\(\\)_\\{\\}\\|\\.a-zA-Z0-9]{1,32}$)";
password_regexpr = "(^[~!@\\$\\^&\\*\\(\\)_\\+\\{\\}\\|:\\\"<>\\?\'-=\\[\\];,\\.a-zA-Z0-9]{12,32}$)";

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

