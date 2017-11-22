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
              {"_", "ewogICAiaXZhciIgOiB7CiAgICAgICJjaGFubmVscyIgOiB7CiAgICAgICAgICJjaGFubmVsIiA6IFsKICAgICAgICAgICAgewogICAgICAgICAgICAgICAiYXNzaWduIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgICAgImNhbWVyYSIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgICJjaGFubmVsIiA6IDEsCiAgICAgICAgICAgICAgICJjb25kaXRpb24iIDogIlNldHRpbmciLAogICAgICAgICAgICAgICAiaW5mb1NldHRpbmciIDogIlNldHRpbmciLAogICAgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICAicmVjb3JkU2V0dGluZyIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgICJzeXN0ZW1FdmVudCIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgICAgInZpc2libGUiIDogdHJ1ZQogICAgICAgICAgICB9CiAgICAgICAgIF0KICAgICAgfSwKICAgICAgImV2ZW50cyIgOiB7CiAgICAgICAgICJlbmFibGUiIDogdHJ1ZSwKICAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciIDogIlNldHRpbmciLAogICAgICAgICAic3Vic2NyaXB0aW9uIiA6ICJTZXR0aW5nIgogICAgICB9LAogICAgICAic3lzdGVtIiA6IHsKICAgICAgICAgImFjY291bnQiIDogIkFjY291bnQrUm9sZSIsCiAgICAgICAgICJiZXRhIiA6ICJBbGxvdyIsCiAgICAgICAgICJpbmZvIiA6ICJTZXR0aW5nIiwKICAgICAgICAgImxtcyIgOiAiU2V0dGluZyIsCiAgICAgICAgICJtbXMiIDogIlNldHRpbmciLAogICAgICAgICAicmVib290IiA6ICJBbGxvdyIsCiAgICAgICAgICJzdG9yYWdlIiA6ICJTZXR0aW5nIiwKICAgICAgICAgInRpbWUiIDogIlNldHRpbmciLAogICAgICAgICAidXBncmFkZSIgOiAiQWxsb3ciCiAgICAgIH0sCiAgICAgICJ0dldhbGxzIiA6IHsKICAgICAgICAgImVuYWJsZSIgOiB0cnVlCiAgICAgIH0KICAgfSwKICAgInZtcyIgOiB7fQp9Cg=="}
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
            {"_", "ewogICAiaXZhciIgOiB7CiAgICAgICJjaGFubmVscyIgOiB7CiAgICAgICAgICJjaGFubmVsIiA6IFsKICAgICAgICAgICAgewogICAgICAgICAgICAgICAiYXNzaWduIiA6ICJTZXR0aW5nIiwKICAgICAgICAgICAgICAgImNhbWVyYSIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgICJjaGFubmVsIiA6IDEsCiAgICAgICAgICAgICAgICJjb25kaXRpb24iIDogIlNldHRpbmciLAogICAgICAgICAgICAgICAiaW5mb1NldHRpbmciIDogIlNldHRpbmciLAogICAgICAgICAgICAgICAiaXZhRXZlbnQiIDogIlJlYWQiLAogICAgICAgICAgICAgICAicmVjb3JkU2V0dGluZyIgOiAiU2V0dGluZyIsCiAgICAgICAgICAgICAgICJzeXN0ZW1FdmVudCIgOiAiUmVhZCIsCiAgICAgICAgICAgICAgICJ2aWRlbyIgOiAiTGl2ZStQbGF5YmFjaytBcmNoaXZlIiwKICAgICAgICAgICAgICAgInZpc2libGUiIDogdHJ1ZQogICAgICAgICAgICB9CiAgICAgICAgIF0KICAgICAgfSwKICAgICAgImV2ZW50cyIgOiB7CiAgICAgICAgICJlbmFibGUiIDogdHJ1ZSwKICAgICAgICAgImV2ZW50TWFuYWdlclNldHRpbmciIDogIlNldHRpbmciLAogICAgICAgICAic3Vic2NyaXB0aW9uIiA6ICJTZXR0aW5nIgogICAgICB9LAogICAgICAic3lzdGVtIiA6IHsKICAgICAgICAgImFjY291bnQiIDogIkFjY291bnQrUm9sZSIsCiAgICAgICAgICJiZXRhIiA6ICJBbGxvdyIsCiAgICAgICAgICJpbmZvIiA6ICJTZXR0aW5nIiwKICAgICAgICAgImxtcyIgOiAiU2V0dGluZyIsCiAgICAgICAgICJtbXMiIDogIlNldHRpbmciLAogICAgICAgICAicmVib290IiA6ICJBbGxvdyIsCiAgICAgICAgICJzdG9yYWdlIiA6ICJTZXR0aW5nIiwKICAgICAgICAgInRpbWUiIDogIlNldHRpbmciLAogICAgICAgICAidXBncmFkZSIgOiAiQWxsb3ciCiAgICAgIH0sCiAgICAgICJ0dldhbGxzIiA6IHsKICAgICAgICAgImVuYWJsZSIgOiB0cnVlCiAgICAgIH0KICAgfSwKICAgInZtcyIgOiB7fQp9Cg=="}
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

