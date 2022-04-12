#include <vector>
#include <string>
class class_entity;
class class_entity;

//std::string strip_trailing_(std::string data);
size_t find_key_word(std::string type, const char* val, bool check_end);
bool is_uint8(std::string type);
bool is_int8(std::string type);
bool is_uint16(std::string type);
bool is_int16(std::string type);
bool is_uint32(std::string type);
bool is_int32(std::string type);
bool is_ulong(std::string type);
bool is_long(std::string type);
bool is_uint64(std::string type);
bool is_int64(std::string type);
bool is_bool(std::string type);
bool is_float(std::string type);
bool is_double(std::string type);
bool is_char_star(std::string type);
bool is_enum(std::string type, const class_entity& library);
std::string unconst(std::string type);
std::string unenum(std::string type);
void strip_reference_modifiers(std::string& param_type, std::string& referenceModifiers);
void translate_type(std::string param_type, const class_entity& library);
std::string get_template_param(std::string type);
std::vector<std::string> split_namespaces(std::string type);

/*bool isString(std::string type);
bool isJavaScriptObject(std::string type);
bool isVector(std::string type);
bool isList(std::string type);
bool isMap(std::string type);
bool isSet(std::string type);*/
