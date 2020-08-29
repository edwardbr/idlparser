namespace non_blocking
{

	namespace json
	{

		//entry point
		void writeFiles(const ClassObject& m_ob, ostream& hos, ostream& cos, std::vector<std::string>& namespaces/*, const std::string& header*/);

		//utils
		void writeStructureItems(const ClassObject* pObj, ostringstream& cos, std::string& dereferencer);

		//parse generation
		bool readTypeAtomic(ostream& cos, std::string& paramType, int next_pos, const char* field, int comma_count);
		std::string buildStructure(std::string& paramType, const attributes& attrib, const Library& library);
		std::string buildVector(const std::string& type, const attributes& attrib, const Library& library);
		std::string buildList(const std::string& type, const attributes& attrib, const Library& library);
		std::string buildMap(const std::string& type, const attributes& attrib, const Library& library);
		std::string buildSet(const std::string& type, const attributes& attrib, const Library& library);
		std::string buildEnum(const std::string& type, const attributes& attrib, const Library& library);

		//write generation
		std::string writeVector(std::string& paramType, const Library& library);
		std::string writeList(std::string& paramType, const Library& library);
		std::string writeMap(std::string& paramType, const Library& library);
		std::string writeEnum(std::string& paramType, const Library& library);
		std::string writeStructure(std::string& paramType, const Library& library);
		std::string writeSet(std::string& paramType, const Library& library);
		bool writeType(ostream& cos, std::string& paramType, const char* paramName, const Library& library);
	}
}