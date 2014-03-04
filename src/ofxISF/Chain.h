#pragma once

#include "Shader.h"

OFX_ISF_BEGIN_NAMESPACE

class Chain
{
public:
	
	Chain() : input(NULL), result(NULL) {}
	~Chain()
	{
		for (int i = 0; i < passes.size(); i++)
		{
			delete passes[i];
		}
		
		passes.clear();
		pass_map.clear();
	}
	
	void setup(int width, int height, int internalformat = GL_RGB)
	{
		this->width = width;
		this->height = height;
		this->internalformat = internalformat;
	}
	
	bool load(const string& path, bool enabled = true)
	{
		Shader *shader = new Shader;
		shader->setup(width, height, internalformat);
		
		if (!shader->load(path))
		{
			delete shader;
			return false;
		}
		
		ShaderPass *pass = new ShaderPass;
		pass->enabled = enabled;
		pass->shader = shader;

		passes.push_back(pass);
		pass_map[shader->getName()] = pass;

		return true;
	}
	
	void update()
	{
		if (passes.empty()) return;
		
		ofTexture *tex = input;
		
		for (int i = 0; i < passes.size(); i++)
		{
			ShaderPass &p = *passes[i];
			if (p.enabled == false) continue;
			
			Shader *o = p.shader;
			o->setImage(tex);
			o->update();
			tex = &o->getTextureReference();
		}
		
		result = tex;
	}
	
	inline void draw(float x, float y) { draw(x, y, width, height); }
	
	void draw(float x, float y, float width, float height)
	{
		if (result == NULL) return;
		result->draw(x, y, width, height);
	}
	
	//
	
	inline void setImage(ofTexture *img) { input = img; }
	inline void setImage(ofTexture &img) { setImage(&img); }
	inline void setImage(ofImage &img) { setImage(img.getTextureReference()); }
	
	inline float getWidth() const { return width; }
	inline float getHeight() const { return height; }
	
	//
	
	inline size_t size() const { return passes.size(); }
	inline bool hasShader(const string& name) const { return pass_map.find(name) != pass_map.end(); }
	
	Shader* getShader(size_t index) const { return passes[index]->shader; }
	Shader* getShader(const string& name) const
	{
		if (!hasShader(name)) return NULL;
		return pass_map[name]->shader;
	}
	
	void setEnable(size_t index, bool state) { passes[index]->enabled = state; }
	void setEnable(const string& name, bool state)
	{
		if (!hasShader(name)) return;
		pass_map[name]->enabled = state;
	}

	bool getEnable(size_t index) { return passes[index]->enabled; }
	bool getEnable(const string& name)
	{
		if (!hasShader(name)) return false;
		return pass_map[name]->enabled;
	}

	bool toggle(size_t index) { passes[index]->enabled = !passes[index]->enabled; }
	bool toggle(const string& name)
	{
		if (!hasShader(name)) return false;;
		pass_map[name]->enabled != pass_map[name]->enabled;
	}

protected:
	
	int internalformat;
	int width, height;
	
	struct ShaderPass {
		bool enabled;
		Shader *shader;
	};
	
	vector<ShaderPass*> passes;
	mutable map<string, ShaderPass*> pass_map;
	
	ofTexture *input;
	ofTexture *result;
};

OFX_ISF_END_NAMESPACE