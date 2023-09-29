#include <iostream>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <functional>

#include "macro_parser.h"

macro_parser::macro_parser()
{
	operatorBehaviours.assign({
		{"!",&macro_parser::not_,true}, //Logical NOT   
		{"~",&macro_parser::bnot,true}, //Bitwise NOT   
		{"*",&macro_parser::mult,false}, //Multiply Left to right 
		{"/",&macro_parser::div,false}, //Divide   
		{"%",&macro_parser::mod,false}, //Remainder   
		{"+",&macro_parser::add,false}, //Add Left to right 
		{"-",&macro_parser::sub,false}, //Subtract   
		{"<<",&macro_parser::shiftl,false}, //Left shift Left to right 
		{">>",&macro_parser::shiftr,false}, //Right shift   
		{"<",&macro_parser::lt,false}, //Less than Left to right 
		{"<=",&macro_parser::lte,false}, //Less than or equal to   
		{">",&macro_parser::gt,false}, //Greater than   
		{">=",&macro_parser::gte,false}, //Greater than or equal to   
		{"==",&macro_parser::eq,false}, //Equal Left to right 
		{"!=",&macro_parser::neq,false}, //Not equal   
		{"&",&macro_parser::band,false}, //Bitwise AND Left to right 
		{"^",&macro_parser::bxor,false}, //Bitwise exclusive OR Left to right 
		{"|",&macro_parser::bor,false}, //Bitwise OR Left to right 
		{"&&",&macro_parser::and_,false}, //Logical AND Left to right 
		{"||",&macro_parser::or_,false} //Logical OR Left to right 
	});
	operators = {"reinterpret_cast", "dynamic_cast", "static_cast", "const_cast", "delete", "sizeof", "typeid", "new", "<<=", ">>=", "->*", "::", "++", "--", "->", ".*", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||", "%=", "&=", "|=", "*=", "/=", "+=", "-=", "^=", "=", "!", "~", "*", "/", "%", "+", "-", "<", ">", "&", "^", "|", ",", ":", ".", ":", "[", "]", "(", ")", "!", "-", "+", "/"};
}

std::string macro_parser::ExtractExpression(const char*& pData, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
	//((sfsd)*(gfg))
	//       ^
	//       | here is a dip, knowing this point we can asertain if we can strip out the outer brackets

	bool haveReachedDip = false;
 	int lowestBracketDip = -1;
	int maxBracketCount = -1;

	std::string retVal;

	while(*pData == ' ')
		pData++;

	int bracketCount = -1;

	//we need to scan a line until we come accross two words that are not delimited by an operator
	while(*pData != '\0' && *pData != '\n')
	{
		if(*pData == ' ')
		{
			pData++;
			continue;
		}

/*		if(firstpass)
		{
			firstpass = false;
			retVal += ' ';
		}*/

		std::string tempString;
		
		if(is_word(pData,"defined"))
		{
			pData += 7;

			while(*pData == ' ')
				pData++;

			bool required_closing = true;
			if(*pData != '(')
				required_closing = false;

			pData++;

			while(*pData == ' ')
				pData++;
			
			if(!extract_word(pData, tempString))
			{
				throw std::runtime_error("expected a word insde the brackets of the defined keyword");
			}

			while(*pData == ' ')
				pData++;
			
			if(*pData != ')' && required_closing == true)
			{
				throw std::runtime_error("expected a ')'");
			}
			else if(required_closing == true)
			{
				pData++;
			}

			if(defines.find(tempString) != defines.end())
				retVal += "true";
			else
				retVal += "false";

			continue;
		}

		else if(	*pData == '!' || 
			*pData == '"' || 
			*pData == '%' || 
			*pData == '^' || 
			*pData == '&' || 
			*pData == '*' || 
			*pData == '+' || 
			*pData == '=' || 
			*pData == '{' || 
			*pData == '}' || 
			*pData == '~' || 
			*pData == '/' || 
			*pData == '>' || 
			*pData == '<' || 
			*pData == '.' || 
			*pData == ',' || 
			*pData == '|' || 
			*pData == '\\' || 
			*pData == '\n')
		{
			retVal += *pData++;

			while(*pData == '!' || 
				*pData == '"' || 
				*pData == '%' || 
				*pData == '^' || 
				*pData == '&' || 
				*pData == '*' || 
				*pData == '+' || 
				*pData == '=' || 
				*pData == '{' || 
				*pData == '}' || 
				*pData == '~' || 
				*pData == '/' || 
				*pData == '>' || 
				*pData == '<' || 
				*pData == '.' || 
				*pData == ',' || 
				*pData == '|' || 
				*pData == '\\' || 
				*pData == '\n')
			{
				retVal += *pData++;
			}
			continue;
		}


		else if(*pData == '(')
		{
			if(bracketCount == -1)
				bracketCount = 1;
			else
				bracketCount++;

			if(maxBracketCount > bracketCount - 1)
			{
				haveReachedDip = true;
				lowestBracketDip = std::min(lowestBracketDip,bracketCount - 1);
			}

			maxBracketCount = std::max(maxBracketCount,bracketCount);

			retVal += *pData++;
			continue;
		}
		else if(*pData == ')')
		{
			if(bracketCount == -1)
				return retVal;

			bracketCount--;
			if(bracketCount == 0)
			{
				retVal += *pData++;
				const char* temp = pData;
				while(*temp == ' ')
					temp++;

				if(*temp != '!' && 
					*temp != '"' && 
					*temp != '%' && 
					*temp != '^' && 
					*temp != '&' && 
					*temp != '*' && 
					*temp != '+' && 
					*temp != '=' && 
					*temp != '{' && 
					*temp != '}' && 
					*temp != '~' && 
					*temp != '/' && 
					*temp != '>' && 
					*temp != '<' && 
					*temp != '.' && 
					*temp != ',' && 
					*temp != '|' && 
					*temp != '\\' && 
					*temp != '\n')
				{
					break;//we have reached the end of the extpression
				}
			}
		}
		else
		{
			tempString = "";
			while(*pData != '\0' && *pData != '\n' && *pData != '\r' && !(*pData == '&' && pData[1] == '&') && !(*pData == '|' && pData[1] == '|'))
				tempString += *pData++;

			std::stringstream tempStream;
			const char* tempStringData = tempString.data();
			if(SubstituteMacro(false, tempStringData, tempStream, includeDirectories, loaded_includes))
			{
				tempStream << std::ends;
				retVal += tempStream.str();
			}
			else
			{
				retVal += tempString;
			}
			continue;
		}


		break;
	}
	while(*pData == '\n' || *pData == ' ')
		pData++;


	if(!haveReachedDip && maxBracketCount && retVal[0] == '(' && retVal[retVal.length() - 1] == ')')
		retVal = retVal.substr(1,retVal.length() - 2);
	else
		while(lowestBracketDip > 0)
		{
			if(retVal[0] == '(' && retVal[retVal.length() - 1] == ')')
				retVal = retVal.substr(retVal.length() - 2, 1);

			lowestBracketDip--;
		}
	return retVal;
}

enum if_status
{
	ifdef,
	ifndef,
	hash_if
};

void macro_parser::ProcessIf(const char*& pData, std::ostream& dest, const paths& includeDirectories, int& inIfDef, int& ignoreText, bool& bInTheMiddleOfWord, std::vector<std::string>& loaded_includes)
{

	if_status status = ifdef;

	if(is_preproc_eat(pData,"ifdef"))			
	{
		status = ifdef;
		bInTheMiddleOfWord = false;
	}
	else if(is_preproc_eat(pData,"ifndef"))			
	{
		status = ifndef;
		bInTheMiddleOfWord = false;
	}
	else if(is_preproc_eat(pData,"if"))			
	{
		status = hash_if;
		bInTheMiddleOfWord = false;
	}

	bool current_expression_true = false;
	bool processExpression = true;
	bool expressionFound = false;
	while(*pData != '\0')
	{
		if(!ignoreText)
		{
			dest << ' ';
		}

		while(*pData == ' ')
			pData++;

		if(processExpression == true)
		{
			if(ifdef == status || ifndef == status)
			{
				std::string var;
				while(*pData != ' ' && *pData != '\0' && *pData != '\n')
					var += *pData++;
				
				if(expressionFound == false)
				{
					current_expression_true = FindDefString(var);
					if(status == ifndef)
					{
						current_expression_true = !current_expression_true;
					}
				}
				else
				{
					current_expression_true = false;
				}
			}
			else
			{
				std::string expr = ExtractExpression(pData, includeDirectories, loaded_includes);

				const char* exprString = expr.data();
				std::string value = ReduceExpression(exprString, includeDirectories, loaded_includes);

				if(expressionFound == false)
				{
					if(value == "true" || atoi(value.data()))
					{
						current_expression_true = true;
					}
					else
					{
						current_expression_true = false;
					}
				}
			}
		}

		ignoreText += current_expression_true ? 0 : 1;
		
		CleanBuffer(pData, dest, includeDirectories, inIfDef + 1, ignoreText, loaded_includes);
		
		ignoreText -= current_expression_true ? 0 : 1;

		if(current_expression_true == true)
		{
			expressionFound = true;
			ignoreText++;
		}

		if(is_preproc_eat(pData,"endif"))
		{
			break;
		}
		else if(is_preproc_eat(pData,"else"))
		{
			processExpression = false;
			current_expression_true = !expressionFound;
		}
		else if(is_preproc_eat(pData,"elif"))
		{
			status = hash_if;
		}
	}
	if(expressionFound == true)
	{
		ignoreText--;
	}
	if(!ignoreText)
	{
		dest << ' ';
	}
}

bool macro_parser::ParseInclude(const char*& pData, int ignoreText, std::ostream& dest, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
	if(!is_preproc_eat(pData,"include"))			
		return false;

	while(*pData == ' ')
		pData++;

	std::string var;

	if(*pData == '\"' || *pData == '<')
	{
		pData++;

		while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
		{
			var += *pData++;
		}

		if(!(*pData == '\"' || *pData == '>'))
		{
			std::stringstream err;
			err << "Error unexpected character: " << *pData;
			err << std::ends;
			std::string errString(err.str());
			throw std::runtime_error(errString);
		}
		pData++;
	}
	else
	{
		extract_word(pData, var);
		auto item = defines.find(var);
		if(item == defines.end())
		{
			std::stringstream err;
			err << "Error unexpected character: " << *pData;
			err << std::ends;
			std::string errString(err.str());
			throw std::runtime_error(errString);
		}
		var = item->second.m_substitutionString;
	}

	extract_path_and_load(ignoreText, dest, includeDirectories, var.data(), loaded_includes);
	return true;
}

void find_end_quote(const char*& pData)
{
	for(auto ch : "#cpp_quote(")
	{
		if(!ch)
			break;
		++pData;
	}	

	const char* pStart = nullptr;
	const char* pSuffix = nullptr;
	auto* oldPData = pData;
	if(extract_multiline_string_literal(pData, pStart, pSuffix))
	{
		while(oldPData != pData)
		{
			++oldPData;
		}
	}
	else
	{
		if(*pData != '\"' && *pData != '^')
			throw std::runtime_error("missing initial \" in #cpp_quote");
		auto quote_char = *pData;
		pData++;        
		while(*pData && *pData != quote_char)
		{
			if(*pData == '\\')
			{
				pData++;
				if(!*pData)
					throw std::runtime_error("invalid ending in #cpp_quote (no quote)");
			} 
			pData++;
		}    
		if(!*pData || *pData != quote_char)
			throw std::runtime_error("invalid ending in #cpp_quote (no quote)");
		pData++;  
	}
	while(*pData && (*pData == ' ' || *pData == '\t'))
		pData++;
	if(!*pData || *pData != ')')
		throw std::runtime_error("missing final ) in #cpp_quote");
	++pData;
}

void macro_parser::CleanBuffer(const char*& pData, std::ostream& dest, const paths& includeDirectories, int inIfDef, int ignoreText, std::vector<std::string>& loaded_includes)
{
	bool bInTheMiddleOfWord = false;
	//strip out comments and endls;
	while(*pData != 0)
	{
		if(inIfDef != 0)
		{
			if(is_preproc(pData,"endif") || is_preproc(pData,"else") || is_preproc(pData,"elif"))
			{
				return;
			}
		}

		if(ParseInclude(pData, ignoreText, dest, includeDirectories, loaded_includes))
		{
			bInTheMiddleOfWord = false;
		}
/*		else if(is_word(pData,"importlib"))			
		{
			bInTheMiddleOfWord = false;

			pData += 9;

			while(*pData == ' ')
				pData++;

			if(*pData != '(')
			{
				std::stringstream err;
				err << "Error unexpected character: " << *pData;
				err << std::ends
				std::string errString(err.str());
				throw std::runtime_error(errString);
			}
			pData++;
			if(*pData != '\"')
			{
				std::stringstream err;
				err << "Error unexpected character: " << *pData;
				err << std::ends;
				std::string errString(err.str());
				throw std::runtime_error(errString);
			}
			pData++;

			std::string var;
			while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
			{
				var += *pData++;
			}

			if(*pData != '\"')
			{
				std::stringstream err;
				err << "Error unexpected character: " << *pData;
				err << std::ends;
				std::string errString(err.str());
				throw std::runtime_error(errString);
			}
			pData++;

			if(*pData != ')')
			{
				std::stringstream err;
				err << "Error unexpected character: " << *pData;
				err << std::ends;
				std::string errString(err.str());
				throw std::runtime_error(errString);
			}
			pData++;

			std::stringstream includeStream;
			load(includeStream, var.data());
			includeStream << std::ends;
			output << includeStream.str();
		}*/
		else if(is_preproc_eat(pData,"import"))			
		{
			bInTheMiddleOfWord = false;

			while(*pData == ' ')
				pData++;

			if(*pData != '\"')
			{
				std::stringstream err;
				err << "Error unexpected character: " << *pData;
				err << std::ends;
				std::string errString(err.str());
				throw std::runtime_error(errString);
			}
			pData++;

			std::string var;
			while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
			{
				var += *pData++;
			}

			if(*pData != '\"')
			{
				std::stringstream err;
				err << "Error unexpected character: " << *pData;
				err << std::ends;
				std::string errString(err.str());
				throw std::runtime_error(errString);
			}
			pData++;

			while(*pData == ' ')
				pData++;

			if(*pData == ';')
			{
				pData++;
			}

			var = to_lower(var);
			//if(openedImports.find(var) == openedImports.end())
			{
				//openedImports.insert(var);
				dest << "import \"" << var << "\" {";
				extract_path_and_load(ignoreText, dest, includeDirectories, var.data(), loaded_includes);
				dest << "}";
			}
		}
		else if(is_preproc_eat(pData,"define"))			
		{
			bInTheMiddleOfWord = false;

			while(*pData == ' ')
				pData++;

			std::string var;
			while(*pData != '\0' && *pData != '\n')
				var += *pData++;

			if(!ignoreText)
				Define(var);

			while(*pData == '\n')
				pData++;
		}
		else if(is_preproc_eat(pData,"undef"))			
		{
			bInTheMiddleOfWord = false;
			while(*pData == ' ')
				pData++;

			std::string var;
			while(*pData != '\0' && *pData != '\n')
				var += *pData++;

			if(!ignoreText)
				UnDefine(var);
		}

		else if(is_preproc(pData,"ifdef") || is_preproc(pData,"ifndef") || is_preproc(pData,"if"))			
		{
			ProcessIf(pData, dest, includeDirectories, inIfDef, ignoreText, bInTheMiddleOfWord, loaded_includes);
		}
		
		else if(!bInTheMiddleOfWord && SubstituteMacro(ignoreText, pData, dest, includeDirectories, loaded_includes))
		{
		}
		else if(begins_with(pData,"#cpp_quote("))			
		{
			auto* start = pData; //save the beginning
			find_end_quote(pData);
			std::string data(start, pData);
			dest << data;
		}
		else
		{
			if(*pData != '\n')
			{
				if(	*pData == ' '  || 
					*pData == '!'  || 
					*pData == '\"' || 
					*pData == '~'  || 
					*pData == '^'  || 
					*pData == '&'  || 
					*pData == '*'  || 
					*pData == '('  || 
					*pData == ')'  || 
					*pData == '}'  || 
					*pData == '{'  || 
					*pData == '['  || 
					*pData == ']'  || 
					*pData == ';'  || 
					*pData == ':'  || 
					*pData == '\'' || 
					*pData == '<'  || 
					*pData == '>'  || 
					*pData == '.'  || 
					*pData == ','  || 
					*pData == '/'  || 
					*pData == '\\' || 
					*pData == '|')	
					bInTheMiddleOfWord = false;
				else
					bInTheMiddleOfWord = true;
				
				if(!ignoreText)
				{
					dest << *pData;
				}
			}
			pData++;
		}
	}
}


/*char IsLogicalExpression(const char* pData)
{
	char isLogicalExpression = '?';
	bool inBrakets = false;
	if(*pData == '(')
	{
		inBrakets = true;
		pData++;
	}

	while(*pData != '\0' && *pData != '\n')
	{
		if(*pData == '(')
		{
			if(isLogicalExpression != '?')
				if(isLogicalExpression != IsLogicalExpression(pData))
					return '?';
			else
				isLogicalExpression = IsLogicalExpression(pData);
		}
		if(*pData == ')')
		{
			pData++;
			break;
		}
		if(*pData == '<' || *pData == '>' || (*pData == '=' && *(pData+1) == '=') || *pData == '&' || *pData == '|' || *pData == '!')
			isLogicalExpression = 'y';

		pData++;
	}
	if(isLogicalExpression == '?')
		isLogicalExpression = 'n';
	return isLogicalExpression;
}*/

std::string macro_parser::not_(const std::string val, const std::string)
{
	if(val == "true")
		return "false";
	if(val == "false")
		return "true";
	if(!atoi(val.data()))
		return "true";
	return "false";
};
std::string macro_parser::bnot(const std::string val, const std::string)
{
	assert(val != "true" && val != "false");
	char buf[16];
	sprintf(buf,"%d",~atoi(val.data()));
	return buf;
};
std::string macro_parser::mult(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) * atoi(val2.data()));
	return buf;
};
std::string macro_parser::div(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) / atoi(val2.data()));
	return buf;
};
std::string macro_parser::mod(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) % atoi(val2.data()));
	return buf;
};
std::string macro_parser::add(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) + atoi(val2.data()));
	return buf;
};

std::string macro_parser::sub(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) - atoi(val2.data()));
	return buf;
};

std::string macro_parser::shiftl(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) << atoi(val2.data()));
	return buf;
};

std::string macro_parser::shiftr(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) >> atoi(val2.data()));
	return buf;
};

std::string macro_parser::lt(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) < atoi(val2.data()));
	return buf;
};

std::string macro_parser::lte(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) <= atoi(val2.data()));
	return buf;
};

std::string macro_parser::gt(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) > atoi(val2.data()));
	return buf;
};

std::string macro_parser::gte(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) >= atoi(val2.data()));
	return buf;
};

std::string macro_parser::eq(const std::string val1,const std::string val2)
{
	if(val1 == val2)
		return "true";
	return "false";
};

std::string macro_parser::neq(const std::string val1,const std::string val2)
{
	if(val1 != val2)
		return "true";
	return "false";
};

std::string macro_parser::band(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) & atoi(val2.data()));
	return buf;
};

std::string macro_parser::bxor(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) ^ atoi(val2.data()));
	return buf;
};

std::string macro_parser::bor(const std::string val1,const std::string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) | atoi(val2.data()));
	return buf;
};

std::string macro_parser::and_(const std::string val1,const std::string val2)
{
	if((val1 == "true" || atoi(val1.data())) && (val2 == "true" || atoi(val2.data())))
		return "true";
	return "false";
};

std::string macro_parser::or_(const std::string val1,const std::string val2)
{
	if(val1 == "true" || atoi(val1.data()) || val2 == "true" || atoi(val2.data()))
		return "true";
	return "false";
};

bool macro_parser::IsOperator(const char* pData, std::string& operatorString)
{
	for(int i = 0; i < operators.size();i++)
	{
		if(begins_with(pData,operators[i]))
		{
			operatorString = operators[i];
			return true;
		}
	}
	return false;	
}


//reduce a complex expression to its value
std::string macro_parser::ReduceExpression(const char*& pData, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
//	pData++;  //gobble up those leading brackets

	std::list<std::string> components;
	std::string temp;

	while(*pData != '\0' && *pData != '\n')
	{
		if(*pData == ' ')
		{
			while(*pData == ' ')
				pData++;

			temp += ' ';
			continue;
		}

		if(*pData == '(')
		{
			if(temp == "defined")
			{
				std::string expr = ExtractExpression(pData, includeDirectories, loaded_includes);
				assert(expr[0] == '(');

				expr = expr.substr(1,expr.length() - 2); //strip out the brackets
				
				if(defines.find(expr) != defines.end())
					components.push_back("true");
				else
					components.push_back("false");
			}
			else
			{
				if(temp.length())
				{
					components.push_back(temp);
					temp = "";
				}

				std::string expr = ExtractExpression(pData, includeDirectories, loaded_includes);

				const char* exprData = expr.data();
				temp = ReduceExpression(exprData, includeDirectories, loaded_includes);

				components.push_back(temp);
			}
			temp = "";
			continue;
		}
		if(*pData == ')' || *pData == '\0')
		{
			if(temp.length())
			{
				components.push_back(temp);
				temp = "";
			}
			pData++;
			break;
		}

		std::string operatorString;
		if(IsOperator(pData,operatorString))
		{
			if(temp.length())
			{
				components.push_back(temp);
				temp = "";
			}
			components.push_back(operatorString);
			pData += operatorString.length();
			continue;
		}
		if(!extract_word(pData,temp))
		{
			assert(false);
			return "";
		}
		components.push_back(temp);
		temp = "";
	}
	if(temp.length())
		components.push_back(temp);

	while(*pData == '\n' || *pData == ' ')
		pData++;

	//Having reduced the expresion to a list of values sandwiched between operatorBehaviours we now evaluate the expression
	//( ) Function call   //should now be evaluated
	
	for(std::list<std::string>::iterator it = components.begin(); it != components.end(); it++)
	{
		for(int i = 0;i < operatorBehaviours.size() && components.size() > 1;i++)
		{
			if(*it == operatorBehaviours[i].m_name)
			{
				if(operatorBehaviours[i].m_isunary)
				{
					it++;
					std::string value = *it;
					it++;

					components.insert(it,std::invoke(operatorBehaviours[i].m_fn, this, value,""));
					it--;
					std::list<std::string>::iterator it2 = it;
					it--;
					it--;
					std::list<std::string>::iterator it1 = it;
					components.erase(it1,it2);
					it = components.begin();
				}
				else
				{
					it--;
					std::string value1 = *it;
					it++;
					it++;
					std::string value2 = *it;
					it++;
					components.insert(it,std::invoke(operatorBehaviours[i].m_fn, this, value1,value2));
					it--;
					std::list<std::string>::iterator it2 = it;
					it--;
					it--;
					it--;
					std::list<std::string>::iterator it1 = it;
					components.erase(it1,it2);
					it = components.begin();
				}
				break;
			}
		}
	}
	if(!components.size())
		return "";
	assert(components.size() == 1);
	return *components.begin();
}

bool macro_parser::SubstituteMacro(int ignoreText, const char*& pData, std::ostream& dest, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
	const char* tempdata = pData;

	//first find if the leading std::string is a macro
	std::unordered_map<std::string, definition>::iterator it;
	{
		//first pickle out the potential name of the definition
		std::string macroName;
		if(!extract_word(tempdata,macroName))
			return false;

		//find if it exists in the macros map
		it = defines.find(macroName);
		if(it == defines.end())
			return false; // no return

		/*if(macroName == "_CRT_DEPRECATE_TEXT")
		{
			macroName = macroName;
		}*/

		pData += macroName.length();
	}


	//found it now implement the substitution
	definition& def = (*it).second;

	//get past the spaces
	while(*pData == ' ')
		pData++;

	//extract the list of parameter values used for this macro
	std::vector<std::string> paramsList;
	if(*pData == '(') //this define has a parameter list
	{
		pData++;

		while(*pData == ' ' || *pData == '\t')
		{
			pData++;
		}

		bool inQuotes = false;

		//get the parameters
		std::string param;
		while(*pData)
		{
			if(inQuotes == true && *pData == '\\' && pData[1] == '\"')
			{
				pData += 2;
				param += '\"';
				continue;
			}
			else if(*pData == '\"')
			{
				inQuotes = !inQuotes;
			}

			if(inQuotes == false && (*pData == ')' || *pData == ','))
			{
				while(1)
				{
					if(param[0] == ' ')
					{
						param = param.substr(1,param.length() - 1);
						continue;
					}
					if(param[param.length() - 1] == ' ')
					{
						param = param.substr(0,param.length() - 1);
						continue;
					}
					break;
				}
				//yuk expand any macros inside the parameter inside this temporary buffer:

				//expand any macros
				{
					const char* bufpos = param.data();
					std::stringstream output;
					CleanBuffer(bufpos, output, includeDirectories, 0, 0, loaded_includes);
					output << std::ends;

					paramsList.push_back(output.str());

					if(*pData == ')')
					{
						pData++;
						break;
					}

					while(*pData == ' ')
						pData++;

					if(*pData != ',')
					{
						std::stringstream err;
						err << "error expecting ','";
						err << std::ends;
						std::string errorMsg(err.str());
						throw std::runtime_error(errorMsg);
					}
					pData++;

					param = "";
				}
			}
			else
				param += *pData++;
		}
	}

	assert(paramsList.size() == def.m_paramsList.size());

	//now we expand the substitution std::string against any parameters
	std::string newSubstString;
	for(unsigned int i = 0; i < def.m_substitutionString.length();)
	{
		//copy spaces;
		while((def.m_substitutionString[i] == ' ' || def.m_substitutionString[i] == '(' || def.m_substitutionString[i] == ')') && def.m_substitutionString.length() > i)
			newSubstString += def.m_substitutionString[i++];

		//now extract a morcel of std::string data 
		std::string morcel;
		while(def.m_substitutionString[i] != ' ' && def.m_substitutionString[i] != '#' && def.m_substitutionString[i] != '(' && def.m_substitutionString[i] != ')' && def.m_substitutionString.length() > i)
			morcel += def.m_substitutionString[i++];

		//substitute parameters to values
		for(unsigned int pit = 0; pit != def.m_paramsList.size();pit++)
		{
			if(def.m_paramsList[pit] == morcel)
			{
				morcel = paramsList[pit];
				break;
			}
		}
		//add it to the buffer
		newSubstString += morcel;

		//deal with special processing of operators
		if(def.m_substitutionString[i] == '#')
		{
			bool concatenate = false;
			i++;
			if(def.m_substitutionString[i] == '#')//concatenate			
			{
				i++;
				concatenate = true;
			}

			while(def.m_substitutionString[i] == ' ')
				i++;

			morcel = "";
			while(def.m_substitutionString[i] != ' ' && def.m_substitutionString[i] != '#' && def.m_substitutionString[i] != '(' && def.m_substitutionString[i] != ')' && def.m_substitutionString.length() > i)
				morcel += def.m_substitutionString[i++];
		
			for(unsigned int pit = 0; pit != def.m_paramsList.size();pit++)
			{
				std::string& paramName = def.m_paramsList[pit];
				if(paramName == morcel)
				{
					morcel = paramsList[pit];
					break;
				}
			}
			if(concatenate)
			{//concatenate
				while(newSubstString[newSubstString.length()-1] == ' ')
					newSubstString = newSubstString.substr(0, newSubstString.length()-1);
				newSubstString += morcel;
			}
			else //stringise
			{
				newSubstString += '"';
				newSubstString += morcel;
				newSubstString += '"';
			}
		}
	}
	if(!ignoreText)
	{
		dest << newSubstString;
	}
	return true;
}

void macro_parser::Define(std::string& var)
{
	definition def;
	std::string defName;

	//pickle out the definition name plus any paramaters
	unsigned int i = 0;
	for(; i < var.length();i++)
	{
		if(var[i] == ' ')
			break;
		if(var[i] == '(')
		{
			i++;
			//get the parameters
			std::string param;
			for(; i < var.length();i++)
			{
				if(var[i] == ')')
				{
					def.m_paramsList.push_back(param);
					param = "";
					i++;
					break;
				}
				if(var[i] == ',')
				{
					def.m_paramsList.push_back(param);
					param = "";
					i++;
					while(var[i] == ' ')
						i++;
				}
				param += var[i];
			}
			break;
		}
		defName += var[i];
	}
	//clean up the remainder of the std::string and put into m_substitutionString
	while(var[i] == ' ')
		i++;
	def.m_substitutionString = var.substr(i,var.length());

	//add the definition to the list
	defines[defName] = def;
}

void macro_parser::UnDefine(std::string& var)
{
	defines.erase(var);
}

bool macro_parser::FindDefString(std::string& var)
{
	bool strip_brackets = false;
	if(var.size() != 0 && var[0] == '(')
	{
		strip_brackets = true;
		for(size_t i = 1; i < var.size() - 1;i++)
		{
			if(var[i] == '(' || var[i] == ')')
			{
				strip_brackets = false;
				break;
			}
		}
		if(var[var.size() - 1] != ')')
		{
			strip_brackets = false;
		}
	}
	if(strip_brackets == true)
	{
		var = var.substr(1, var.size() - 2);
	}
	if(defines.find(var) == defines.end())
		return false;
	return true;
}

bool macro_parser::load(std::ostream& output_file, const std::string& file, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
	if(std::find(loaded_includes.begin(), loaded_includes.end(), file) == loaded_includes.end())
	{
		//loaded_includes.push_back(file);
	}
	
	std::ifstream input_file(file);
	std::string data;
	std::getline(input_file, data, '\0');
	const char* pData = data.data();
	
	CleanBufferOfComments(pData);
	pData = data.data();
	CleanBuffer(pData, output_file, includeDirectories, 0, 0, loaded_includes);

	return true;
}


bool macro_parser::LoadUsingEnv(std::ostream& stream, const std::string& file, const paths& includeDirectories, std::vector<std::string>& loaded_includes)
{
	if(std::filesystem::exists(file))
	{
		return load(stream, file, includeDirectories, loaded_includes);
	}

	if(includeDirectories.empty())
	{
		return false;
	}

	for(auto includeDirectory : includeDirectories)
	{
		auto candidate = includeDirectory/file;
		std::error_code ec;
		candidate = std::filesystem::canonical(candidate, ec).string();
		if(ec.value() == 0)
		{
			std::string modified_file_name = candidate.string();
			std::ifstream pathstream(modified_file_name);
			if(std::filesystem::exists(modified_file_name))
			{
				return load(stream, modified_file_name, includeDirectories, loaded_includes);
			}
		}
	}
	return false;
}

void macro_parser::extract_path_and_load(int ignoreText, std::ostream& stream, const paths& includeDirectories, const char* file, std::vector<std::string>& loaded_includes)
{
	if(ignoreText)
	{
		return;
	}

	if (!LoadUsingEnv(stream, file, includeDirectories,loaded_includes))
	{
		std::stringstream err;
		err << "failed to load " << file << "\n";
		err << std::ends;
		std::string errorMsg(err.str());
		throw std::runtime_error(errorMsg);
	}
}




void CleanBufferOfLineFeedsAndTabs(const char* pData)
{
	char* oldBufPos = (char*)pData;
	while(*pData)
	{
		if(*pData == '\r') //line feeds
		{
			if(pData[-1] != '\n' && pData[1] != '\n')
				*oldBufPos++ = '\n';
			pData++;
			continue;
		}
		if(*pData == '\t') //tabs
		{
			pData++;
			*oldBufPos++ = ' ';
			continue;
		}
		*oldBufPos++ = *pData++;
	}
	*oldBufPos = '\0';
}

void DeleteRestofLine(const char*& pData)
{
	while(*pData != 0 && *pData != '\n')
		pData++;
}

void DeleteCommentBlock(const char*& pData)
{
	while(*pData != 0 && !begins_with(pData,"*/"))
		pData++;
	pData += 2;
}

void CleanBufferOfComments(const char*& pData)
{
	CleanBufferOfLineFeedsAndTabs(pData);

	char* oldBufPos = (char*)pData;
	//strip out comments and endls;
	bool changed = true;
	while(changed)
	{
		changed = false;
		while(*pData != 0)
		{
			if(is_preproc(pData,"pragma"))
			{
				while(*pData != 0 && *pData != '\n')
				{
					pData++;
				}
				while(*pData == '\n')
				{
					pData++;
				}
			}
			//#defines and conditional statements need special handling and are characterised by being terminated by a charage return 
			else if(is_preproc(pData,"define") || is_preproc(pData,"ifdef") || is_preproc(pData,"ifndef") || is_preproc(pData,"if") || is_preproc(pData,"elif") || is_preproc(pData,"endif")  || is_preproc(pData,"error"))		
			{
				if(oldBufPos != pData)
				{
					*oldBufPos++ = '\n';
				}

				while(*pData != '\n'  && *pData)//copy the #define removing any continuance lines
				{
					if(*pData == '\\')
					{
						pData++;

						bool foundSpace = false;
						while(*pData != 0 && *pData == ' ')
						{
							pData++;
							foundSpace = true;
						}
						
						if(*pData == '\n')
						{
							pData++;
							*oldBufPos = ' ';
							oldBufPos++;

							while(*pData != 0 && (*pData == ' '))
								pData++;
						}
						else
						{//some other querk
							*oldBufPos++ = '\\';
							if(foundSpace)
								*oldBufPos++ = ' ';
							*oldBufPos++ = *pData++;
						}
					}
					else if(begins_with(pData,"//"))			
					{
						DeleteRestofLine(pData);
						*oldBufPos = ' ';
						oldBufPos++;
						changed = true;
						continue;
					}
					else if(begins_with(pData,"/*"))			
					{
						DeleteCommentBlock(pData);
						changed = true;
						continue;
					}
					else
						*oldBufPos++ = *pData++;

				}
				*oldBufPos = '\n';
				oldBufPos++;

				if(*pData == 0)
				{
					break;
				}
				pData++;
				continue;
			}
			//#defines and conditional statements need special handling and are characterised by being terminated by a charage return 
			else if(begins_with(pData,"#cpp_quote("))			
			{
				const char* start = pData;
				find_end_quote(pData);
				while(start != pData)
				{
					*oldBufPos = *start;
					start++;
					oldBufPos++;
				}
			}
			/*else if(is_preproc_eat(pData,"pragma"))
			{
				changed = true;
				int bracketCount = 0;
				bool bInComments = false;
				while(*pData != 0)
				{
					if(bInComments)
					{
						if(is_word(pData,"\\\\"))
							pData++;
						else if(is_word(pData,"\\\""))
							pData++;
						else if(*pData == '\"')
							bInComments = false;
						pData++;
						continue;

					}
					else if(*pData == ')' && bracketCount == 1)
					{
						pData++;
						break;
					}
					else if(*pData == '(')
						bracketCount++;
					else if(*pData == ')')
						bracketCount--;
					else if(*pData == '\"')
						bInComments = true;

					pData++;
				}
				continue;
			}*/
			else if(begins_with(pData,"//"))			
			{
				DeleteRestofLine(pData);
				*oldBufPos = ' ';
				oldBufPos++;
				changed = true;
				continue;
			}
			else if(begins_with(pData,"/*"))			
			{
				DeleteCommentBlock(pData);
				changed = true;
				continue;
			}
			else if(begins_with(pData,"\\"))
			{
				const char* pTempData = ++pData;
				while(*pTempData != 0 && *pTempData == ' ')
					pTempData++;
				if(*pTempData == '\n')
				{
					*oldBufPos = ' ';
					oldBufPos++;

					pData = ++pTempData;
					while(*pData == '\n')
						pData++;
				}
				continue;
			}
			else if(is_word(pData,"  "))			
			{
				changed = true;
				do
				{
					pData++;
				}while(is_word(pData,"  "));

				continue;
			}
			else
			{
				if(*pData == '\n')
				{
					*oldBufPos = ' ';
					oldBufPos++;
					pData++;

					while(*pData == '\n')
						pData++;
				}
				else
				{
					*oldBufPos = *pData;
					oldBufPos++;
					pData++;
				}
			}
		}
		*oldBufPos = 0;
	}
}
