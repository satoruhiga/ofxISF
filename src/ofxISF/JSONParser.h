#pragma once

#include "Constants.h"
#include "Uniforms.h"

#include "jsonxx.h"

OFX_ISF_BEGIN_NAMESPACE

class JSONParser
{
public:

	JSONParser(ofVec2f &render_size, Uniforms &uniforms, vector<PresistentBuffer> &presistent_buffers, vector<Pass> &passes)
	:
		render_size(render_size),
		uniforms(uniforms),
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
			
			Uniform::Ref uniform = setup_input_uniform(o);
			if (uniform)
			{
				// uniform type changed
				if (uniforms.hasUniform(name)
					&& uniforms.getUniform(name)->getTypeID() != uniform->getTypeID())
				{
					uniforms.removeUniform(name);
				}

				uniforms.addUniform(name, uniform);
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
				
				// TODO: uniform expression
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
			
			// TODO: uniform expression
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
	Uniforms &uniforms;
	vector<PresistentBuffer> &presistent_buffers;
	vector<Pass> &passes;

protected:

	Uniform::Ref setup_input_uniform(const jsonxx::Object& obj)
	{
		string name = obj.get<string>("NAME", "");
		string type = obj.get<string>("TYPE", "");

		Uniform::Ref uniform = NULL;

		if (type == "image")
		{
			uniform = new ImageUniform(name);
		}
		else if (type == "bool")
		{
			uniform = new BoolUniform(name, obj.get<bool>("DEFAULT", false));
		}
		else if (type == "float")
		{
			FloatUniform *o = new FloatUniform(name, obj.get<jsonxx::Number>("DEFAULT", 0));

			if (obj.has<jsonxx::Number>("MIN") && obj.has<jsonxx::Number>("MAX"))
			{
				float m0 = obj.get<jsonxx::Number>("MIN", std::numeric_limits<float>::min());
				float m1 = obj.get<jsonxx::Number>("MAX", std::numeric_limits<float>::max());
				o->setRange(m0, m1);
			}

			uniform = o;
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

			uniform = new ColorUniform(name, def);
		}
		else if (type == "event")
		{
			uniform = new EventUniform(name);
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

			uniform = new Point2DUniform(name, def);
		}

		return uniform;
	}
};

OFX_ISF_END_NAMESPACE