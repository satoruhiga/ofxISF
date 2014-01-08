#pragma once

#include "ofxISFConstants.h"

OFX_ISF_BEGIN_NAMESPACE

#define _S(src) # src

class Shader;
class CodeGenerator;
class ImageParam;
template <typename T>
class Param_;

class Param
{
public:

	typedef Ref_<Param> Ref;

	Param(const string& name, unsigned int type_id) : name(name), type_id(type_id)
	{}
	virtual ~Param() {}

	const string& getName() const { return name; }

	template <typename TT>
	bool isTypeOf() const { return Type2Int<TT>::value() == type_id; }
	
	unsigned int getTypeID() const { return type_id; }

protected:

	friend class CodeGenerator;
	friend class Shader;
	
	string name;
	unsigned int type_id;

	virtual string getUniform() const = 0;
	virtual void updateUniform(ofShader *shader) = 0;
};

//

class Params
{
public:

	bool addParam(Param::Ref param)
	{
		if (params_map.find(param->getName()) != params_map.end())
			return false;
		params_map[param->getName()] = param;
		
		updateCache();
		
		return true;
	}
	
	void removeParam(const string& key)
	{
		params_map.erase(key);
	}

	size_t size() const { return params_arr.size(); }

	Param::Ref getParam(size_t idx) const { return params_arr.at(idx); }
	Param::Ref getParam(const string& key) const
	{
		if (params_map.find(key) == params_map.end()) return Param::Ref();
		return params_map[key];
	}
	
	bool hasParam(const string& key) const { return params_map.find(key) != params_map.end(); }
	
	template <typename T0, typename T1>
	void setParam(const string& name, const T1& value);

public:

	const vector<Ref_<ImageParam> >& getImageParams() const { return images; }

protected:

	mutable map<string, Param::Ref> params_map;
	vector<Param::Ref> params_arr;

	vector<Ref_<ImageParam> > images;
	
	void updateCache();
};

//

template <typename T>
class Param_ : public Param
{
public:

	typedef T Type;

	T value;
	T min, max;
	bool has_range;

	Param_(const string& name, const T& default_value = T()) : Param(name, Type2Int<T>::value()), value(default_value), has_range(false) {}

	void setRange(const T& min_, const T& max_)
	{
		has_range = true;
		min = min_;
		max = max_;
	}

	template <typename TT>
	void set(const TT& v)
	{
		value = v;
	}

	template <typename TT>
	TT get() const
	{
		TT v = value;
		return v;
	}
};

class BoolParam : public Param_<bool>
{
public:

	BoolParam(const string& name, const bool& default_value = Type()) : Param_(name, default_value) {}

	void updateUniform(ofShader *shader)
	{
		shader->setUniform1i(name, value);
	}

protected:

	string getUniform() const
	{
		string s = _S(
			uniform bool $NAME$;
		);
		ofStringReplace(s, "$NAME$", getName());
		return s;
	}
};

class FloatParam : public Param_<float>
{
public:

	FloatParam(const string& name, const float& default_value = Type()) : Param_(name, default_value) {}

	void updateUniform(ofShader *shader)
	{
		if (has_range) value = ofClamp(value, min, max);
		shader->setUniform1f(name, value);
	}

protected:

	string getUniform() const
	{
		string s = _S(
			uniform float $NAME$;
		);
		ofStringReplace(s, "$NAME$", getName());
		return s;
	}
};

class ColorParam : public Param_<ofFloatColor>
{
public:

	ColorParam(const string& name, const ofFloatColor& default_value = Type()) : Param_(name, default_value) {}

	void updateUniform(ofShader *shader)
	{
		if (has_range)
		{
			value.r = ofClamp(value.r, min.r, max.r);
			value.g = ofClamp(value.g, min.g, max.g);
			value.b = ofClamp(value.b, min.b, max.b);
			value.a = ofClamp(value.a, min.a, max.a);
		}
		shader->setUniform4fv(name, &value.r);
	}

protected:

	string getUniform() const
	{
		string s = _S(
			uniform vec4 $NAME$;
		);
		ofStringReplace(s, "$NAME$", getName());
		return s;
	}
};

class Point2DParam : public Param_<ofVec2f>
{
public:

	Point2DParam(const string& name, const ofVec2f& default_value = Type()) : Param_(name, default_value) {}

	void updateUniform(ofShader *shader)
	{
		shader->setUniform2fv(name, value.getPtr());
	}

protected:

	string getUniform() const
	{
		string s = _S(
			uniform vec2 $NAME$;
		);
		ofStringReplace(s, "$NAME$", getName());
		return s;
	}
};

class ImageParam : public Param_<ofTexture*>
{
public:

	int texture_unit_id;

	ImageParam(const string& name) : Param_(name, NULL), texture_unit_id(-1), is_rectangle_texture(false) {}

	void updateUniform(ofShader *shader)
	{
		if (value == NULL) return;
		shader->setUniformTexture(name, *value, texture_unit_id);

		string s = "_$NAME$_pct";
		ofStringReplace(s, "$NAME$", getName());
		
		ofVec2f pct = value->getCoordFromPercent(1, 1);
		shader->setUniform2fv(s, pct.getPtr());
	}

	bool isValid() const { return value != NULL; }

	bool isRectangleTexture() const
	{
		if (value == NULL) return false;
		return value->texData.textureTarget == GL_TEXTURE_RECTANGLE_ARB;
	}
	
	bool checkTextureFormatChanged()
	{
		if (value == NULL) return false;
		bool result = is_rectangle_texture != isRectangleTexture();
		is_rectangle_texture = isRectangleTexture();
		return result;
	}

protected:

	bool is_rectangle_texture;
	
	string getUniform() const
	{
		string s = _S(
			uniform $SAMPLER$ $NAME$;
			uniform vec2 _$NAME$_pct;
		);
		ofStringReplace(s, "$NAME$", getName());
		ofStringReplace(s, "$SAMPLER$", isRectangleTexture() ? "sampler2DRect" : "sampler2D");
		return s;
	}
};

class EventParam : public Param_<bool>
{
public:

	EventParam(const string& name) : Param_(name, false) {}

	void updateUniform(ofShader *shader)
	{
		shader->setUniform1i(name, value);
		value = false;
	}

protected:

	string getUniform() const
	{
		string s = _S(
			uniform bool $NAME$;
		);
		ofStringReplace(s, "$NAME$", getName());
		return s;
	}
};

//

template <typename INT_TYPE, typename EXT_TYPE>
inline void Params::setParam(const string& name, const EXT_TYPE& value)
{
	if (params_map.find(name) == params_map.end())
	{
		ofLogError("ofxISF::Params") << "param not found: " << name;
		return;
	}
	
	Param::Ref &p = params_map[name];
	if (!p->isTypeOf<INT_TYPE>())
	{
		ofLogError("ofxISF::Params") << "type mismatch";
		return;
	}
	
	Param_<INT_TYPE> *ptr = (Param_<INT_TYPE>*)p.get();
	ptr->value = value;
}

//

inline void Params::updateCache()
{
	params_arr.clear();
	images.clear();
	
	int texture_unit_id = 0;
	
	map<string, Param::Ref>::iterator it = params_map.begin();
	while (it != params_map.end())
	{
		Param::Ref o = it->second;
		params_arr.push_back(o);
		if (o->isTypeOf<ofTexture*>())
		{
			Ref_<ImageParam> p = o.cast<ImageParam>();
			p->texture_unit_id = ++texture_unit_id;
			images.push_back(p);
		}
		it++;
	}
}

#undef _S

OFX_ISF_END_NAMESPACE