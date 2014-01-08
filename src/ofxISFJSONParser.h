#pragma once

#include "ofxISFConstants.h"
#include "ofxISFParams.h"

#include "jsonxx.h"

OFX_ISF_BEGIN_NAMESPACE

class JSONParser
{
public:

	JSONParser(ofVec2f &render_size, Params &params, vector<PresistentBuffer> &presistent_buffers, vector<Pass> &passes)
	:
		render_size(render_size),
		params(params),
		presistent_buffers(presistent_buffers),
		passes(passes)
	{}

	bool parse(const string& header_directive)
	{
		jsonxx::Object o;
		assert(o.parse(header_directive));

		description = o.get<string>("DESCRIPTION", "");
		credit = o.get<string>("CREDIT", "");

		jsonxx::Array a;

		categories.clear();
		a = o.get<jsonxx::Array>("CATEGORIES", jsonxx::Array());
		for (int i = 0; i < a.size(); i++)
			if (a.has<string>(i))
				categories.push_back(a.get<string>(i));

		a = o.get<jsonxx::Array>("INPUTS", jsonxx::Array());
		for (int i = 0; i < a.size(); i++)
		{
			jsonxx::Object o = a.get<jsonxx::Object>(i, jsonxx::Object());
			string name = o.get<string>("NAME", "");
			
			Param::Ref param = setup_input_param(o);
			if (param)
			{
				// param type changed
				if (params.hasParam(name)
					&& params.getParam(name)->getTypeID() != param->getTypeID())
				{
					params.removeParam(name);
				}

				params.addParam(param);
			}
		}
		
		presistent_buffers.clear();
		
		if (o.has<jsonxx::Array>("PERSISTENT_BUFFERS"))
		{
			a = o.get<jsonxx::Array>("PERSISTENT_BUFFERS", jsonxx::Array());
			for (int i = 0; i < a.size(); i++)
			{
				string name = a.get<string>(i);
				
				PresistentBuffer buf;
				buf.name = name;
				buf.width = render_size.x;
				buf.height = render_size.y;
				presistent_buffers.push_back(buf);
			}
		}
		else if (o.has<jsonxx::Object>("PERSISTENT_BUFFERS"))
		{
#if 0
			jsonxx::Object obj = o.get<jsonxx::Object>("PERSISTENT_BUFFERS", jsonxx::Object());
			const jsonxx::Object::container& kv_map = obj.kv_map();
			
			jsonxx::Object::container::const_iterator it = kv_map.begin();
			while (it != kv_map.end())
			{
				string name = it->first;
				PresistentBuffer buf;
				buf.name = name;
				
				// TODO: param expression
				buf.width = ...;
				buf.height = ...;
				
				presistent_buffers.push_back(buf);

				it++;
			}
#endif
			
			throw "not implemented yet";
		}

		passes.clear();
		
		a = o.get<jsonxx::Array>("PASSES", jsonxx::Array());
		for (int i = 0; i < a.size(); i++)
		{
			jsonxx::Object pass = a.get<jsonxx::Object>(i);
			
			string target = pass.get<string>("TARGET", "");
			
			// TODO: param expression
			string width = pass.get<string>("WIDTH", "");
			string height = pass.get<string>("HEIGHT", "");
			
			Pass o;
			o.target = target;
			
			passes.push_back(o);
		}

		return true;
	}

	///
	
	const string& getDescription() const { return description; }
	const string& getCredit() const { return credit; }
	const vector<string>& getCategories() const { return categories; }

protected:

	string description;
	string credit;
	vector<string> categories;

	ofVec2f &render_size;
	Params &params;
	vector<PresistentBuffer> &presistent_buffers;
	vector<Pass> &passes;

protected:

	Param::Ref setup_input_param(const jsonxx::Object& obj)
	{
		string name = obj.get<string>("NAME", "");
		string type = obj.get<string>("TYPE", "");

		Param::Ref param = NULL;

		if (type == "image")
		{
			param = new ImageParam(name);
		}
		else if (type == "bool")
		{
			param = new BoolParam(name, obj.get<bool>("DEFAULT", false));
		}
		else if (type == "float")
		{
			FloatParam *o = new FloatParam(name, obj.get<jsonxx::Number>("DEFAULT", 0));

			if (obj.has<jsonxx::Number>("MIN") && obj.has<jsonxx::Number>("MAX"))
			{
				float m0 = obj.get<jsonxx::Number>("MIN", std::numeric_limits<float>::min());
				float m1 = obj.get<jsonxx::Number>("MAX", std::numeric_limits<float>::max());
				o->setRange(m0, m1);
			}

			param = o;
		}
		else if (type == "color")
		{
			ofFloatColor def;

			jsonxx::Array a = obj.get<jsonxx::Array>("DEFAULT", jsonxx::Array());
			if (a.size() == 4)
			{
				def.r = a.get<jsonxx::Number>(0, 0);
				def.g = a.get<jsonxx::Number>(1, 0);
				def.b = a.get<jsonxx::Number>(2, 0);
				def.a = a.get<jsonxx::Number>(3, 0);
			}

			param = new ColorParam(name, def);
		}
		else if (type == "event")
		{
			param = new EventParam(name);
		}
		else if (type == "point2D")
		{
			ofVec2f def;

			jsonxx::Array a = obj.get<jsonxx::Array>("DEFAULT", jsonxx::Array());
			if (a.size() == 2)
			{
				def.x = a.get<jsonxx::Number>(0, 0);
				def.y = a.get<jsonxx::Number>(1, 0);
			}

			param = new Point2DParam(name, def);
		}

		return param;
	}
};

OFX_ISF_END_NAMESPACE