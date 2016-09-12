#ifndef JSON_TOOLS_HPP_
#define JSON_TOOLS_HPP_

#include <string>
#include "json.hpp"

namespace gorilla
{
	inline bool IsKeyExsist(const nlohmann::json& j_object, const std::string& str_key)
	{
		auto it = j_object.find(str_key);
		if(it != j_object.end()){
			return true;
		}

		return false;
	}
	
	inline bool IsKeyExsist(const nlohmann::json& j_object, const std::string& str_key,
				std::string& out_srt_value)
	{
		auto it = j_object.find(str_key);
		if(it != j_object.end()){
			
			out_srt_value = *it;
			return true;
		}

		return false;
	}
	
	inline bool IsKeyExsist(const nlohmann::json& j_object, const std::string& str_key,
				nlohmann::json& out_json_value)
	{
		auto it = j_object.find(str_key);
		if(it != j_object.end()){
			
			out_json_value = *it;
			return true;
		}

		return false;				
	}
	
	inline void SetJson(const json& json_src, json& out_json_dst)
	{
		for(auto it = json_src.begin(); it != json_src.end(); ++it){
        
            /* if jason is object, do recursive */ 
            if((*it).is_object()){
                SetJson(*it,out_json_dst[it.key()]);
            }
            else{
           
                /* if jason is not null, can assign value, otherwise defult vaule */
                if(!(*it).is_null()){
                    out_json_dst[it.key()] = it.value(); 
                }
            }
        } 
	}
}

#endif