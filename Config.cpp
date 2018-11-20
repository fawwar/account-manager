#include <fstream>
#include <iostream>
#include <string>
#include <json/json.h>
#include "Config.h"

Config::Config()
{
  permission = "ewogICAgIml2YXIiOiB7CiAgICAgICJjaGFubmVscyI6IHsKICAgICAgICAiY2hhbm5lbCI6IFsKICAgICAgICAgIHsKICAgICAgICAgICAgImNoYW5uZWwiOiAwLAogICAgICAgICAgICAidmlzaWJsZSI6IHRydWUsCiAgICAgICAgICAgICJhc3NpZ24iOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJ2aWRlbyI6ICJMaXZlK1BsYXliYWNrK0FyY2hpdmUiLAogICAgICAgICAgICAiY29uZGl0aW9uIjogIlNldHRpbmciLAogICAgICAgICAgICAiY2FtZXJhIjogIlNldHRpbmciLAogICAgICAgICAgICAiaW5mb1NldHRpbmciOiAiU2V0dGluZyIsCiAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogIlNldHRpbmciLAogICAgICAgICAgICAiaXZhRXZlbnQiOiAiUmVhZCIsCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6ICJSZWFkIgogICAgICAgICAgfSAgICAgICAgICAKICAgICAgICBdLAogICAgICAgICJ0ZW1wbGF0ZSI6IHsKICAgICAgICAgICAgInZpc2libGUiOiB0cnVlLAogICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLAogICAgICAgICAgICAidmlkZW8iOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgImNvbmRpdGlvbiI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgImNhbWVyYSI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgImluZm9TZXR0aW5nIjogIlNldHRpbmciLAogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwKICAgICAgICAgICAgIml2YUV2ZW50IjogIlJlYWQiLAogICAgICAgICAgICAic3lzdGVtRXZlbnQiOiAiUmVhZCIKICAgICAgICB9CiAgICAgIH0sCiAgICAgIAogICAgICAidHZXYWxscyI6IHsKICAgICAgICAiZW5hYmxlIjogdHJ1ZQogICAgICB9LAogICAgICAiZXZlbnRzIjogewogICAgICAgICJlbmFibGUiOiB0cnVlLAogICAgICAgICJzdWJzY3JpcHRpb24iOiAiU2V0dGluZyIsCiAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciOiAiU2V0dGluZyIKICAgICAgfSwKICAgICAgIml2YSIgOiB7CiAgICAgICAgImFuYWx5c2lzIiA6ICJTZXR0aW5nIgogICAgIH0sCiAgICAgICJzeXN0ZW0iOiB7CiAgICAgICAgImluZm8iOiAiU2V0dGluZyIsCiAgICAgICAgIm1tcyI6ICJTZXR0aW5nIiwKICAgICAgICAibG1zIjogIlNldHRpbmciLAogICAgICAgICJzdG9yYWdlIjogIlNldHRpbmciLAogICAgICAgICJ0aW1lIjogIlNldHRpbmciLAogICAgICAgICJhY2NvdW50IjogIkFjY291bnQrUm9sZSIsCiAgICAgICAgInJlYm9vdCI6ICJBbGxvdyIsCiAgICAgICAgInNtYXJ0UmV0YWlsU2VydmljZSIgOiAiU2V0dGluZyIsCiAgICAgICAgInVwZ3JhZGUiOiAiQWxsb3ciLCAKICAgICAgICAiYmV0YSI6ICJBbGxvdyIgIAogICAgICB9CiAgICB9LAogICAgInZtcyI6IHsKICAgICAgCiAgICB9CiAgfQ==";
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
