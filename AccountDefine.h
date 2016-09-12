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

        enum AccessLevel
        {
            ADMINISTRATOR,
            USER    
        };

        const json::object_t DEFAULT_USER
        {
            {"account", ""}, 
            {"password", ""}, 
            {"levelName", ""},
            {"description", ""}
        };

        const json::object_t DEFAULT_RESPONSE_USER
        {
            {"account", ""}, 
            {"encryptedPassword", ""}, 
            {"levelName", ""},
            {"description", ""}
        };

        const json::object_t DISABLE_IVAR_FEATURES
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

        const json::object_t DISABLE_IOT_FEATURES
        {
            {"get", false}, 
            {"set", false}, 
            {"getSubscription", false},
            {"setSubscription", false}
        };

        const json::object_t DISABLE_FEATURES
        {
            {"ivar", DISABLE_IVAR_FEATURES}, 
            {"iot", DISABLE_IOT_FEATURES}
        };

        const json::object_t DISABLE_LEVEL
        {
            {"levelName", ""}, 
            {"features", DISABLE_FEATURES},
            {"description", ""}
        };
        

        const json::object_t DEFAULT_IVAR_FEATURES
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

        const json::object_t DEFAULT_IOT_FEATURES
        {
            {"get", true}, 
            {"set", true}, 
            {"getSubscription", true},
            {"setSubscription", true}
        };

        const json::object_t DEFAULT_FEATURES
        {
            {"ivar", DEFAULT_IVAR_FEATURES}, 
            {"iot", DEFAULT_IOT_FEATURES}
        };

        const json::object_t DEFAULT_LEVEL
        {
            {"levelName", ""}, 
            {"features", DEFAULT_FEATURES},
            {"description", ""}
        };

        const nlohmann::json::object_t DEFAULT_ERROR = 
        {
            {"userMessage",""}, 
            {"internalMessage",""}
        };

     }
}

