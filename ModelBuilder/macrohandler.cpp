#include "stdafx.h"
#include "macrohandler.h"
#include "sstream"
#include "function_timer.h"
#include <filesystem>

extern xt::function_timer* p_timer;

std::unordered_map<string, definition> defines;
set<string> openedImports;


string ExtractExpression(const char*& pData, string& includeDirectories)
{
	//((sfsd)*(gfg))
	//       ^
	//       | here is a dip, knowing this point we can asertain if we can strip out the outer brackets

	bool haveReachedDip = false;
 	int lowestBracketDip = -1;
	int maxBracketCount = -1;

	string retVal;

	while(*pData == ' ')
		pData++;

	int bracketCount = -1;

	//we need to scan a line until we come accross two words that are not delimited by an operator
	bool reachedSpace = false; 
	bool firstpass = true;
	while(*pData != '\0' && *pData != '\n')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

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

		string tempString;
		
		if(IsWord(pData,"defined"))
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
			
			if(!ExtractWord(pData, tempString))
			{
				cerr << "expected a word insde the brackets of the defined keyword" << endl;
				tempString == "";
			}

			if(tempString == "RC_INVOKED")
			{
			tempString = tempString;
			}

			while(*pData == ' ')
				pData++;
			
			if(*pData != ')' && required_closing == true)
			{
				cerr << "expected a ')'" << endl;
				tempString == "";
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
				lowestBracketDip = min(lowestBracketDip,bracketCount - 1);
			}

			maxBracketCount = max(maxBracketCount,bracketCount);

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

			strstream tempStream;
			const char* tempStringData = tempString.data();
			if(SubstituteMacro(false, tempStringData, tempStream, includeDirectories))
			{
				tempStream << ends;
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

bool processIf(const char*& pData, ostream& dest, string& includeDirectories, int& inIfDef, int& ignoreText, bool& bInTheMiddleOfWord)
{

	if_status status = ifdef;

	if(IsPreprocEat(pData,"ifdef"))			
	{
		status = ifdef;
		bInTheMiddleOfWord = false;
	}
	else if(IsPreprocEat(pData,"ifndef"))			
	{
		status = ifndef;
		bInTheMiddleOfWord = false;
	}
	else if(IsPreprocEat(pData,"if"))			
	{
		status = hash_if;
		bInTheMiddleOfWord = false;
	}

	bool current_expression_true = false;
	bool processExpression = true;
	bool expressionFound = false;
	while(*pData != '\0')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

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
				string var;
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
				const char* pos = pData;
				string expr = ExtractExpression(pData, includeDirectories);

//				assert(IsLogicalExpression(expr.data()));//we want to check that this is a logical expression

				const char* exprString = expr.data();
				string value = ReduceExpression(exprString, includeDirectories);

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
		
		CleanBuffer(pData, dest, includeDirectories, inIfDef + 1, ignoreText);
		
		ignoreText -= current_expression_true ? 0 : 1;

		if(current_expression_true == true)
		{
			expressionFound = true;
			ignoreText++;
		}

		if(IsPreprocEat(pData,"endif"))
		{
			break;
		}
		else if(IsPreprocEat(pData,"else"))
		{
			processExpression = false;
			current_expression_true = !expressionFound;
		}
		else if(IsPreprocEat(pData,"elif"))
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
	return true;
}

void CleanBuffer(const char*& pData, ostream& dest, string& includeDirectories, int inIfDef, int ignoreText)
{
	int amountToReplace = 0;
	bool bInTheMiddleOfWord = false;
	//strip out comments and endls;
	while(*pData != 0)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

		if(inIfDef != 0)
		{
			if(IsPreproc(pData,"endif") || IsPreproc(pData,"else") || IsPreproc(pData,"elif"))
			{
				return;
			}
		}

		if(IsPreprocEat(pData,"include"))			
		{
			bInTheMiddleOfWord = false;

			while(*pData == ' ')
				pData++;

			if(!(*pData == '\"' || *pData == '<'))
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			string var;
			while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
			{
				var += *pData++;
			}

			if(!(*pData == '\"' || *pData == '>'))
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			ParseAndLoad(ignoreText, dest, includeDirectories, var.data());
		}
/*		else if(IsWord(pData,"importlib"))			
		{
			bInTheMiddleOfWord = false;

			pData += 9;

			while(*pData == ' ')
				pData++;

			if(*pData != '(')
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends
				string errString(err.str());
				throw errString;
			}
			pData++;
			if(*pData != '\"')
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			string var;
			while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
			{
				var += *pData++;
			}

			if(*pData != '\"')
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			if(*pData != ')')
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			strstream includeStream;
			Load(includeStream, var.data());
			includeStream << ends;
			output << includeStream.str();
		}*/
		else if(IsPreprocEat(pData,"import"))			
		{
			bInTheMiddleOfWord = false;

			while(*pData == ' ')
				pData++;

			if(*pData != '\"')
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			string var;
			while(*pData != '\0' && *pData != '\n' && *pData != '\"' && *pData != '>')
			{
				var += *pData++;
			}

			if(*pData != '\"')
			{
				strstream err;
				err << "Error unexpected character: " << *pData;
				err << ends;
				string errString(err.str());
				throw errString;
			}
			pData++;

			while(*pData == ' ')
				pData++;

			if(*pData == ';')
			{
				pData++;
			}

			_strlwr((char*)var.data());
			if(openedImports.find(var) == openedImports.end())
			{
				openedImports.insert(var);
				ParseAndLoad(ignoreText, dest, includeDirectories, var.data());
			}
			else
			{
			int i = 0;
			}
		}
		else if(IsPreprocEat(pData,"define"))			
		{
			bInTheMiddleOfWord = false;

			while(*pData == ' ')
				pData++;

			string var;
			while(*pData != '\0' && *pData != '\n')
				var += *pData++;

			if(!ignoreText)
				Define(var);

			while(*pData == '\n')
				pData++;
		}
		else if(IsPreprocEat(pData,"undef"))			
		{
			bInTheMiddleOfWord = false;
			while(*pData == ' ')
				pData++;

			string var;
			while(*pData != '\0' && *pData != '\n')
				var += *pData++;

			if(!ignoreText)
				UnDefine(var);
		}

		else if(IsPreproc(pData,"ifdef") || IsPreproc(pData,"ifndef") || IsPreproc(pData,"if"))			
		{
			processIf(pData, dest, includeDirectories, inIfDef, ignoreText, bInTheMiddleOfWord);
		}
		
		else if(!bInTheMiddleOfWord && SubstituteMacro(ignoreText, pData, dest, includeDirectories))
		{
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
					if(*pData == '#')
					{ignoreText = ignoreText;}
					dest << *pData;
				}
			}
			pData++;
		}
	}

/*	if(!ignoreText)
	{
		output << ends;
		dest << output.str();
	}*/
}


void CleanBufferOfLineFeedsAndTabs(const char* pData)
{
	char* oldBufPos = (char*)pData;
	while(*pData)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

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
	while(*pData != 0 && !BeginsWith(pData,"*/"))
		pData++;
	pData += 2;
}

void CleanBufferOfComments(const char*& pData)
{
	CleanBufferOfLineFeedsAndTabs(pData);

/*	fstream fs;
	fs.open("c:\\test3.idl", ios::out | ios::trunc, filebuf::sh_none );
	fs << pData;
	fs << ends;
	fs.close();
*/

	char* oldBufPos = (char*)pData;
	//strip out comments and endls;
	bool changed = true;
	while(changed)
	{
		changed = false;
		while(*pData != 0)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}

			//#defines and conditional statements need special handling and are characterised by being terminated by a charage return 
			if(IsPreproc(pData,"define") || IsPreproc(pData,"ifdef") || IsPreproc(pData,"ifndef") || IsPreproc(pData,"if") || IsPreproc(pData,"elif") || IsPreproc(pData,"endif")  || IsPreproc(pData,"error") || IsPreproc(pData,"pragma") || IsWord(pData,"cpp_quote"))			
			{
				if(oldBufPos != pData)
				{
					*oldBufPos++ = '\n';
				}

				while(*pData != '\n'  && *pData)//copy the #define removing any continuance lines
				{
					if(p_timer != NULL && p_timer->is_timedOut())
					{
						exit(0);
					}
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
					else if(BeginsWith(pData,"//"))			
					{
						DeleteRestofLine(pData);
						*oldBufPos = ' ';
						oldBufPos++;
						changed = true;
						continue;
					}
					else if(BeginsWith(pData,"/*"))			
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
			/*else if(IfIsWordEat(pData,"cpp_quote") || IsPreprocEat(pData,"pragma"))
			{
				changed = true;
				int bracketCount = 0;
				bool bInComments = false;
				while(*pData != 0)
				{
					if(bInComments)
					{
						if(IsWord(pData,"\\\\"))
							pData++;
						else if(IsWord(pData,"\\\""))
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
			else if(BeginsWith(pData,"//"))			
			{
				DeleteRestofLine(pData);
				*oldBufPos = ' ';
				oldBufPos++;
				changed = true;
				continue;
			}
			else if(BeginsWith(pData,"/*"))			
			{
				DeleteCommentBlock(pData);
				changed = true;
				continue;
			}
			else if(BeginsWith(pData,"\\"))
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
			else if(IsWord(pData,"  "))			
			{
				changed = true;
				do
				{
					pData++;
				}while(IsWord(pData,"  "));

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
	*oldBufPos;
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

string not(const string val, const string)
{
	if(val == "true")
		return "false";
	if(val == "false")
		return "true";
	if(!atoi(val.data()))
		return "true";
	return "false";
};
string bnot(const string val, const string)
{
	assert(val != "true" && val != "false");
	char buf[16];
	sprintf(buf,"%d",~atoi(val.data()));
	return buf;
};
string mult(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) * atoi(val2.data()));
	return buf;
};
string div(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) / atoi(val2.data()));
	return buf;
};
string mod(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) % atoi(val2.data()));
	return buf;
};
string add(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) + atoi(val2.data()));
	return buf;
};

string sub(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) - atoi(val2.data()));
	return buf;
};

string shiftl(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) << atoi(val2.data()));
	return buf;
};

string shiftr(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) >> atoi(val2.data()));
	return buf;
};

string lt(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) < atoi(val2.data()));
	return buf;
};

string lte(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) <= atoi(val2.data()));
	return buf;
};

string gt(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) > atoi(val2.data()));
	return buf;
};

string gte(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) >= atoi(val2.data()));
	return buf;
};

string eq(const string val1,const string val2)
{
	if(val1 == val2)
		return "true";
	return false;
};

string neq(const string val1,const string val2)
{
	if(val1 != val2)
		return "true";
	return false;
};

string band(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) & atoi(val2.data()));
	return buf;
};

string bxor(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) ^ atoi(val2.data()));
	return buf;
};

string bor(const string val1,const string val2)
{
	assert(val1 != "true" && val1 != "false" && val2 != "true" && val2 != "false");
	char buf[16];
	sprintf(buf,"%d",atoi(val1.data()) | atoi(val2.data()));
	return buf;
};

string and(const string val1,const string val2)
{
	if((val1 == "true" || atoi(val1.data())) && (val2 == "true" || atoi(val2.data())))
		return "true";
	return "false";
};

string or(const string val1,const string val2)
{
	if(val1 == "true" || atoi(val1.data()) || val2 == "true" || atoi(val2.data()))
		return "true";
	return "false";
};

const operatorDesc operatorBehaviours[] = {
						operatorDesc("!",not,true), //Logical NOT   
						operatorDesc("~",bnot,true), //Bitwise NOT   
						operatorDesc("*",mult,false), //Multiply Left to right 
						operatorDesc("/",div,false), //Divide   
						operatorDesc("%",mod,false), //Remainder   
						operatorDesc("+",add,false), //Add Left to right 
						operatorDesc("-",sub,false), //Subtract   
						operatorDesc("<<",shiftl,false), //Left shift Left to right 
						operatorDesc(">>",shiftr,false), //Right shift   
						operatorDesc("<",lt,false), //Less than Left to right 
						operatorDesc("<=",lte,false), //Less than or equal to   
						operatorDesc(">",gt,false), //Greater than   
						operatorDesc(">=",gte,false), //Greater than or equal to   
						operatorDesc("==",eq,false), //Equal Left to right 
						operatorDesc("!=",neq,false), //Not equal   
						operatorDesc("&",band,false), //Bitwise AND Left to right 
						operatorDesc("^",bxor,false), //Bitwise exclusive OR Left to right 
						operatorDesc("|",bor,false), //Bitwise OR Left to right 
						operatorDesc("&&",and,false), //Logical AND Left to right 
						operatorDesc("||",or,false) //Logical OR Left to right 
					};


const char* operators[] = {"reinterpret_cast", "dynamic_cast", "static_cast", "const_cast", "delete", "sizeof", "typeid", "new", "<<=", ">>=", "->*", "::", "++", "--", "->", ".*", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||", "%=", "&=", "|=", "*=", "/=", "+=", "-=", "^=", "=", "!", "~", "*", "/", "%", "+", "-", "<", ">", "&", "^", "|", ",", ":", ".", ":", "[", "]", "(", ")", "!", "-", "+", "/"};

bool IsOperator(const char* pData, string& operatorString)
{
	for(int i = 0; i < sizeof(operators)/sizeof(operators[0]);i++)
	{
		if(BeginsWith(pData,operators[i]))
		{
			operatorString = operators[i];
			return true;
		}
	}
	return false;	
}


//reduce a complex expression to its value
string ReduceExpression(const char*& pData, string& includeDirectories)
{
//	pData++;  //gobble up those leading brackets

	list<string> components;
	string temp;

	while(*pData != '\0' && *pData != '\n')
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

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
				string expr = ExtractExpression(pData, includeDirectories);
				assert(expr[0] == '(');
			if(expr == "RC_INVOKED")
			{
			expr = expr;
			}

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

				string expr = ExtractExpression(pData, includeDirectories);

				const char* exprData = expr.data();
				temp = ReduceExpression(exprData, includeDirectories);

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

		string operatorString;
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
		if(!ExtractWord(pData,temp))
			assert(false);
	}
	if(temp.length())
		components.push_back(temp);

	while(*pData == '\n' || *pData == ' ')
		pData++;

	//Having reduced the expresion to a list of values sandwiched between operatorBehaviours we now evaluate the expression
	//( ) Function call   //should now be evaluated
	
	for(int i = 0;i < sizeof(operatorBehaviours)/sizeof(operatorDesc) && components.size() > 1;i++)
	{
		for(list<string>::iterator it = components.begin(); it != components.end(); it++)
		{
			if(*it == operatorBehaviours[i].m_name)
			{
				if(p_timer != NULL && p_timer->is_timedOut())
				{
					exit(0);
				}
				if(operatorBehaviours[i].m_isunary)
				{
					it++;
					string value = *it;
					it++;

					components.insert(it,operatorBehaviours[i].m_fn(value,""));
					it--;
					list<string>::iterator it2 = it;
					it--;
					it--;
					list<string>::iterator it1 = it;
					components.erase(it1,it2);
					it = components.begin();
				}
				else
				{
					it--;
					string value1 = *it;
					it++;
					it++;
					string value2 = *it;
					it++;
					components.insert(it,operatorBehaviours[i].m_fn(value1,value2));
					it--;
					list<string>::iterator it2 = it;
					it--;
					it--;
					it--;
					list<string>::iterator it1 = it;
					components.erase(it1,it2);
					it = components.begin();
				}
			}
		}
	}
	if(!components.size())
		return "";
	assert(components.size() == 1);
	return *components.begin();
}

bool SubstituteMacro(int ignoreText, const char*& pData, ostream& dest, string& includeDirectories)
{
	const char* tempdata = pData;

	//first find if the leading string is a macro
	std::unordered_map<string, definition>::iterator it;
	{
		//first pickle out the potential name of the definition
		string macroName;
		if(!ExtractWord(tempdata,macroName))
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
	vector<string> paramsList;
	if(*pData == '(') //this define has a parameter list
	{
		pData++;

		while(*pData == ' ' || *pData == '\t')
		{
			pData++;
		}

		bool inQuotes = false;

		//get the parameters
		string param;
		while(*pData)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
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
					if(p_timer != NULL && p_timer->is_timedOut())
					{
						exit(0);
					}
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
					strstream output;
					CleanBuffer(bufpos, output, includeDirectories);
					output << ends;

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
						strstream err;
						err << "error expecting ','";
						err << ends;
						string errorMsg(err.str());
						throw errorMsg;
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

	//now we expand the substitution string against any parameters
	string newSubstString;
	for(unsigned int i = 0; i < def.m_substitutionString.length();)
	{
		//copy spaces;
		while((def.m_substitutionString[i] == ' ' || def.m_substitutionString[i] == '(' || def.m_substitutionString[i] == ')') && def.m_substitutionString.length() > i)
			newSubstString += def.m_substitutionString[i++];

		//now extract a morcel of string data 
		string morcel;
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
				string& paramName = def.m_paramsList[pit];
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
		dest << newSubstString << ' ';
	}
	return true;
}


bool operator == (const definition& first, const definition& second)
{
	return first.m_defName == second.m_defName;
}

void Define(string& var)
{
	definition def;

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
			string param;
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
		def.m_defName += var[i];
	}
	//clean up the remainder of the string and put into m_substitutionString
	while(var[i] == ' ')
		i++;
	def.m_substitutionString = var.substr(i,var.length());

	//add the definition to the list
	defines.insert(std::unordered_map<string, definition>::value_type(def.m_defName, def));
}

void UnDefine(string& var)
{
	defines.erase(var);
}

bool FindDefString(string& var)
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

bool Load(ostream& stream, istream& file, string& includeDirectories)
{
	char* pData = NULL;
	{
		string file_data = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		pData = new char[file_data.length() + 1];
		memset(pData, 0, file_data.length() + 1);
		strcpy(pData, file_data.data());
	}

	const char* pTmpDataPtr = pData;//keep the start of pointer position
	CleanBufferOfComments(pTmpDataPtr);
	pTmpDataPtr = pData;//reset the start of pointer position
	CleanBuffer(pTmpDataPtr, stream, includeDirectories);

	delete [] pData;
	return true;
}


bool LoadUsingEnv(ostream& stream, const string& file, string& includeDirectories)
{
	const char* includes = includeDirectories.data();
	if(includes == NULL || includeDirectories.length() == 0)
	{
		return false;
	}

	while(*includes != NULL)
	{
		if(p_timer != NULL && p_timer->is_timedOut())
		{
			exit(0);
		}

		string path;
		while(*includes && *includes != ';')
		{
			path += *includes++;
		}
		path += '\\' + file;

		std::error_code ec;
		path = std::experimental::filesystem::canonical(path, ec).string();
		if(ec.value() == 0)
		{
			ifstream pathstream(path.data());
			if(pathstream.is_open())
			{
				if(Load(stream, pathstream, includeDirectories) == true)
				{
					return true;
				}
			}
		}

		if(*includes == ';')
		{
			includes++;
		}
	}
	return false;
}

void ParseAndLoad(int ignoreText, ostream& stream, string& includeDirectories, const char* file)
{
	if(ignoreText)
	{
		return;
	}

	const char* fname_ext = std::max(strrchr( file, '\\'), strrchr( file, '/'));
	if(fname_ext != NULL)
/*	char path[MAX_PATH];
	char drive[_MAX_DRIVE];
	char directory[_MAX_DIR];
	_splitpath(file,drive, directory,NULL,NULL);
	if(*drive || *directory)*/
	{
		istringstream pathstream(file);
		if(Load(stream, pathstream, includeDirectories) == true)
		{
			return;	
		}
		if (!LoadUsingEnv(stream, file, includeDirectories))
		{
			strstream err;
			err << "failed to load " << file << endl;
			err << ends;
			string errorMsg(err.str());
			throw errorMsg;
		}
	}
/*	else if(!file)
	{
		if(!LoadUsingEnv(file))
		{
			strstream err;
			err << "failed to load " << path << endl;
			err << ends;
			string errorMsg(err.str());
			throw errorMsg;
		}
	}*/
	else
	{
		string path = file;
/*		const char* fname_ext = std::max(strrchr( file, '\\'), strrchr( file, '/'));
		if(fname_ext != NULL)
		{
			path.assign(file, fname_ext - file + 1);
			path += temp.data());
		}
		else
		(
			strcpy(path, temp.data());
		)
		_splitpath(file,drive, directory,NULL,NULL);
		_makepath(path,drive, directory,file,NULL);*/

		std::error_code ec;
		path = std::experimental::filesystem::canonical(path, ec).string();

		ifstream pathstream(path.data());
		if(pathstream.is_open())
		{
			Load(stream, pathstream, includeDirectories);
		}
		else
		{
			if(LoadUsingEnv(stream, file, includeDirectories) == false)
			{
				strstream err;
				err << "failed to load " << path << endl;
				err << ends;
				string errorMsg(err.str());
				throw errorMsg;
			}
		}
	}
}