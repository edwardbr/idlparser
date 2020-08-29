//this file contains classes that describe further interpretation of the data in inside the class object model

struct size_is_item
{
	bool isParam;
	string name;
	size_t pos;
	size_is_item() : isParam(false), pos(-1)
	{}
	size_is_item(bool ip, string n, size_t p) : isParam(ip), name(n), pos(p)
	{}
	size_is_item(const size_is_item& other) : isParam(other.isParam), name(other.name), pos(other.pos)
	{}
	void operator=(const size_is_item& other)
	{
		isParam = other.isParam;
		name = other.name;
		pos = other.pos;
	}
};

struct paramInfo
{
	paramInfo() : 
				starCount(0), 
				referenceCount(0), 
				m_param(0), 
				isTemplate(false), 
				iid_is(-1), 
				refiid_is(-1), 
				isArray(false), 
				isSize(false), 
				isLength(false), 
				array_ptr_ref(-1),
				array_length_ref(-1),
				isInParam(false), 
				isOutParam(false), 
				isInOutParam(false)
	{}
	int starCount;
	int referenceCount;
	typeInfo typeinfo;
	size_t iid_is;
	size_t refiid_is;
	bool isTemplate;
	bool isArray;
	bool isSize;
	bool isLength;
	bool isInParam;
	bool isOutParam;
	bool isInOutParam;
	size_t array_length_ref;
	size_t array_ptr_ref;

	list<size_is_item> size_is_breakdown;
	const ParameterObject& GetParam()
	{
		return *m_param;
	}
	void SetParam(const ParameterObject& param)
	{
		m_param = &param;
	}
	const ParameterObject* m_param;
};

struct functionInfo
{
	functionInfo(const FunctionObject& fn, const Library& lookup) : retval(-1), m_paramSize(0), templateCount(0), arrayCount(0)
	{
		m_paramSize = fn.parameters.size();
		m_params = new paramInfo[m_paramSize];

		size_t pos = 0;

		//create a list of params sorted by name length - to prevent StrCmp2 picking up the wrong parameter
		list<size_is_item> sortedParams;

		for(list<ParameterObject>::const_iterator pit = fn.parameters.begin();pit != fn.parameters.end();pit++)
		{
			const ParameterObject& po = *pit;
			paramInfo& param = m_params[pos];

			bool found = false;
			for(list<size_is_item>::iterator it = sortedParams.begin(); it != sortedParams.end(); it++)
			{
				if((*it).name.length() <= po.name.length())
				{
					sortedParams.insert(it, size_is_item(true, po.name, pos));
					found = true;
					break;
				}
			}
			if(!found)
				sortedParams.push_back(size_is_item(true, po.name, pos));

			param.isInParam = po.HasValue("in") && !po.HasValue("out");
			param.isOutParam = !po.HasValue("in") && po.HasValue("out");
			param.isInOutParam = !param.isInParam && !param.isOutParam;
			
			param.SetParam(po);
			string type = expandTypeString(po.type.data(), lookup);
			getTypeStringInfo(type.data(), param.typeinfo, lookup);

			for(size_t i = 0; i < param.typeinfo.suffix.length();i++)
				if(param.typeinfo.suffix[i] == '*')
					param.starCount++;

			param.referenceCount = param.starCount;
			if(!po.m_arraySuffix.empty())
			{
				param.isArray = true;
		//		if(isVariableLengthArray(*po.m_arraySuffix.begin()))
					param.referenceCount++;
			}
			pos++;
		}

		for(pos = 0;pos < m_paramSize ;pos++)
		{
			string name;
			paramInfo& param = m_params[pos];
			const ParameterObject& po = param.GetParam();

			if(po.GetValue("iid_is(", name))
			{
				size_t end = name.find(")");
				name = name.substr(0,end);
				while(name[0] == ' ')
					name = name.substr(1, name.length());
				while(name[name.length() - 1] == ' ')
					name = name.substr(0, name.length() - 1);

				param.isTemplate = true;
				templateCount++;
				for(size_t i = 0; i < m_paramSize - 1; i++)
				{
					if(m_params[i].GetParam().name == name)
					{
						m_params[i].iid_is = pos;
						param.refiid_is = i;
						break;
					}
				}
			}

			//WHAT an effort!
			if(po.GetValue("size_is(", name))
			{
				size_t end = name.find(")");
				name = name.substr(0,end);

				const char* nameData = name.data();

				string tmp;
				for(size_t i = 0;i < name.length();i++)
				{
					bool success = false;
					for(list<size_is_item>::iterator it = sortedParams.begin(); it != sortedParams.end(); it++)
					{
						if(!Strcmp2(nameData, (*it).name.data()))		
						{
							if(tmp.length())
								param.size_is_breakdown.push_back(size_is_item(false, tmp,-1));
							param.size_is_breakdown.push_back(size_is_item(true, (*it).name, (*it).pos));
							i += (*it).name.length();
							nameData += (*it).name.length();
							tmp = "";

							m_params[(*it).pos].array_ptr_ref = pos;
							m_params[(*it).pos].isSize = true;

							success = true;
							break;
						}
					}
					if(!success)
					{
						tmp += name[i];
						nameData++;
					}
				}
				if(tmp.length())
					param.size_is_breakdown.push_back(size_is_item(false, tmp,-1));

				param.isArray = true;
			}
			if(po.GetValue("length_is(", name))
			{
				size_t end = name.find(")");
				name = name.substr(0,end);
				while(name[0] == ' ' || name[0] == '*')
					name = name.substr(1, name.length());
				while(name[name.length() - 1] == ' ')
					name = name.substr(0, name.length() - 1);

				param.isArray = true;

				for(int i = 0; i < m_paramSize;i++)
				{
					if(m_params[i].GetParam().name == name)
					{
						m_params[i].array_ptr_ref = pos;
						m_params[i].isLength = true;
						param.array_length_ref = i;
						break;
					}
				}
			}
		}

		for(pos = 0;pos < m_paramSize ;pos++)
		{
			if(m_params[pos].GetParam().HasValue("retval"))
			{
				retval = pos;
				break;
			}
		}
		if(retval == -1)
		{
			for(pos = m_paramSize -1;pos >= 0;pos--)
			{
				if(m_params[pos].GetParam().HasValue("out") && !m_params[pos].GetParam().HasValue("in") && m_params[pos].refiid_is == -1 && m_params[pos].typeinfo.pObj != NULL && m_params[pos].typeinfo.type == ObjectTypeInterface && !m_params[pos].isArray)
				{
					retval = pos;
					break;
				}
			}
		}
	}
	~functionInfo()
	{
		delete [] m_params;
	}

	bool isVariableLengthArray(string arraysuffix)
	{
		for(int i = 0;arraysuffix[i];i++)
		{
			if(arraysuffix[i] == '*' || arraysuffix[i] == '.')
				return true;
		}
		return false;
	}

	size_t retval;
	size_t m_paramSize;
	size_t templateCount;
	size_t arrayCount;
	paramInfo* m_params;
//	void ExtractParamInfo(const Library& lookup, const FunctionObject& fn, functionInfo& func);
};
