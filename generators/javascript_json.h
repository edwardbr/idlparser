namespace javascript_json
{
	namespace json
	{
		//entry point
		void writeJSONFiles(const ClassObject& m_ob, std::ostream& jos);
		void buildJSONEnum(const ClassObject* pObj, std::string& unconst_type);
		void writeStructureItems(const ClassObject* pObj, std::string& dereferencer, std::stringstream& jos);

		//parse generation
		bool readTypeAtomic(std::string& paramType, const char* paramName);
		void buildVector(const std::string& type, const Library& library);
		void buildList(const std::string& type, const Library& library);
		void buildMap(const std::string& type, const Library& library);
		void buildSet(const std::string& type, const Library& library);
		void buildEnum(const std::string& type, const Library& library);

		//write generation
		std::string writeVector(std::string& paramType, const Library& library);
		std::string writeList(std::string& paramType, const Library& library);
		std::string writeMap(std::string& paramType, const Library& library);
		std::string writeEnum(std::string& paramType, const Library& library);
		std::string writeStructure(std::string& paramType, const Library& library);
		std::string writeSet(std::string& paramType, const Library& library);
		bool writeType(std::ostream& jos, std::string& paramType, const char* paramName, const Library& library);
	}
}