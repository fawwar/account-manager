#include "Error.h"
#include "gorilla/log/logger.h"
#include "gorilla/log/logger_config.h"

using namespace gorilla::log;

namespace gorilla {
    namespace account{
        
        ErrorReply::ErrorReply()
            : m_json_error(DEFAULT_ERROR)
        {
        }
   
        ErrorReply::~ErrorReply()
        {
        }

        std::string ErrorReply::GetError(const std::string& str_user_message,
                const std::string& str_internal_msg) const
        {
            m_json_error["userMessage"] = str_user_message;
            m_json_error["internalMessage"] = str_internal_msg;
  
            LOGGER_S(debug) << "Error = "<< m_json_error;
            
            return m_json_error.dump();
        }

    }
}


