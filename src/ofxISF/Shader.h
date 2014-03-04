#pragma once

#include "Constants.h"
#include "Uniforms.h"

#include "jsonxx.h"

OFX_ISF_BEGIN_NAMESPACE

class Shader
{
public:

	Shader()
		:code_generator(uniforms)
		,current_framebuffer(NULL)
		,result_texture(NULL)
		,internalformat(GL_RGB)
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
		const vector<Ref_<ImageUniform> >& images = uniforms.getImageUniforms();
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
	
	ofTexture& getTextureReference()
	{
		return *result_texture;
	}
	
	//
	
	void setImage(ofTexture *img)
	{
		if (default_image_input_name == "")
		{
			static bool shown = false;
			if (!shown)
			{
				shown = true;
				ofLogError("Shader") << "no default image input";
			}
			
			return;
		}
		uniforms.setUniform<ofTexture*>(default_image_input_name, img);
	}
	
	void setImage(ofTexture &img)
	{
		setImage(&img);
	}
	
	void setImage(ofImage &img)
	{
		setImage(&img.getTextureReference());
	}

	//
	
	template <typename INT_TYPE, typename EXT_TYPE>
	void setUniform(const string& name, const EXT_TYPE& value)
	{
		uniforms.setUniform<INT_TYPE>(name, value);
	}
	
	void setImage(const string& name, ofTexture *img)
	{
		uniforms.setUniform<ofTexture*>(name, img);
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
	bool hasUniform(const string& name) const
	{
		if (!uniforms.hasUniform(name)) return false;
		if (!uniforms.getUniform(name)->isTypeOf<T>()) return false;
		return true;
	}
	
	bool hasImage(const string& name) const
	{
		return hasUniform<ofTexture*>(name);
	}
	
	void dumpShader() const
	{
		code_generator.dumpShader();
	}

	//
	
	const string& getName() const { return name; }
	const string& getDescription() const { return description; }
	const string& getCredit() const { return credit; }
	const vector<string>& getCategories() const { return categories; }

	const Uniforms& getInputs() const { return input_uniforms; }
	
	//
	
	const vector<ofTexture*>& getTextures() const { return textures; }
	
protected:

	ofVec2f render_size;
	int internalformat;
	
	string name;
	string description;
	string credit;
	vector<string> categories;

	vector<Input> inputs;
	vector<PresistentBuffer> presistent_buffers;
	vector<Pass> passes;
	
	string default_image_input_name;
	
	//
	
	Uniforms uniforms;
	Uniforms input_uniforms;
	CodeGenerator code_generator;

	string header_directive;
	string shader_directive;

	map<string, ofFbo> framebuffer_map;
	ofFbo *current_framebuffer;
	
	vector<ofTexture*> textures;
	ofTexture *result_texture;
	
	ofShader shader;

protected:
	
	void render_pass(int index)
	{
		if (!shader.isLoaded()) return;
		
		current_framebuffer->begin();
		
		ofPushStyle();
		ofEnableAlphaBlending();
		ofSetColor(255);
		
		shader.begin();
		shader.setUniform1i("PASSINDEX", index);
		shader.setUniform2fv("RENDERSIZE", render_size.getPtr());
		shader.setUniform1f("TIME", ofGetElapsedTimef());
		
		ImageUniform::resetTextureUnitID();
		
		for (int i = 0; i < uniforms.size(); i++)
			uniforms.getUniform(i)->update(&shader);
		
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
		
		ofPopStyle();
		
		current_framebuffer->end();
	}

#pragma mark -
	
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
		
		if (!parse(header_directive)) return false;
		
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
			
			ImageUniform *uniform = new ImageUniform(buf.name);
			uniform->set(&fbo.getTextureReference());
			uniforms.addUniform(buf.name, Uniform::Ref(uniform));
		}
		
		{
			string result_texture_name = "";
			
			if (!passes.empty())
			{
				result_texture_name = passes.back().target;
			}
			
			if (result_texture_name == "")
				result_texture_name = "DEFAULT";
			
			result_texture = &framebuffer_map[result_texture_name].getTextureReference();
		}
		
		if (!code_generator.generate(shader_directive)) return false;
		
		shader.unload();
		if (!shader.setupShaderFromSource(GL_VERTEX_SHADER, code_generator.getVertexShader()))
		{
			cout << code_generator.getVertexShader() << endl;
			return false;
		}
		
		if (!shader.setupShaderFromSource(GL_FRAGMENT_SHADER, code_generator.getFragmentShader()))
		{
			cout << code_generator.getFragmentShader() << endl;
			return false;
		}
		
		if (!shader.linkProgram())
		{
			return false;
		}
		
		return true;
	}
	
	//
	
	bool parse(const string& header_directive)
	{
		jsonxx::Object o;
		assert(o.parse(header_directive));
		
		description = o.get<string>("DESCRIPTION", "");
		credit = o.get<string>("CREDIT", "");
		
		jsonxx::Array a;
		
		{
			categories.clear();
			a = o.get<jsonxx::Array>("CATEGORIES", jsonxx::Array());
			for (int i = 0; i < a.size(); i++)
				if (a.has<string>(i))
					categories.push_back(a.get<string>(i));
		}
		
		{
			default_image_input_name = "";
			input_uniforms.clear();
			
			a = o.get<jsonxx::Array>("INPUTS", jsonxx::Array());
			for (int i = 0; i < a.size(); i++)
			{
				jsonxx::Object o = a.get<jsonxx::Object>(i, jsonxx::Object());
				string name = o.get<string>("NAME", "");
				string type = o.get<string>("TYPE", "");
				
				Input input;
				input.name = name;
				input.type = type;
				inputs.push_back(input);
				
				if (type == "image"
					&& default_image_input_name == "")
				{
					default_image_input_name = name;
				}
				
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
					input_uniforms.addUniform(name, uniform);
				}
			}
		}
		
		{
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
		}
		
		{
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
		}
		
		return true;
	}
	
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