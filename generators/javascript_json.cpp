#include "stdafx.h"

#include "coreclasses.h"
#include <sstream>
#include "CPPParser.h"
#include "javascript_json.h"
#include "function_timer.h"

extern xt::function_timer* p_timer;

namespace javascript_json
{
	namespace json
	{
		string namespace_name = "";

		void writeFunction(const std::string& paramType, const Library& library, std::stringstream* outerjos = NULL);

		set<std::string> json_structures;
		set<std::string> source_structures;
		set<std::string> structures_check;
		set<std::string> json_structures_check;
		
		void buildDependentFuntions(std::string paramType, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			if(isUInt8(paramType) == true
			|| isInt8(paramType) == true
			|| isUInt16(paramType) == true
			|| isInt16(paramType) == true
			|| isUInt32(paramType) == true
			|| isInt32(paramType) == true
			|| isULong(paramType) == true
			|| isLong(paramType) == true
			|| isUInt64(paramType) == true 
			|| isInt64(paramType) == true)
			{
			}
			else if(isBool(paramType) == true)
			{
			}
			else if(isDouble(paramType) == true || isFloat(paramType) == true)
			{
			}
			else if(isCharStar(paramType) == true)
			{
			}
			else if(isString(paramType) == true)
			{}
			else if(isJavaScriptObject(paramType) == true)
			{}
			else if(isVector(paramType) == true)
			{
				buildVector(paramType, library);
			}
			else if(isList(paramType) == true)
			{
				buildList(paramType, library);
			}
			else if(isMap(paramType) == true)
			{
				buildMap(paramType, library);
			}
			else if(isSet(paramType) == true)
			{
				buildSet(paramType, library);
			}
			else if(isEnum(paramType, library) == true)
			{
				buildEnum(paramType, library);
			}
			else
			{
				writeFunction(paramType, library);
			}
		}

		std::string getTemplateTypeAlias(const std::string& templateType)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			if(templateType == "std::string")
			{}
			else if(templateType.substr(0,5) == "std::")
			{
				std::string lead;
				lead += templateType[5];
				lead = toUpper(lead);
				return lead + templateType.substr(6);
			}
			return templateType;
		}

		void buildVector(const std::string& type, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);

			buildDependentFuntions(templateTypeAlias, library);
		}

		void buildList(const std::string& type, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);

			buildDependentFuntions(templateTypeAlias, library);
		}

		void buildMap(const std::string& type, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			
			buildDependentFuntions(templateTypeAlias, library);
		}

		void buildSet(const std::string& type, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string templateType = getTemplateParam(type);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);
			std::string templateTypeAlias = getTemplateTypeAlias(templateType);
			
			buildDependentFuntions(templateTypeAlias, library);

		}

		void buildEnum(const std::string& type, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string templateType = type.substr(type.find_last_of(' ') + 1);
			std::string referenceModifiers;
			stripReferenceModifiers(templateType, referenceModifiers);
			translateType(templateType, library);

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(templateType, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			buildJSONEnum(pObj, templateType);
		}

		void writeJSONFiles(const ClassObject& m_ob, ostream& jos)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::stringstream tmpjos;//throw away
			std::stringstream fnjos;//throw away
			for(std::list<FunctionObject>::const_iterator fit = m_ob.functions.begin(); fit != m_ob.functions.end(); fit++)
			{
				if(fit->type == FunctionTypeMethod)
				{
					std::string json_func_param_list;
					std::string json_func_structure;

					std::string param_list;
					for(list<ParameterObject>::const_iterator pit = fit->parameters.begin(); pit != fit->parameters.end(); pit++)
					{
						bool is_last = false;

						{
							std::list<ParameterObject>::const_iterator temp = pit;
							temp++;
							if(temp == fit->parameters.end())
							{
								is_last = true;
							}
						}

						if(param_list.length() != 0)
						{
							param_list += ", ";
						}
						if(is_last == false)
						{
							if(param_list.length() != 0)
							{
								json_func_structure += ",";
							}
							json_func_param_list += ", ";
							json_func_param_list += pit->name;
							json_func_structure += pit->name + ": " + pit->name;
						}

						std::string unconst_type = unconst(pit->type);
						std::string paramType = unconst_type;

						//remove reference modifiers from paramType
						std::string referenceModifiers;
						stripReferenceModifiers(paramType, referenceModifiers);
						translateType(paramType, m_ob.GetLibrary());
						paramType = unconst(paramType);
						std::string modified_name = pit->name;
						if(referenceModifiers == "*")
						{
							modified_name = std::string("&") + modified_name;
						}

						buildDependentFuntions(paramType, m_ob.GetLibrary());

						param_list += modified_name;
					}

					for(list<ParameterObject>::const_iterator pit = fit->parameters.begin(); pit != fit->parameters.end(); pit++)
					{
						{
							std::list<ParameterObject>::const_iterator temp = pit;
							temp++;
							if(temp != fit->parameters.end())
							{
								continue;
							}
						}

						std::string unconst_type = unconst(pit->type);
						std::string paramType = unconst_type;

						//remove reference modifiers from paramType
						std::string referenceModifiers;
						stripReferenceModifiers(paramType, referenceModifiers);
						translateType(paramType, m_ob.GetLibrary());
						std::string modified_name = pit->name;

						writeType(tmpjos, paramType, pit->name.data(), m_ob.GetLibrary());

						writeFunction(paramType, m_ob.GetLibrary());
					}
					std::string json_func_name = fit->name;
					fnjos << "\t\t\t" << json_func_name << ": function(context, callback" << json_func_param_list << ") {" << std::endl;
					fnjos << "\t\t\t\treturn this.json_call(context, callback, '" << fit->name << "', {" << json_func_structure << "});" << std::endl;
					fnjos << "\t\t\t}," << std::endl;
				}
			}


			jos << "(function() {" << std::endl;
			jos << "\tvar clarinet = (function() {" << std::endl;
			jos << "\t\tvar clarinet = {" << std::endl;
			
			jos << std::endl;
			for(set<std::string>::iterator it = json_structures.begin(); it != json_structures.end(); it++)
			{
				jos << *it << std::endl;
			}

			jos << "\t\t\tsession:null" << std::endl;
			jos << "\t\t};" << std::endl;

			jos << "\t\tclarinet.session = {" << std::endl;
			jos << "\t\t\turl: 'http://localhost:80'," << std::endl;
			jos << "\t\t\thostname: 'localhost'," << std::endl;
			jos << "\t\t\tport: '80'," << std::endl;
			jos << "\t\t\tsession_id: ''," << std::endl;
			jos << "\t\t\tlog_traffic: false," << std::endl;
			jos << "\t\t\treporting_level: clarinet.log_warning | clarinet.log_error," << std::endl;
			jos << "\t\t\tbunch_request_count: 0," << std::endl;
			jos << "\t\t\tbunch_request_callbacks: []," << std::endl;
			jos << "\t\t\tbunch_request_addref: function(){this.bunch_request_count++;}," << std::endl;
			jos << "\t\t\tbunch_request_release: function(callback){" << std::endl;
			jos << "\t\t\t\tvar This = this;" << std::endl;
			jos << "\t\t\t\tif(callback !== null){" << std::endl;
			jos << "\t\t\t\t\tThis.bunch_request_callbacks.push(callback);" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\tThis.bunch_request_count--;" << std::endl;
			jos << "\t\t\t\tif(This.bunch_request_count === 0){" << std::endl;
			jos << "\t\t\t\t\tThis.get_multi_sorted_rows(" << std::endl;
			jos << "\t\t\t\t\t\tnull," << std::endl;
			jos << "\t\t\t\t\t\tfunction(context, resultValue, xmlHttpRequest, message) {" << std::endl;
			jos << "\t\t\t\t\t\t\tif(resultValue === null || resultValue.returnCode != clarinet.return_code.rc_success) {" << std::endl;
			jos << "\t\t\t\t\t\t\t\tfor(var cb in This.bunched_sorted_row_callback){" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\tvar info = This.bunched_sorted_row_callback[cb];" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\tinfo.callback.call(info.callback_object, info.context, null, xmlHttpRequest, message);" << std::endl;
			jos << "\t\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\telse {" << std::endl;
			jos << "\t\t\t\t\t\t\t\tfor(var i = 0; i < resultValue.items.length; i++){" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\tvar item = resultValue.items[i];" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\tvar info = This.bunched_sorted_row_callback[item.name];" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\tif(typeof info !== 'undefined'){" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\t\tinfo.callback.call(info.callback_object, info.context, item, xmlHttpRequest, message);" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\telse{" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\t\tinfo.callback.call(info.callback_object, null, item, xmlHttpRequest, message);" << std::endl;
			jos << "\t\t\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\tThis.bunched_sorted_row_callback = {};" << std::endl;
			jos << "\t\t\t\t\t\t\tfor(var i = 0; i < This.bunch_request_callbacks.length; i++){" << std::endl;
			jos << "\t\t\t\t\t\t\t\tThis.bunch_request_callbacks[i]();" << std::endl;
			jos << "\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\tThis.bunch_request_callbacks = [];" << std::endl;
			jos << "\t\t\t\t\t\t}," << std::endl;
			jos << "\t\t\t\t\t\tThis.bunched_sorted_row_instructions" << std::endl;
			jos << "\t\t\t\t\t);" << std::endl;
			jos << "\t\t\t\t\tThis.bunched_sorted_row_instructions = [];" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t}," << std::endl;
			jos << "\t\t\tbunched_sorted_row_instructions: []," << std::endl;
			jos << "\t\t\tbunched_sorted_row_callback: {}," << std::endl;
			jos << "\t\t\tmakeSalt: function() {" << std::endl;
			jos << "\t\t\t\tvar text = '';" << std::endl;
			jos << "\t\t\t\tvar possible = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';" << std::endl;
			jos << std::endl;
			jos << "\t\t\t\tfor(var i = 0; i < 10; i++)" << std::endl;
			jos << "\t\t\t\t\ttext += possible.charAt(Math.floor(Math.random() * possible.length));" << std::endl;
			jos << "\t\t\t\treturn text;" << std::endl;
			jos << "\t\t\t}," << std::endl;
			jos << std::endl;
			jos << "\t\t\t//principal method for calling json services from the client" << std::endl;
			jos << "\t\t\tjson_call: function(context, callback, method, soapMess) {" << std::endl;
			jos << "\t\t\t\tvar This = this;" << std::endl;
			jos << "\t\t\t\tif(!window.XMLHttpRequest) {// Mozilla, Safari, ..." << std::endl;
			jos << "\t\t\t\t\thttp_request.callback(context, null, null, 'Your browser does not or is configured not to support ajax');" << std::endl;
			jos << "\t\t\t\t\treturn null;" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << std::endl;
			jos << "\t\t\t\tvar http_request = new XMLHttpRequest();" << std::endl;
			jos << "\t\t\t\thttp_request.context = context;" << std::endl;
			jos << "\t\t\t\thttp_request.callback = callback;" << std::endl;
			jos << "\t\t\t\thttp_request.onreadystatechange = function() {" << std::endl;
			jos << "\t\t\t\t\tThis.json_callback(http_request);" << std::endl;
			jos << "\t\t\t\t};" << std::endl;
			jos << "\t\t\t\thttp_request.open('POST', this.url + '/?' + This.makeSalt(), true);" << std::endl;
			jos << "\t\t\t\thttp_request.setRequestHeader('function', method);" << std::endl;
			jos << "\t\t\t\thttp_request.method = method;" << std::endl;
			jos << "\t\t\t\t//http_request.responseType = 'json';" << std::endl;
			jos << "\t\t\t\t//cookie override for window specific sessions, not much good for naked gets though" << std::endl;
			jos << std::endl;
			jos << "\t\t\t\tif(context && typeof context.clarinetSessionId !== 'undefined') {" << std::endl;
			jos << "\t\t\t\t\thttp_request.setRequestHeader('session_id', context.clarinetSessionId);" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\telse if(This.session_id !== '') {" << std::endl;
			jos << "\t\t\t\t\thttp_request.setRequestHeader('session_id', This.session_id);" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\tvar tmp = JSON.stringify(soapMess);" << std::endl;
			jos << "\t\t\t\thttp_request.send(tmp);" << std::endl;
			jos << std::endl;
			jos << "\t\t\t\tif(typeof logger !== 'undefined' && method !== 'log_in') {" << std::endl;
			jos << "\t\t\t\t\tlogger.log('clarinet', tmp);" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << std::endl;
			jos << "\t\t\t\tif(this.log_traffic === true){" << std::endl;
			jos << "\t\t\t\t\twindow.console.log(This.session_id + ' <-- ' + method + ' ' + tmp);" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\treturn http_request;" << std::endl;
			jos << "\t\t\t}," << std::endl;
			jos << std::endl;
			jos << "\t\t\t// this function will be called when result return from web service." << std::endl;
			jos << "\t\t\tjson_callback: function(http_request) {" << std::endl;
			jos << "\t\t\t\t// return value from web service is an xml document." << std::endl;
			jos << "\t\t\t\tvar rawData;" << std::endl;
			jos << "\t\t\t\tif(http_request.readyState === 4) {" << std::endl;
			jos << "\t\t\t\t\tif(http_request.status === 200) {" << std::endl;
			jos << "\t\t\t\t\t\t//interpret the returned json" << std::endl;
			jos << "\t\t\t\t\t\tvar data = null;" << std::endl;
			jos << "\t\t\t\t\t\ttry {" << std::endl;
			jos << "\t\t\t\t\t\t\tif(this.log_traffic === true){" << std::endl;
			jos << "\t\t\t\t\t\t\t\twindow.console.log(this.session_id + ' --> ' + http_request.method + ' ' + http_request.responseText);" << std::endl;
			jos << "\t\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\t\tdata = JSON.parse(http_request.responseText);" << std::endl;
			jos << "\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\tcatch (e) {" << std::endl;
			jos << "\t\t\t\t\t\t\twindow.console.error('An exception has occurred: \"' + e + '\" when processing text: ' + http_request.responseText);" << std::endl;
			jos << "\t\t\t\t\t\t\thttp_request.callback(http_request.context, null, http_request, http_request.status);" << std::endl;
			jos << "\t\t\t\t\t\t\treturn;" << std::endl;
			jos << "\t\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t\thttp_request.callback(http_request.context, data, http_request, http_request.status);" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\telse {" << std::endl;
			jos << "\t\t\t\t\t\thttp_request.callback(http_request.context, null, http_request, http_request.status);" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t}," << std::endl;
			
			jos << fnjos.str();

			jos << "\t\t\tcheckStatus: function (response) {" << std::endl;
			jos << "\t\t\t\tif (response === null) {" << std::endl;
			jos << "\t\t\t\t\treturn {success: false, message: 'Server has gone away'};" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\tif (response.returnCode != clarinet.return_code.rc_success) {" << std::endl;
			jos << "\t\t\t\t\tvar messageText = '';" << std::endl;
			jos << "\t\t\t\t\tif (typeof response.messages != 'undefined') {" << std::endl;
			jos << "\t\t\t\t\t\tmessageText = this.formatMessages(response.messages);" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\telse {" << std::endl;
			jos << "\t\t\t\t\t\tmessageText = 'No message in response';" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t\t" << std::endl;
			jos << "\t\t\t\t\treturn {success: false, message: 'Server returned with a server error : '  + clarinet.return_code[response.returnCode] + ', message : ' + messageText};" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\treturn {success: true};" << std::endl;
			jos << "\t\t\t}," << std::endl;
			jos << std::endl;
			jos << "\t\t\tformatMessages: function (messages) {" << std::endl;
			jos << "\t\t\t\tvar messageText = '';" << std::endl;
			jos << "\t\t\t\tvar report = false;" << std::endl;
			jos << "\t\t\t\tfor (i = 0; i < messages.length; i++) {" << std::endl;
			jos << "\t\t\t\t\tvar message = messages[i];" << std::endl;
			jos << "\t\t\t\t\tif (message.level &= this.reporting_level) {" << std::endl;
			jos << "\t\t\t\t\t\treport = true;" << std::endl;
			jos << "\t\t\t\t\t\tbreak;" << std::endl;
			jos << "\t\t\t\t\t}" << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\tfor (i = 0; report === true && i < messages.length; i++) {" << std::endl;
			jos << "\t\t\t\t\tvar message = messages[i];" << std::endl;
			jos << "\t\t\t\t\tmessageText += clarinet.log_level[message.level] + ': ';" << std::endl;
			jos << "\t\t\t\t\tmessageText += message.message + '\\r\\n'; " << std::endl;
			jos << "\t\t\t\t}" << std::endl;
			jos << "\t\t\t\treturn messageText;" << std::endl;
			jos << "\t\t\t}" << std::endl;
			jos << "\t\t};" << std::endl;
			jos << "\t\treturn clarinet;" << std::endl;
			jos << "\t})();" << std::endl;
			jos << std::endl;
			jos << "\tif (typeof module !== 'undefined' && typeof module.exports !== 'undefined'){" << std::endl;
			jos << "\t\tmodule.exports = clarinet;" << std::endl;
			jos << "\t}" << std::endl;
			jos << "\telse {" << std::endl;
			jos << "\t\twindow.clarinet = clarinet;" << std::endl;
			jos << "\t}" << std::endl;
			jos << "})();" << std::endl;
		}
		
		void buildJSONEnum(const ClassObject* pObj, std::string& unconst_type)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			if(json_structures_check.find(unconst_type) != json_structures_check.end())
			{
				return;
			}
			json_structures_check.insert(unconst_type);

			if(pObj->functions.size() != 0)
			{
				std::stringstream jos;
				std::stringstream jos2;
				jos << "\t\t\t" << unconst_type << ": {" << std::endl;
				int i = 0;
				for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
				{
					if(i != 0)
					{
						jos2 << "," << std::endl;
					}
					string val = fit->returnType;
					if(fit->returnType == "")
					{
						jos2 << "\t\t\t\t'" << i << "': '" << fit->name << "'";
						jos << "\t\t\t\t" << fit->name << ": " <<  i << "," << std::endl;
					}
					else
					{
						jos2 << "\t\t\t\t'" << fit->returnType << "': '" << fit->name << "'";
						jos << "\t\t\t\t" << fit->name << ": " << fit->returnType << "," << std::endl;
					}
					i++;
				}
				jos << std::endl;
				jos << jos2.str().data() << std::endl;
				jos << "\t\t\t}," << std::endl;

				json_structures.insert(jos.str());
			}
		}

		inline std::string trim(std::string& str)
		{
			str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
			str.erase(str.find_last_not_of(' ') + 1);         //surfixing spaces
			return str;
		}
		
		void writeFunctionParameters(const std::string& type, const Library& library, std::stringstream& jos, bool& includeComma)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			const ClassObject* pObj = NULL;
			if(library.FindClassObject(type, pObj) == false || pObj == NULL)
			{
				std::stringstream err;
				err << "unable to find type: " << type;
				err << ends;
				string errString(err.str());
				throw errString;
			}

			if(pObj->parentName != "")
			{
				std::string parent = unconst(pObj->parentName);
				writeFunctionParameters(parent, library, jos, includeComma);
				includeComma = true;
			}


			for(std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();fit != pObj->functions.end();fit++)
			{
				if(fit->type != FunctionTypeVariable)
					continue;

				if(includeComma == true)
				{
					jos << "," << std::endl;
				}
				includeComma = true;
		//		if(fit->type == FunctionTypeVariable)
				{
					std::string unconst_type = unconst(fit->returnType);
					std::string paramType = unconst_type;

					//remove reference modifiers from paramType
					std::string referenceModifiers;
					stripReferenceModifiers(paramType, referenceModifiers);
					translateType(paramType, library);

					std::string name = fit->name;
					trim(name);
					jos << "\t\t\t\t\t" << name << ": ";
					writeType(jos, paramType, ("object." + fit->name).data(), library);
				}
			}
		}

		void writeFunction(const std::string& paramType, const Library& library, std::stringstream* outerjos)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string type = unconst(paramType);

			/*if(isUInt8(type) == true
				|| isInt8(type) == true
				|| isUInt16(type) == true
				|| isInt16(type) == true
				|| isUInt32(type) == true
				|| isInt32(type) == true
				|| isULong(type) == true
				|| isLong(type) == true
				|| isUInt64(type) == true 
				|| isInt64(type) == true
				|| isBool(type) == true
				|| isDouble(type) == true 
				|| isFloat(type) == true
				|| isCharStar(type) == true 
				|| isCharStar(type) == true
				|| isString(type) == true
				|| isVector(type) == true
				|| isList(type) == true
				|| isMap(type) == true
				|| isSet(type) == true
				|| isEnum(type, library) == true)
			{
				return;
			}*/

			const ClassObject* pObj = NULL;
			if(library.FindClassObject(type, pObj) == false || pObj == NULL)
			{
				return;
			}

			if(pObj->type == ObjectEnum)
			{
				return;
			}

			std::stringstream jos;
			jos << "\t\t\tcreate_" << type << ": function()" << std::endl;
			if(json_structures_check.find(jos.str()) != json_structures_check.end())
			{
				return;
			}
			json_structures_check.insert(jos.str());
			jos << "\t\t\t{" << std::endl;
			jos << "\t\t\t\tvar tmp = " << std::endl;
			jos << "\t\t\t\t{" << std::endl;

			bool includeComma = false;
			writeFunctionParameters(type, library, jos, includeComma);
			
			jos << std::endl << "\t\t\t\t};" << std::endl;
			jos << "\t\t\t\treturn tmp;" << std::endl;
			jos << "\t\t\t}," << std::endl;

			json_structures.insert(jos.str());
		}

		bool writeType(ostream& jos, std::string& paramType, const char* paramName, const Library& library)
		{
			if(p_timer != NULL && p_timer->is_timedOut())
			{
				exit(0);
			}
			std::string referenceModifiers;
			std::string translatedType(paramType);
			stripReferenceModifiers(translatedType, referenceModifiers);
			translateType(translatedType, library);
			std::string referenceModifiers2;
			stripReferenceModifiers(translatedType, referenceModifiers2);
			referenceModifiers += referenceModifiers2;
			bool isPointer = false;
			if(referenceModifiers == "*")
			{
				isPointer = true;
			}

			if(isUInt8(translatedType) == true
				|| isInt8(translatedType) == true
				|| isUInt16(translatedType) == true
				|| isInt16(translatedType) == true
				|| isUInt32(translatedType) == true
				|| isInt32(translatedType) == true
				|| isULong(translatedType) == true
				|| isLong(translatedType) == true
				|| isUInt64(translatedType) == true 
				|| isInt64(translatedType) == true)
			{
				jos << "0";
			}
			else if(isBool(translatedType) == true)
			{
				jos << "true";
			}
			else if(isDouble(paramType) == true || isFloat(paramType) == true)
			{
				jos << "0.0";
			}
			else if(isCharStar(translatedType) == true || isCharStar(paramType) == true)
			{
				jos << "''";
			}
			else if(isString(translatedType) == true)
			{
				jos << "''";
			}
			else if(isJavaScriptObject(translatedType) == true)
			{
				jos << "''";
			}
			else if(isVector(paramType) == true)
			{
				jos << "[]";
				buildVector(paramType, library);
			}
			else if(isList(paramType) == true)
			{
				jos << "[]";
				buildList(paramType, library);
			}
			else if(isMap(paramType) == true)
			{
				jos << "[]";
				buildMap(paramType, library);
			}
			else if(isSet(paramType) == true)
			{
				jos << "[]";
				buildSet(paramType, library);
			}
			else if(isEnum(translatedType, library) == true)
			{
				std::string unconst_type = unenum(unconst(translatedType));
				const ClassObject* pObj = NULL;
				if(library.FindClassObject(unconst_type, pObj) == false || pObj == NULL)
				{
					std::stringstream err;
					err << "unable to find type: " << unconst_type;
					err << ends;
					string errString(err.str());
					throw errString;
				}
				std::list<FunctionObject>::const_iterator fit = pObj->functions.begin();
				if(fit != pObj->functions.end())
				{
					jos << namespace_name << "." << unconst_type << "." << fit->name;
				}
				else
				{
					jos << "0";
				}
				buildEnum(unconst_type, library);
			}
			else
			{
				writeFunction(unconst(paramType), library);
				jos << unconst(paramType) << "()";
			}
			return true;
		}
	}
}
