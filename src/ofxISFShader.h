#pragma once

#include "ofxISFConstants.h"
#include "ofxISFParams.h"
#include "ofxISFJSONParser.h"

OFX_ISF_BEGIN_NAMESPACE

class Shader
{
public:

	Shader()
	:
		json_parser(render_size, params, presistent_buffers, passes),
		code_gen(params)
	{}

	void setup(int w, int h, int internalformat = GL_RGB)
	{
		render_size.set(w, h);
		this->internalformat = internalformat;
		
		ofFbo &fbo = framebuffers["DEFAULT"];
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
		
		current_framebuffer = &framebuffers["DEFAULT"];
		
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
					current_framebuffer = &framebuffers[pass.target];
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
	
	ofTexture& getTextureRecerence()
	{
		return current_framebuffer->getTextureReference();
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
	
	void dumpShader() const
	{
		code_gen.dumpShader();
	}

	//
	
	const string& getDescription() const { return json_parser.getDescription(); }
	const string& getCredit() const { return json_parser.getCredit(); }
	const vector<string>& getCategories() const { return json_parser.getCategories(); }
	
protected:

	ofVec2f render_size;

	string description;
	string credit;
	vector<string> categories;

	Params params;
	JSONParser json_parser;
	CodeGenerator code_gen;

	string header_directive;
	string shader_directive;

	map<string, ofFbo> framebuffers;
	ofFbo *current_framebuffer;
	
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
		current_framebuffer = &framebuffers["DEFAULT"];
		
		if (!json_parser.parse(header_directive)) return false;
		
		for (int i = 0; i < presistent_buffers.size(); i++)
		{
			const PresistentBuffer &buf = presistent_buffers[i];
			ofFbo &fbo = framebuffers[buf.name];
			
			if (!fbo.isAllocated())
			{
				fbo.allocate(buf.width, buf.height, internalformat);
				
				fbo.begin();
				ofClear(0);
				fbo.end();
			}
			
			ImageParam *param = new ImageParam(buf.name);
			param->set(&fbo.getTextureReference());
			params.addParam(Param::Ref(param));
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