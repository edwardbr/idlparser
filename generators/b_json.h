
namespace blocking
{
	namespace json
	{

		//entry point
		void writeJSONFiles(const ClassObject& m_ob, ostream& hos, ostream& cos, ostream& jos, const char* gsoap_namespace, const std::string& header);

		//utils
		std::string unconst(const std::string& type);
//		std::string strip_trailing_(std::string data);
		int findKeyWord(const std::string& type, const char* val, bool check_end);
		bool isSize_t(const std::string& type);
		bool isInt(const std::string& type);
		bool isLong(const std::string& type);
		bool isBool(const std::string& type);
		bool isDouble(const std::string& type);
		bool isCharStar(const std::string& type);
		bool isString(const std::string& type);
		bool isVector(const std::string& type);
		bool isList(const std::string& type);
		bool isMap(const std::string& type);
		bool isSet(const std::string& type);
		bool isEnum(const std::string& type, const Library& library);
		void stripReferenceModifiers(std::string& paramType, std::string& referenceModifiers);
		void translateType(std::string& paramType, const Library& library);
		std::string getTemplateParam(const std::string& type);
		std::string getTemplateTypeAlias(const std::string& templateType);
		void buildJSONEnum(const ClassObject* pObj, std::string& unconst_type);
		void writeStructureItems(const ClassObject* pObj, std::stringstream& cos, std::string& dereferencer, std::stringstream& jos);

		//parse generation
		bool readTypeAtomic(ostream& cos, std::string& paramType, const char* paramName);
		std::string buildStructure(std::string& paramType, const Library& library);
		std::string buildVector(const std::string& type, const Library& library);
		std::string buildList(const std::string& type, const Library& library);
		std::string buildMap(const std::string& type, const Library& library);
		std::string buildSet(const std::string& type, const Library& library);
		std::string buildEnum(const std::string& type, const Library& library);

		//write generation
		std::string writeVector(std::string& paramType, const Library& library);
		std::string writeList(std::string& paramType, const Library& library);
		std::string writeMap(std::string& paramType, const Library& library);
		std::string writeEnum(std::string& paramType, const Library& library);
		std::string writeStructure(std::string& paramType, const Library& library);
		std::string writeSet(std::string& paramType, const Library& library);
		bool writeType(ostream& cos, ostream& jos, std::string& paramType, const char* paramName, const Library& library);
	}
}