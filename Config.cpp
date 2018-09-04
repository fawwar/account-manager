#include <fstream>
#include <iostream>
#include <string>
#include <json/json.h>
#include "Config.h"

Config::Config()
{
  permission = "ew0KICAgICJpdmFyIjogew0KICAgICAgImNoYW5uZWxzIjogew0KICAgICAgICAiY2hhbm5lbCI6IFsNCiAgICAgICAgICB7DQogICAgICAgICAgICAiY2hhbm5lbCI6IDAsDQogICAgICAgICAgICAidmlzaWJsZSI6IHRydWUsDQogICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLA0KICAgICAgICAgICAgInZpZGVvIjogIkxpdmUrUGxheWJhY2srQXJjaGl2ZSIsDQogICAgICAgICAgICAiY29uZGl0aW9uIjogIlNldHRpbmciLA0KICAgICAgICAgICAgImNhbWVyYSI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJpbmZvU2V0dGluZyI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogIlNldHRpbmciLA0KICAgICAgICAgICAgIml2YUV2ZW50IjogIlJlYWQiLA0KICAgICAgICAgICAgInN5c3RlbUV2ZW50IjogIlJlYWQiDQogICAgICAgICAgfSAgICAgICAgICANCiAgICAgICAgXSwNCiAgICAgICAgInRlbXBsYXRlIjogew0KICAgICAgICAgICAgInZpc2libGUiOiB0cnVlLA0KICAgICAgICAgICAgImFzc2lnbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJ2aWRlbyI6ICJMaXZlK1BsYXliYWNrK0FyY2hpdmUiLA0KICAgICAgICAgICAgImNvbmRpdGlvbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJjYW1lcmEiOiAiU2V0dGluZyIsDQogICAgICAgICAgICAiaW5mb1NldHRpbmciOiAiU2V0dGluZyIsDQogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJpdmFFdmVudCI6ICJSZWFkIiwNCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6ICJSZWFkIg0KICAgICAgICB9DQogICAgICB9LA0KICAgICAgDQogICAgICAidHZXYWxscyI6IHsNCiAgICAgICAgImVuYWJsZSI6IHRydWUNCiAgICAgIH0sDQogICAgICAiZXZlbnRzIjogew0KICAgICAgICAiZW5hYmxlIjogdHJ1ZSwNCiAgICAgICAgInN1YnNjcmlwdGlvbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciOiAiU2V0dGluZyINCiAgICAgIH0sDQogICAgICAic3lzdGVtIjogew0KICAgICAgICAiaW5mbyI6ICJTZXR0aW5nIiwNCiAgICAgICAgIm1tcyI6ICJTZXR0aW5nIiwNCiAgICAgICAgImxtcyI6ICJTZXR0aW5nIiwNCiAgICAgICAgInN0b3JhZ2UiOiAiU2V0dGluZyIsDQogICAgICAgICJ0aW1lIjogIlNldHRpbmciLA0KICAgICAgICAiYWNjb3VudCI6ICJBY2NvdW50K1JvbGUiLA0KICAgICAgICAicmVib290IjogIkFsbG93IiwNCiAgICAgICAgInVwZ3JhZGUiOiAiQWxsb3ciLCANCiAgICAgICAgImJldGEiOiAiQWxsb3ciICANCiAgICAgIH0NCiAgICB9LA0KICAgICJ2bXMiOiB7DQogICAgICANCiAgICB9DQogIH0NCg==";
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

