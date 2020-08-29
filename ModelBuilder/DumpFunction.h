void DumpFunction(const functionInfo& m_params, const string& obj, const FunctionObject& fn, ostream& os, bool istatic, bool dumpHeader, bool dumpBody, bool includeTemplates, const char* classSuffixName);
void PrintSPFunction(const functionInfo& m_params, const ClassObject& obj, const FunctionObject& fn, ostream& hos);

void DumpFunctionHeader(const FunctionObject& fn, 
						const functionInfo& func,  
						const string& obj,
						ostream& os, 
						bool istatic, 
						bool dumpHeader, 
						bool dumpBody,
						const char* classSuffixName);

void DumpFunctionBody(	const FunctionObject& fn, 
						const functionInfo& func, 
						ostream& os, 
						bool istatic, 
						bool dumpHeader, 
						bool dumpBody);

string ExtractArrayParamWrapper(paramInfo& param, bool showConst);
bool allocateArrayOnClient(paramInfo& param);
