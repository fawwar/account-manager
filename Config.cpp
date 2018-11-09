#include <fstream>
#include <iostream>
#include <string>
#include <json/json.h>
#include "Config.h"

Config::Config()
{
  permission = "ewogICJhY2Nlc3NSaWdodE5hbWUiOiAiYWRtaW4iLAogICJkZXNjcmlwdGlvbiI6ICIiLAogICJwZXJtaXNzaW9ucyI6IHsKICAgICJpdmFyIjogewogICAgICAiY2hhbm5lbHMiOiB7CiAgICAgICAgImNoYW5uZWwiOiBbCiAgICAgICAgICB7CiAgICAgICAgICAgICJjaGFubmVsIjogMCwKICAgICAgICAgICAgInZpc2libGUiOiB0cnVlLAogICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLAogICAgICAgICAgICAidmlkZW8iOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgImNvbmRpdGlvbiI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgImNhbWVyYSI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgImluZm9TZXR0aW5nIjogIlNldHRpbmciLAogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgIml2YUV2ZW50IjogIlJlYWQiLAogICAgICAgICAgICAic3lzdGVtRXZlbnQiOiAiUmVhZCIKICAgICAgICAgIH0gICAgICAgICAgCiAgICAgICAgXSwKICAgICAgICAidGVtcGxhdGUiOiB7CiAgICAgICAgICAgICJ2aXNpYmxlIjogdHJ1ZSwKICAgICAgICAgICAgImFzc2lnbiI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgInZpZGVvIjogIkxpdmUrUGxheWJhY2srQXJjaGl2ZSIsCiAgICAgICAgICAgICJjb25kaXRpb24iOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJjYW1lcmEiOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJpbmZvU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgInJlY29yZFNldHRpbmciOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJpdmFFdmVudCI6ICJSZWFkIiwKICAgICAgICAgICAgInN5c3RlbUV2ZW50IjogIlJlYWQiCiAgICAgICAgfQogICAgICB9LAogICAgICAKICAgICAgInR2V2FsbHMiOiB7CiAgICAgICAgImVuYWJsZSI6IHRydWUKICAgICAgfSwKICAgICAgImV2ZW50cyI6IHsKICAgICAgICAiZW5hYmxlIjogdHJ1ZSwKICAgICAgICAic3Vic2NyaXB0aW9uIjogIlNldHRpbmciLAogICAgICAgICJldmVudE1hbmFnZXJTZXR0aW5nIjogIlNldHRpbmciCiAgICAgIH0sCiAgICAgICJpdmEiIDogewogICAgICAgICJhbmFseXNpcyIgOiAiU2V0dGluZyIKICAgICB9LAogICAgICAic3lzdGVtIjogewogICAgICAgICJpbmZvIjogIlNldHRpbmciLAogICAgICAgICJtbXMiOiAiU2V0dGluZyIsCiAgICAgICAgImxtcyI6ICJTZXR0aW5nIiwKICAgICAgICAic3RvcmFnZSI6ICJTZXR0aW5nIiwKICAgICAgICAidGltZSI6ICJTZXR0aW5nIiwKICAgICAgICAiYWNjb3VudCI6ICJBY2NvdW50K1JvbGUiLAogICAgICAgICJyZWJvb3QiOiAiQWxsb3ciLAogICAgICAgICJzbWFydFJldGFpbFNlcnZpY2UiIDogIlNldHRpbmciLAogICAgICAgICJ1cGdyYWRlIjogIkFsbG93IiwgCiAgICAgICAgImJldGEiOiAiQWxsb3ciICAKICAgICAgfQogICAgfSwKICAgICJ2bXMiOiB7CiAgICAgIAogICAgfQogIH0KfQ==";
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

