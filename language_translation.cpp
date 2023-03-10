#include "language_translation.h"
#include <debug_logger.h>
#include <json_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <file_utils.h>
#include <map>

LOG_CATEGORY(LANGUAGE_TRANSLATIONS, "LANGUAGE_TRANSLATIONS")


namespace {

static const char* 
parseLanguage(std::string language_file_path, const char* in)
{
    static std::map<std::string, cJSON *>language_json;

    auto json_it = language_json.find(language_file_path);
    cJSON * json = nullptr;
    if(json_it != language_json.end()){
        json = json_it->second;
    }
    else if(!FileUtils::fileExists(language_file_path.c_str())){
        LOG(FATAL, LANGUAGE_TRANSLATIONS, "File doesn't exist: %s\n", language_file_path.c_str());
    }
    else{
        char *json_str = loadJsonFromFile(language_file_path.c_str());
        if(json_str == NULL){
            LOG(FATAL, LANGUAGE_TRANSLATIONS, "Could not open data json file for main status, %s\n", language_file_path.c_str());
        }
        json = cJSON_Parse(json_str);
        if (!json)  {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                LOG(FATAL, LANGUAGE_TRANSLATIONS, "Error before: %s\n", error_ptr);
            }
        }
        language_json[language_file_path] = json;
    }
    cJSON *translated_text = cJSON_GetObjectItemCaseSensitive(json, in);
    if(translated_text){
        return translated_text->valuestring;
    }
    else{
        return in;
    }

}


static std::string 
defaultLanguage(const std::string &default_language)
{
    char *json_str = loadJsonFromFile(FileUtils::configPath("config.json").c_str());
    if(json_str == NULL){
        LOG(FATAL, LANGUAGE_TRANSLATIONS, "Could not open config file: %s\n", FileUtils::configPath("config.json").c_str());
    }
    cJSON *config = cJSON_Parse(json_str);
    if (config == NULL)  {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            LOG(FATAL, LANGUAGE_TRANSLATIONS, "Error before: %s\n", error_ptr);
        }
    }
    char *language = cJSON_GetObjectItemCaseSensitive(config, "language")->valuestring;
    return (language && *language) ? language : default_language;
}
}//namespace

const char* 
tr(const char* in)
{
    static std::string language = defaultLanguage("english");
    static std::string json_path = FileUtils::configPath(std::string("ui-") + language);
    return parseLanguage(json_path, in);
} 

