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

	string m_defName;
	std::vector<string> m_paramsList;
	string m_substitutionString;
};

bool operator == (const definition& first, const definition& second);

extern std::unordered_map<string, definition> defines;

bool FindDefString(string& var);
void UnDefine(string& var);
void Define(string& var);
//char IsLogicalExpression(const char* pData);
string ExtractExpression(const char*& pData, string& includeDirectories);
bool SubstituteMacro(int ignoreText, const char*& pData, ostream& dest, string& includeDirectories);
void CleanBuffer(const char*& pData, ostream& dest, string& includeDirectories, int inIfDef = 0, int ignoreText = 0);
string ReduceExpression(const char*& pData, string& includeDirectories);
bool Load(std::ostream& stream, istream& file, string& includeDirectories);
void ParseAndLoad(int ignoreText, ostream& stream, string& includeDirectories, const char* file);
bool LoadUsingEnv(ostream& stream, const string& file, string& includeDirectories);




typedef string (*operatorFN)(const string, const string);

string not(const string val, const string);
string bnot(const string val, const string);
string mult(const string val1,const string val2);
string div(const string val1,const string val2);
string mod(const string val1,const string val2);
string add(const string val1,const string val2);
string sub(const string val1,const string val2);
string shiftl(const string val1,const string val2);
string shiftr(const string val1,const string val2);
string lt(const string val1,const string val2);
string lte(const string val1,const string val2);
string gt(const string val1,const string val2);
string gte(const string val1,const string val2);
string eq(const string val1,const string val2);
string neq(const string val1,const string val2);
string band(const string val1,const string val2);
string bxor(const string val1,const string val2);
string bor(const string val1,const string val2);
string and(const string val1,const string val2);
string or(const string val1,const string val2);

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
