#pragma once

// std
#include <string>
#include "AccountDefine.h"

using json = nlohmann::json;

namespace gorilla {
    namespace account{

        class ErrorReply
        {
        public:

            ErrorReply();
            ~ErrorReply();

            std::string GetError(const std::string& str_user_message,
                const std::string& str_internal_msg) const;

        private:

            mutable json m_json_error;
        };
        
    }
}


