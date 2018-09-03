#pragma once

#include <string>

#include "./import/json-develop/json.hpp"

using json = nlohmann::json;

namespace gorilla {
    namespace account{

        enum Error
        {
            SUCCESS_RESPONSE = 200, 
            PET_DELETE = 204,
            BAD_REQUEST = 400,
            UNAUTHORIZED = 401,    
            FORBIDDEN = 403,    
            NAME_NOT_FOUND = 404,
            INTERNAL_SERVER_ERROR = 500   
        };

        enum SQLError
        {
            CONSTRAINT = 19
        };
        /*
        const json::object_t DEFAULT_USER
        {
            {"account", ""}, 
            {"password", ""}, 
            {"accessRightName", ""},
            {"description", ""}
        };
        */
        const json::object_t DEFAULT_RESPONSE_USER
        {
            {"account", ""}, 
            {"encryptedPassword", ""}, 
            {"accessRightName", ""},
            {"description", ""}
            //{"name", ""}
        };

        const json::object_t DISABLE_IVAR_PERMISSIONS
        {
            {"getinfo", false}, 
            {"setinfo", false}, 
            {"task", false},
            {"live", json::array()},
            {"playback", json::array()},
            {"archive", json::array()},
            {"getRecord", false}, 
            {"setRecord", false}, 
            {"getSystem", false},
            {"setSystem", false}
        };

        const json::object_t DISABLE_IOT_PERMISSIONS
        {
            {"get", false}, 
            {"set", false}, 
            {"getSubscription", false},
            {"setSubscription", false}
        };

        const json::object_t DISABLE_PERMISSIONS
        {
//            {"ivar", DISABLE_IVAR_PERMISSIONS}, 
//            {"iot", DISABLE_IOT_PERMISSIONS}
//            {"_", "ew0KICAgICJpdmFyIjogew0KICAgICAgImNoYW5uZWxzIjogew0KICAgICAgICAiY2hhbm5lbCI6IFsNCiAgICAgICAgICB7DQogICAgICAgICAgICAiY2hhbm5lbCI6IDAsDQogICAgICAgICAgICAidmlzaWJsZSI6IHRydWUsDQogICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLA0KICAgICAgICAgICAgInZpZGVvIjogIkxpdmUrUGxheWJhY2srQXJjaGl2ZSIsDQogICAgICAgICAgICAiY29uZGl0aW9uIjogIlNldHRpbmciLA0KICAgICAgICAgICAgImNhbWVyYSI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJpbmZvU2V0dGluZyI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogIlNldHRpbmciLA0KICAgICAgICAgICAgIml2YUV2ZW50IjogIlJlYWQiLA0KICAgICAgICAgICAgInN5c3RlbUV2ZW50IjogIlJlYWQiDQogICAgICAgICAgfSAgICAgICAgICANCiAgICAgICAgXSwNCiAgICAgICAgInRlbXBsYXRlIjogew0KICAgICAgICAgICAgInZpc2libGUiOiB0cnVlLA0KICAgICAgICAgICAgImFzc2lnbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJ2aWRlbyI6ICJMaXZlK1BsYXliYWNrK0FyY2hpdmUiLA0KICAgICAgICAgICAgImNvbmRpdGlvbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJjYW1lcmEiOiAiU2V0dGluZyIsDQogICAgICAgICAgICAiaW5mb1NldHRpbmciOiAiU2V0dGluZyIsDQogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJpdmFFdmVudCI6ICJSZWFkIiwNCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6ICJSZWFkIg0KICAgICAgICB9DQogICAgICB9LA0KICAgICAgDQogICAgICAidHZXYWxscyI6IHsNCiAgICAgICAgImVuYWJsZSI6IHRydWUNCiAgICAgIH0sDQogICAgICAiZXZlbnRzIjogew0KICAgICAgICAiZW5hYmxlIjogdHJ1ZSwNCiAgICAgICAgInN1YnNjcmlwdGlvbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciOiAiU2V0dGluZyINCiAgICAgIH0sDQogICAgICAic3lzdGVtIjogew0KICAgICAgICAiaW5mbyI6ICJTZXR0aW5nIiwNCiAgICAgICAgIm1tcyI6ICJTZXR0aW5nIiwNCiAgICAgICAgImxtcyI6ICJTZXR0aW5nIiwNCiAgICAgICAgInN0b3JhZ2UiOiAiU2V0dGluZyIsDQogICAgICAgICJ0aW1lIjogIlNldHRpbmciLA0KICAgICAgICAiYWNjb3VudCI6ICJBY2NvdW50K1JvbGUiLA0KICAgICAgICAicmVib290IjogIkFsbG93IiwNCiAgICAgICAgInVwZ3JhZGUiOiAiQWxsb3ciLCANCiAgICAgICAgImJldGEiOiAiQWxsb3ciICANCiAgICAgIH0NCiAgICB9LA0KICAgICJ2bXMiOiB7DQogICAgICANCiAgICB9DQogIH0NCg=="}
              {"_", "ewogICAgImFjY2Vzc1JpZ2h0TmFtZSI6ICJhZG1pbiIsCiAgICAiZGVzY3JpcHRpb24iOiAiIiwKICAgICJwZXJtaXNzaW9ucyI6IHsKICAgICAgIml2YXIiOiB7CiAgICAgICAgImNoYW5uZWxzIjogewogICAgICAgICAgImNoYW5uZWwiOiBbCiAgICAgICAgICAgIHsKICAgICAgICAgICAgICAiY2hhbm5lbCI6IDAsCiAgICAgICAgICAgICAgImFzc2lnbiIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImNvbmRpdGlvbiIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImluZm9TZXR0aW5nIiA6ICJSZWFkIiwKICAgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIiA6ICJEZW55IiwKICAgICAgICAgICAgICAic3lzdGVtRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgICAidmlzaWJsZSIgOiB0cnVlCiAgICAgICAgICAgIH0gICAgICAgICAgCiAgICAgICAgICBdLAogICAgICAgICAgInRlbXBsYXRlIjogewogICAgICAgICAgICAiYXNzaWduIiA6ICJSZWFkIiwKICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCiAgICAgICAgICAgICJjb25kaXRpb24iIDogIlNldHRpbmciLAogICAgICAgICAgICAiaW5mb1NldHRpbmciIDogIlJlYWQiLAogICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAicmVjb3JkU2V0dGluZyIgOiAiRGVueSIsCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCIgOiAiUmVhZCIsCiAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgInZpc2libGUiIDogdHJ1ZQogICAgICAgICAgfQogICAgICAgIH0sCiAgICAgICAgImV2ZW50cyIgOiB7CiAgICAgICAgICAgICJlbmFibGUiIDogInRydWUiLAogICAgICAgICAgICAiZXZlbnRNYW5hZ2VyU2V0dGluZyIgOiAiUmVhZCIsCiAgICAgICAgICAgICJzdWJzY3JpcHRpb24iIDogIlJlYWQiCiAgICAgICAgIH0sCiAgICAgICAgICJpdmEiIDogewogICAgICAgICAgICAiYW5hbHlzaXMiIDogIlJlYWQiCiAgICAgICAgIH0sCiAgICAgICAgICJzeXN0ZW0iIDogewogICAgICAgICAgICAiYWNjb3VudCIgOiAiQWNjb3VudCIsCiAgICAgICAgICAgICJiZXRhIiA6ICJEZW55IiwKICAgICAgICAgICAgImluZm8iIDogIlNldHRpbmciLAogICAgICAgICAgICAibG1zIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgIm1tcyIgOiAiRGVueSIsCiAgICAgICAgICAgICJyZWJvb3QiIDogIkFsbG93IiwKICAgICAgICAgICAgInNtYXJ0UmV0YWlsU2VydmljZSIgOiAiRGVueSIsCiAgICAgICAgICAgICJzdG9yYWdlIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgInRpbWUiIDogIlNldHRpbmciLAogICAgICAgICAgICAidXBncmFkZSIgOiAiQWxsb3ciCiAgICAgICAgIH0sCiAgICAgICAgICJ0dldhbGxzIiA6IHsKICAgICAgICAgICAgImVuYWJsZSIgOiAiZmFsc2UiCiAgICAgICAgIH0KICAgICAgfSwKICAgICAgInZtcyIgOiB7fQogICAgfQogIH0="}
        };

        const json::object_t DISABLE_ACCESSRIGHT
        {
            {"accessRightName", ""}, 
            {"permissions", DISABLE_PERMISSIONS},
            {"description", ""}
        };
        

        const json::object_t DEFAULT_IVAR_PERMISSIONS
        {
            {"getinfo", true}, 
            {"setinfo", true}, 
            {"task", true},
            {"live", "all"},
            {"playback", "all"},
            {"archive", "all"},
            {"getRecord", true}, 
            {"setRecord", true}, 
            {"getSystem", true},
            {"setSystem", true}
        };

        const json::object_t DEFAULT_IOT_PERMISSIONS
        {
            {"get", true}, 
            {"set", true}, 
            {"getSubscription", true},
            {"setSubscription", true}
        };

        const json::object_t DEFAULT_PERMISSIONS
        {
        /*
            {"ivar", DEFAULT_IVAR_PERMISSIONS}, 
            {"iot", DEFAULT_IOT_PERMISSIONS}
        */
        //    {"_", "ew0KICAgICJpdmFyIjogew0KICAgICAgImNoYW5uZWxzIjogew0KICAgICAgICAiY2hhbm5lbCI6IFsNCiAgICAgICAgICB7DQogICAgICAgICAgICAiY2hhbm5lbCI6IDAsDQogICAgICAgICAgICAidmlzaWJsZSI6IHRydWUsDQogICAgICAgICAgICAiYXNzaWduIjogIlNldHRpbmciLA0KICAgICAgICAgICAgInZpZGVvIjogIkxpdmUrUGxheWJhY2srQXJjaGl2ZSIsDQogICAgICAgICAgICAiY29uZGl0aW9uIjogIlNldHRpbmciLA0KICAgICAgICAgICAgImNhbWVyYSI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJpbmZvU2V0dGluZyI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogIlNldHRpbmciLA0KICAgICAgICAgICAgIml2YUV2ZW50IjogIlJlYWQiLA0KICAgICAgICAgICAgInN5c3RlbUV2ZW50IjogIlJlYWQiDQogICAgICAgICAgfSAgICAgICAgICANCiAgICAgICAgXSwNCiAgICAgICAgInRlbXBsYXRlIjogew0KICAgICAgICAgICAgInZpc2libGUiOiB0cnVlLA0KICAgICAgICAgICAgImFzc2lnbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJ2aWRlbyI6ICJMaXZlK1BsYXliYWNrK0FyY2hpdmUiLA0KICAgICAgICAgICAgImNvbmRpdGlvbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJjYW1lcmEiOiAiU2V0dGluZyIsDQogICAgICAgICAgICAiaW5mb1NldHRpbmciOiAiU2V0dGluZyIsDQogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6ICJTZXR0aW5nIiwNCiAgICAgICAgICAgICJpdmFFdmVudCI6ICJSZWFkIiwNCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6ICJSZWFkIg0KICAgICAgICB9DQogICAgICB9LA0KICAgICAgDQogICAgICAidHZXYWxscyI6IHsNCiAgICAgICAgImVuYWJsZSI6IHRydWUNCiAgICAgIH0sDQogICAgICAiZXZlbnRzIjogew0KICAgICAgICAiZW5hYmxlIjogdHJ1ZSwNCiAgICAgICAgInN1YnNjcmlwdGlvbiI6ICJTZXR0aW5nIiwNCiAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciOiAiU2V0dGluZyINCiAgICAgIH0sDQogICAgICAic3lzdGVtIjogew0KICAgICAgICAiaW5mbyI6ICJTZXR0aW5nIiwNCiAgICAgICAgIm1tcyI6ICJTZXR0aW5nIiwNCiAgICAgICAgImxtcyI6ICJTZXR0aW5nIiwNCiAgICAgICAgInN0b3JhZ2UiOiAiU2V0dGluZyIsDQogICAgICAgICJ0aW1lIjogIlNldHRpbmciLA0KICAgICAgICAiYWNjb3VudCI6ICJBY2NvdW50K1JvbGUiLA0KICAgICAgICAicmVib290IjogIkFsbG93IiwNCiAgICAgICAgInVwZ3JhZGUiOiAiQWxsb3ciLCANCiAgICAgICAgImJldGEiOiAiQWxsb3ciICANCiAgICAgIH0NCiAgICB9LA0KICAgICJ2bXMiOiB7DQogICAgICANCiAgICB9DQogIH0NCg=="}
              {"_", "ewogICAgImFjY2Vzc1JpZ2h0TmFtZSI6ICJhZG1pbiIsCiAgICAiZGVzY3JpcHRpb24iOiAiIiwKICAgICJwZXJtaXNzaW9ucyI6IHsKICAgICAgIml2YXIiOiB7CiAgICAgICAgImNoYW5uZWxzIjogewogICAgICAgICAgImNoYW5uZWwiOiBbCiAgICAgICAgICAgIHsKICAgICAgICAgICAgICAiY2hhbm5lbCI6IDAsCiAgICAgICAgICAgICAgImFzc2lnbiIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImNvbmRpdGlvbiIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgImluZm9TZXR0aW5nIiA6ICJSZWFkIiwKICAgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIiA6ICJEZW55IiwKICAgICAgICAgICAgICAic3lzdGVtRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgICAidmlzaWJsZSIgOiB0cnVlCiAgICAgICAgICAgIH0gICAgICAgICAgCiAgICAgICAgICBdLAogICAgICAgICAgInRlbXBsYXRlIjogewogICAgICAgICAgICAiYXNzaWduIiA6ICJSZWFkIiwKICAgICAgICAgICAgImNhbWVyYSIgOiAiUmVhZCIsCiAgICAgICAgICAgICJjb25kaXRpb24iIDogIlNldHRpbmciLAogICAgICAgICAgICAiaW5mb1NldHRpbmciIDogIlJlYWQiLAogICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAicmVjb3JkU2V0dGluZyIgOiAiRGVueSIsCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCIgOiAiUmVhZCIsCiAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgInZpc2libGUiIDogdHJ1ZQogICAgICAgICAgfQogICAgICAgIH0sCiAgICAgICAgImV2ZW50cyIgOiB7CiAgICAgICAgICAgICJlbmFibGUiIDogInRydWUiLAogICAgICAgICAgICAiZXZlbnRNYW5hZ2VyU2V0dGluZyIgOiAiUmVhZCIsCiAgICAgICAgICAgICJzdWJzY3JpcHRpb24iIDogIlJlYWQiCiAgICAgICAgIH0sCiAgICAgICAgICJpdmEiIDogewogICAgICAgICAgICAiYW5hbHlzaXMiIDogIlJlYWQiCiAgICAgICAgIH0sCiAgICAgICAgICJzeXN0ZW0iIDogewogICAgICAgICAgICAiYWNjb3VudCIgOiAiQWNjb3VudCIsCiAgICAgICAgICAgICJiZXRhIiA6ICJEZW55IiwKICAgICAgICAgICAgImluZm8iIDogIlNldHRpbmciLAogICAgICAgICAgICAibG1zIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgIm1tcyIgOiAiRGVueSIsCiAgICAgICAgICAgICJyZWJvb3QiIDogIkFsbG93IiwKICAgICAgICAgICAgInNtYXJ0UmV0YWlsU2VydmljZSIgOiAiRGVueSIsCiAgICAgICAgICAgICJzdG9yYWdlIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgInRpbWUiIDogIlNldHRpbmciLAogICAgICAgICAgICAidXBncmFkZSIgOiAiQWxsb3ciCiAgICAgICAgIH0sCiAgICAgICAgICJ0dldhbGxzIiA6IHsKICAgICAgICAgICAgImVuYWJsZSIgOiAiZmFsc2UiCiAgICAgICAgIH0KICAgICAgfSwKICAgICAgInZtcyIgOiB7fQogICAgfQogIH0="}
        };

        const json::object_t DEFAULT_ACCESSRIGHT
        {
            {"accessRightName", ""}, 
            {"permissions", DEFAULT_PERMISSIONS},
            {"description", ""}
        };

        const nlohmann::json::object_t DEFAULT_ERROR = 
        {
            {"userMessage",""}, 
            {"internalMessage",""}
        };

     }
}

