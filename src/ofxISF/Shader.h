#pragma once

#include "Constants.h"
#include "Params.h"
#include "JSONParser.h"

OFX_ISF_BEGIN_NAMESPACE

class Shader
{
public:

	Shader()
	:
		json_parser(render_size, params, presistent_buffers, passes),
		code_gen(params),
		current_framebuffer(NULL),
		result_texture(NULL),
		internalformat(GL_RGB)
	{}

	void setup(int w, int h, int internalformat = GL_RGB)
	{
		render_size.set(w, h);
		this->internalformat = internalformat;
		
		ofFbo &fbo = framebuffer_map["DEFAULT"];
		fbo.allocate(render_size.x, render_size.y, internalformat);
		
		fbo.begin();
		ofClear(0);
		fbo.end();
	}

	bool load(const string& path)
	{
		if (!ofFile::doesFileExist(path))
		{
			ofLogError("ofxISF") << "no such file";
			return false;
		}
		
		name = ofFilePath::getBaseName(path);
		
		string data = ofBufferFromFile(path).getText();
		if (!parse_directive(data, header_directive, shader_directive)) return false;
		if (!reload_shader()) return false;

		return true;
	}

	void update()
	{
		const vector<Ref_<ImageParam> >& images = params.getImageParams();
		bool need_reload_shader = false;
		for (int i = 0; i < images.size(); i++)
		{
			if (images[i]->checkTextureFormatChanged())
			{
				need_reload_shader = true;
				break;
			}
		}
		if (need_reload_shader) reload_shader();
		
		current_framebuffer = &framebuffer_map["DEFAULT"];
		current_framebuffer->begin();
		ofClear(0);
		current_framebuffer->end();
		
		if (passes.empty())
		{
			render_pass(0);
		}
		else
		{
			for (int i = 0; i < passes.size(); i++)
			{
				Pass &pass = passes[i];
				if (!pass.target.empty())
				{
					current_framebuffer = &framebuffer_map[pass.target];
				}
				else
				{
					current_framebuffer = &framebuffer_map["DEFAULT"];
				}
				render_pass(i);
			}
		}
	}

	void draw(float x, float y, float w, float h)
	{
		current_framebuffer->draw(x, y, w, h);
	}
	
	void draw(float x, float y)
	{
		current_framebuffer->draw(x, y);
	}
	
	//
	
	void clear(const ofColor& color)
	{
		current_framebuffer->begin();
		ofClear(color);
		current_framebuffer->end();
	}
	
	void clear(float r, float g, float b, float a = 255)
	{
		current_framebuffer->begin();
		ofClear(r, g, b, a);
		current_framebuffer->end();
	}

	void clear(float b, float a = 255)
	{
		current_framebuffer->begin();
		ofClear(b, a);
		current_framebuffer->end();
	}

	//
	
	ofTexture& getTextureRecerence()
	{
		return *result_texture;
	}
	
	//
	
	void setImage(ofTexture *img)
	{
		if (getImageParams().empty()) return;
		const string& name = getImageParams().front()->getName();
		params.setParam<ofTexture*>(name, img);
	}
	
	void setImage(ofTexture &img)
	{
		if (getImageParams().empty()) return;
		const string& name = getImageParams().front()->getName();
		setImage(name, &img);
	}
	
	void setImage(ofImage &img)
	{
		if (getImageParams().empty()) return;
		const string& name = getImageParams().front()->getName();
		setImage(name, &img.getTextureReference());
	}

	//
	
	template <typename INT_TYPE, typename EXT_TYPE>
	void setParam(const string& name, const EXT_TYPE& value)
	{
		params.setParam<INT_TYPE>(name, value);
	}
	
	void setImage(const string& name, ofTexture *img)
	{
		params.setParam<ofTexture*>(name, img);
	}

	void setImage(const string& name, ofTexture &img)
	{
		setImage(name, &img);
	}

	void setImage(const string& name, ofImage &img)
	{
		setImage(name, &img.getTextureReference());
	}
	
	//
	
	template <typename T>
	bool hasParam(const string& name) const
	{
		if (!params.hasParam(name)) return false;
		if (!params.getParam(name)->isTypeOf<T>()) return false;
		return true;
	}
	
	bool hasImage(const string& name) const
	{
		return hasParam<ofTexture*>(name);
	}
	
	void dumpShader() const
	{
		code_gen.dumpShader();
	}

	//
	
	const string& getName() const { return name; }
	const string& getDescription() const { return json_parser.getDescription(); }
	const string& getCredit() const { return json_parser.getCredit(); }
	const vector<string>& getCategories() const { return json_parser.getCategories(); }
	
	//
	
	const vector<Ref_<ImageParam> >& getImageParams() const { return params.getImageParams(); }
	const vector<ofTexture*>& getTextures() const { return textures; }
	
protected:

	ofVec2f render_size;

	string name;
	string description;
	string credit;
	vector<string> categories;

	Params params;
	JSONParser json_parser;
	CodeGenerator code_gen;

	string header_directive;
	string shader_directive;

	map<string, ofFbo> framebuffer_map;
	ofFbo *current_framebuffer;
	
	vector<ofTexture*> textures;
	ofTexture *result_texture;
	
	vector<PresistentBuffer> presistent_buffers;
	vector<Pass> passes;
	
	ofShader shader;
	
	int internalformat;

protected:
	
	bool parse_directive(const string &data, string& header_directive, string& shader_directive)
	{
		const string header_begin = "/*";
		const string header_end = "*/";

		string::size_type begin_pos = data.find_first_of(header_begin) + header_end.size();
		string::size_type end_pos = data.find_first_of(header_end, begin_pos);
		string::size_type count = end_pos - begin_pos;

		if (begin_pos == end_pos
			|| begin_pos == std::string::npos
			|| end_pos == std::string::npos)
		{
			ofLogError("ofxISF") << "invalid format: missing header delective";
			return false;
		}

		header_directive = data.substr(begin_pos, count);

		string::size_type shader_begin = end_pos + header_end.size();
		shader_directive = data.substr(shader_begin);

		return true;
	}
	
	bool reload_shader()
	{
		textures.clear();
		current_framebuffer = &framebuffer_map["DEFAULT"];
		
		textures.push_back(&framebuffer_map["DEFAULT"].getTextureReference());
		
		if (!json_parser.parse(header_directive)) return false;
		
		for (int i = 0; i < presistent_buffers.size(); i++)
		{
			const PresistentBuffer &buf = presistent_buffers[i];
			ofFbo &fbo = framebuffer_map[buf.name];
			textures.push_back(&fbo.getTextureReference());
			
			if (!fbo.isAllocated())
			{
				fbo.allocate(buf.width, buf.height, internalformat);
				
				fbo.begin();
				ofClear(0);
				fbo.end();
			}
			
			ImageParam *param = new ImageParam(buf.name);
			param->set(&fbo.getTextureReference());
			params.addParam(buf.name, Param::Ref(param));
		}
		
		{
			string result_texture_target_name = "";
			if (!passes.empty())
			{
				result_texture_target_name = passes.back().target;
			}
			
			if (result_texture_target_name == "")
				result_texture_target_name = "DEFAULT";
			
			result_texture = &framebuffer_map[result_texture_target_name].getTextureReference();
		}
		
		if (!code_gen.generate(shader_directive)) return false;
		
		shader.unload();
		if (!shader.setupShaderFromSource(GL_VERTEX_SHADER, code_gen.getVertexShader()))
		{
			cout << code_gen.getVertexShader() << endl;
			return false;
		}
		
		if (!shader.setupShaderFromSource(GL_FRAGMENT_SHADER, code_gen.getFragmentShader()))
		{
			cout << code_gen.getFragmentShader() << endl;
			return false;
		}
		
		if (!shader.linkProgram())
		{
			return false;
		}
		
		return true;
	}
	
	void render_pass(int index)
	{
		if (!shader.isLoaded()) return;
		
		current_framebuffer->begin();
		
		shader.begin();
		shader.setUniform1i("PASSINDEX", index);
		shader.setUniform2fv("RENDERSIZE", render_size.getPtr());
		shader.setUniform1f("TIME", ofGetElapsedTimef());
		
		for (int i = 0; i < params.size(); i++)
			params.getParam(i)->updateUniform(&shader);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		
		glTexCoord2f(1, 0);
		glVertex2f(render_size.x, 0);
		
		glTexCoord2f(1, 1);
		glVertex2f(render_size.x, render_size.y);
		
		glTexCoord2f(0, 1);
		glVertex2f(0, render_size.y);
		glEnd();
		
		shader.end();
		
		current_framebuffer->end();
	}
};

OFX_ISF_END_NAMESPACE