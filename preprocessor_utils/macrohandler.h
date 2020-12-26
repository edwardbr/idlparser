#pragma once

#include <vector>
#include <set>
#include <unordered_map>
#include <filesystem>
#include <cassert>
#include "commonfuncs.h"

using paths = std::vector<std::filesystem::path>;

struct macro_parser
{
	using operatorFN = std::string (macro_parser::*)(const std::string, const std::string);

	struct operatorDesc
	{
		const char* m_name;
		operatorFN m_fn;
		bool m_isunary;
	};

	struct definition
	{
		std::vector<std::string> m_paramsList;
		std::string m_substitutionString;
	};

	virtual [[nodiscard]] bool Init();
	virtual [[nodiscard]] bool FindDefString(std::string& var);
	virtual void UnDefine(std::string& var);
	virtual void Define(std::string& var);
	//virtual char IsLogicalExpression(const char* pData);
	virtual [[nodiscard]] std::string ExtractExpression(const char*& pData, const paths& includeDirectories, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] bool SubstituteMacro(int ignoreText, const char*& pData, std::ostream& dest, const paths& includeDirectories, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] bool ProcessIf(const char*& pData, std::ostream& dest, const paths& includeDirectories, int& inIfDef, int& ignoreText, bool& bInTheMiddleOfWord, std::vector<std::string>& loaded_includes);
	virtual void CleanBuffer(const char*& pData, std::ostream& dest, const paths& includeDirectories, int inIfDef, int ignoreText, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] std::string ReduceExpression(const char*& pData, const paths& includeDirectories, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] bool Load(std::ostream& output_file, const std::string& file, const paths& includeDirectories, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] void ParseAndLoad(int ignoreText, std::ostream& stream, const paths& includeDirectories, const char* file, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] bool LoadUsingEnv(std::ostream& stream, const std::string& file, const paths& includeDirectories, std::vector<std::string>& loaded_includes);
	virtual [[nodiscard]] bool ParseInclude(const char*& pData, int ignoreText, std::ostream& stream, const paths& includeDirectories, std::vector<std::string>& loaded_includes);

	virtual std::string not(const std::string val, const std::string);
	virtual std::string bnot(const std::string val, const std::string);
	virtual std::string mult(const std::string val1,const std::string val2);
	virtual std::string div(const std::string val1,const std::string val2);
	virtual std::string mod(const std::string val1,const std::string val2);
	virtual std::string add(const std::string val1,const std::string val2);
	virtual std::string sub(const std::string val1,const std::string val2);
	virtual std::string shiftl(const std::string val1,const std::string val2);
	virtual std::string shiftr(const std::string val1,const std::string val2);
	virtual std::string lt(const std::string val1,const std::string val2);
	virtual std::string lte(const std::string val1,const std::string val2);
	virtual std::string gt(const std::string val1,const std::string val2);
	virtual std::string gte(const std::string val1,const std::string val2);
	virtual std::string eq(const std::string val1,const std::string val2);
	virtual std::string neq(const std::string val1,const std::string val2);
	virtual std::string band(const std::string val1,const std::string val2);
	virtual std::string bxor(const std::string val1,const std::string val2);
	virtual std::string bor(const std::string val1,const std::string val2);
	virtual std::string and(const std::string val1,const std::string val2);
	virtual std::string or(const std::string val1,const std::string val2);
	virtual [[nodiscard]] bool IsOperator(const char* pData, std::string& operatorString);

	virtual void AddDefine(std::string name, definition value){defines[name] = value;}


	std::set<std::string> openedImports;
	std::unordered_map<std::string, definition> defines;
	std::vector<operatorDesc> operatorBehaviours;
	std::vector<const char*> operators;
};


void CleanBufferOfComments(const char*& pData);
