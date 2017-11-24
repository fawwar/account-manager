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
              {"_", "ew0KICAgICJpdmFyIjogew0KICAgICAgImNoYW5uZWxzIjogew0KICAgICAgICAiY2hhbm5lbCI6IFsNCiAgICAgICAgICB7DQogICAgICAgICAgICAiY2hhbm5lbCI6IDAsDQogICAgICAgICAgICAidmlzaWJsZSI6IHRydWUsDQogICAgICAgICAgICAiYXNzaWduIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAidmlkZW8iOiBbIkxpdmUrUGxheWJhY2srQXJjaGl2ZSJdLA0KICAgICAgICAgICAgImNvbmRpdGlvbiI6IFsiU2V0dGluZyJdLA0KICAgICAgICAgICAgImNhbWVyYSI6IFsiU2V0dGluZyJdLA0KICAgICAgICAgICAgImluZm9TZXR0aW5nIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6IFsiU2V0dGluZyJdLA0KICAgICAgICAgICAgIml2YUV2ZW50IjogWyJSZWFkIl0sDQogICAgICAgICAgICAic3lzdGVtRXZlbnQiOiBbIlJlYWQiXQ0KICAgICAgICAgIH0gICAgICAgICAgDQogICAgICAgIF0sDQogICAgICAgICJ0ZW1wbGF0ZSI6IHsNCiAgICAgICAgICAgICJ2aXNpYmxlIjogdHJ1ZSwNCiAgICAgICAgICAgICJhc3NpZ24iOiBbIlNldHRpbmciXSwNCiAgICAgICAgICAgICJ2aWRlbyI6IFsiTGl2ZStQbGF5YmFjaytBcmNoaXZlIl0sDQogICAgICAgICAgICAiY29uZGl0aW9uIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAiY2FtZXJhIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAiaW5mb1NldHRpbmciOiBbIlNldHRpbmciXSwNCiAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAiaXZhRXZlbnQiOiBbIlJlYWQiXSwNCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6IFsiUmVhZCJdDQogICAgICAgIH0NCiAgICAgIH0sDQogICAgICANCiAgICAgICJ0dldhbGxzIjogew0KICAgICAgICAiZW5hYmxlIjogdHJ1ZQ0KICAgICAgfSwNCiAgICAgICJldmVudHMiOiB7DQogICAgICAgICJlbmFibGUiOiB0cnVlLA0KICAgICAgICAic3Vic2NyaXB0aW9uIjogWyJTZXR0aW5nIl0sDQogICAgICAgICJldmVudE1hbmFnZXJTZXR0aW5nIjogWyJTZXR0aW5nIl0NCiAgICAgIH0sDQogICAgICAic3lzdGVtIjogew0KICAgICAgICAiaW5mbyI6IFsiU2V0dGluZyJdLA0KICAgICAgICAibW1zIjogWyJTZXR0aW5nIl0sDQogICAgICAgICJsbXMiOiBbIlNldHRpbmciXSwNCiAgICAgICAgInN0b3JhZ2UiOiBbIlNldHRpbmciXSwNCiAgICAgICAgInRpbWUiOiBbIlNldHRpbmciXSwNCiAgICAgICAgImFjY291bnQiOiBbIkFjY291bnQrUm9sZSJdLA0KICAgICAgICAicmVib290IjogWyJBbGxvdyJdLA0KICAgICAgICAidXBncmFkZSI6IFsiQWxsb3ciXSwgDQogICAgICAgICJiZXRhIjogWyJBbGxvdyJdICANCiAgICAgIH0NCiAgICB9LA0KICAgICJ2bXMiOiB7DQogICAgICANCiAgICB9DQogIH0NCg=="}
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
            {"_", "ew0KICAgICJpdmFyIjogew0KICAgICAgImNoYW5uZWxzIjogew0KICAgICAgICAiY2hhbm5lbCI6IFsNCiAgICAgICAgICB7DQogICAgICAgICAgICAiY2hhbm5lbCI6IDAsDQogICAgICAgICAgICAidmlzaWJsZSI6IHRydWUsDQogICAgICAgICAgICAiYXNzaWduIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAidmlkZW8iOiBbIkxpdmUrUGxheWJhY2srQXJjaGl2ZSJdLA0KICAgICAgICAgICAgImNvbmRpdGlvbiI6IFsiU2V0dGluZyJdLA0KICAgICAgICAgICAgImNhbWVyYSI6IFsiU2V0dGluZyJdLA0KICAgICAgICAgICAgImluZm9TZXR0aW5nIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAicmVjb3JkU2V0dGluZyI6IFsiU2V0dGluZyJdLA0KICAgICAgICAgICAgIml2YUV2ZW50IjogWyJSZWFkIl0sDQogICAgICAgICAgICAic3lzdGVtRXZlbnQiOiBbIlJlYWQiXQ0KICAgICAgICAgIH0gICAgICAgICAgDQogICAgICAgIF0sDQogICAgICAgICJ0ZW1wbGF0ZSI6IHsNCiAgICAgICAgICAgICJ2aXNpYmxlIjogdHJ1ZSwNCiAgICAgICAgICAgICJhc3NpZ24iOiBbIlNldHRpbmciXSwNCiAgICAgICAgICAgICJ2aWRlbyI6IFsiTGl2ZStQbGF5YmFjaytBcmNoaXZlIl0sDQogICAgICAgICAgICAiY29uZGl0aW9uIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAiY2FtZXJhIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAiaW5mb1NldHRpbmciOiBbIlNldHRpbmciXSwNCiAgICAgICAgICAgICJyZWNvcmRTZXR0aW5nIjogWyJTZXR0aW5nIl0sDQogICAgICAgICAgICAiaXZhRXZlbnQiOiBbIlJlYWQiXSwNCiAgICAgICAgICAgICJzeXN0ZW1FdmVudCI6IFsiUmVhZCJdDQogICAgICAgIH0NCiAgICAgIH0sDQogICAgICANCiAgICAgICJ0dldhbGxzIjogew0KICAgICAgICAiZW5hYmxlIjogdHJ1ZQ0KICAgICAgfSwNCiAgICAgICJldmVudHMiOiB7DQogICAgICAgICJlbmFibGUiOiB0cnVlLA0KICAgICAgICAic3Vic2NyaXB0aW9uIjogWyJTZXR0aW5nIl0sDQogICAgICAgICJldmVudE1hbmFnZXJTZXR0aW5nIjogWyJTZXR0aW5nIl0NCiAgICAgIH0sDQogICAgICAic3lzdGVtIjogew0KICAgICAgICAiaW5mbyI6IFsiU2V0dGluZyJdLA0KICAgICAgICAibW1zIjogWyJTZXR0aW5nIl0sDQogICAgICAgICJsbXMiOiBbIlNldHRpbmciXSwNCiAgICAgICAgInN0b3JhZ2UiOiBbIlNldHRpbmciXSwNCiAgICAgICAgInRpbWUiOiBbIlNldHRpbmciXSwNCiAgICAgICAgImFjY291bnQiOiBbIkFjY291bnQrUm9sZSJdLA0KICAgICAgICAicmVib290IjogWyJBbGxvdyJdLA0KICAgICAgICAidXBncmFkZSI6IFsiQWxsb3ciXSwgDQogICAgICAgICJiZXRhIjogWyJBbGxvdyJdICANCiAgICAgIH0NCiAgICB9LA0KICAgICJ2bXMiOiB7DQogICAgICANCiAgICB9DQogIH0NCg=="}
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

