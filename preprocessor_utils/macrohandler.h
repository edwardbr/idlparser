#include <vector>
#include <assert.h>
#include "commonfuncs.h"

struct definition
{
	definition()
	{}
	definition(const definition& other) : m_defName(other.m_defName), m_paramsList(other.m_paramsList), m_substitutionString(other.m_substitutionString)
	{}
	bool operator = (const definition& other)
	{
		m_defName = other.m_defName;
		m_paramsList = other.m_paramsList;
		m_substitutionString = other.m_substitutionString;
	}

	std::string m_defName;
	std::vector<std::string> m_paramsList;
	std::string m_substitutionString;
};

bool operator == (const definition& first, const definition& second);

extern std::unordered_map<std::string, definition> defines;

bool FindDefString(std::string& var);
void UnDefine(std::string& var);
void Define(std::string& var);
//char IsLogicalExpression(const char* pData);
std::string ExtractExpression(const char*& pData, std::string& includeDirectories);
bool SubstituteMacro(int ignoreText, const char*& pData, std::ostream& dest, std::string& includeDirectories);
void CleanBuffer(const char*& pData, std::ostream& dest, std::string& includeDirectories, int inIfDef = 0, int ignoreText = 0);
std::string ReduceExpression(const char*& pData, std::string& includeDirectories);
bool Load(std::ostream& stream, std::istream& file, std::string& includeDirectories);
void ParseAndLoad(int ignoreText, std::ostream& stream, std::string& includeDirectories, const char* file);
bool LoadUsingEnv(std::ostream& stream, const std::string& file, std::string& includeDirectories);




typedef std::string (*operatorFN)(const std::string, const std::string);

std::string not(const std::string val, const std::string);
std::string bnot(const std::string val, const std::string);
std::string mult(const std::string val1,const std::string val2);
std::string div(const std::string val1,const std::string val2);
std::string mod(const std::string val1,const std::string val2);
std::string add(const std::string val1,const std::string val2);
std::string sub(const std::string val1,const std::string val2);
std::string shiftl(const std::string val1,const std::string val2);
std::string shiftr(const std::string val1,const std::string val2);
std::string lt(const std::string val1,const std::string val2);
std::string lte(const std::string val1,const std::string val2);
std::string gt(const std::string val1,const std::string val2);
std::string gte(const std::string val1,const std::string val2);
std::string eq(const std::string val1,const std::string val2);
std::string neq(const std::string val1,const std::string val2);
std::string band(const std::string val1,const std::string val2);
std::string bxor(const std::string val1,const std::string val2);
std::string bor(const std::string val1,const std::string val2);
std::string and(const std::string val1,const std::string val2);
std::string or(const std::string val1,const std::string val2);

struct operatorDesc
{
	operatorDesc(const char* name, operatorFN fn, bool isunary) : m_name(name), m_fn(fn), m_isunary(isunary)
	{}
	operatorDesc(const operatorDesc& other) : m_name(other.m_name), m_fn(other.m_fn), m_isunary(other.m_isunary)
	{}
	void operator = (const operatorDesc& other)
	{
		m_name = other.m_name;
		m_fn = other.m_fn;
		m_isunary = other.m_isunary;
	}
	const char* m_name;
	operatorFN m_fn;
	bool m_isunary;
};

extern const operatorDesc operatorBehaviours[];


void CleanBufferOfComments(const char*& pData);
