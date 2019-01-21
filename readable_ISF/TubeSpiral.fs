/*{
	"CREDIT": "tado",

	"CATEGORIES": [
		""
	],
	"DESCRIPTION": "",
	"INPUTS": [
		{
			"NAME": "mouseX",
			"TYPE": "float",
			"DEFAULT": 0.04,
			"MIN": 0.0,
			"MAX": 1.0
		},
		{
			"NAME": "mouseY",
			"TYPE": "float",
			"DEFAULT": 0.9,
			"MIN": 0.4,
			"MAX": 3.0
		}
		,
		{
			"NAME": "rotSpeed",
			"TYPE": "float",
			"DEFAULT": 1.0,
			"MIN": 0.0,
			"MAX": 5.0
		}
	]
}
*/


#ifdef GL_ES
precision mediump float;
#endif
 

#define PI 3.1415927
#define PI2 (PI*2.0)

vec2 mouse = vec2(mouseX, mouseY);

void main(void)
{
	vec2 position = mouse.y * 64.0 * ((2.0 * gl_FragCoord.xy - RENDERSIZE) / RENDERSIZE.xx);

	float r = length(position);
	float a = atan(position.x, position.y) + PI;
	float d = r - a + PI2;
	float n = PI2 * float(int((d / PI2)));
	float da = a + n;
	float pos = (PI * 0.02 * mouse.x * 4.0 * da * da - TIME * rotSpeed);
	vec3 norm;
	norm.xy = vec2(fract(pos) - .5, fract(d / PI2) - .5)*2.0;
	pos = floor(pos);
	float len = length(norm.xy);
	vec3 color = vec3(0.0);
	if (len < 1.0)
	{
		norm.z = sqrt(1.0 - len*len);
		vec3 lightdir = normalize(vec3(0.0, 1.0, 1.0));
		float dd = dot(lightdir, norm);
		dd = max(dd, 0.25);
		color = dd*vec3(0.5*cos(pos+2.0)+0.5, 0.5*cos(pos+4.0)+0.5, 0.5*cos(pos)+0.5);
		vec3 halfv = normalize(lightdir + vec3(0.0, 0.0, 1.0));
		float spec = dot(halfv, norm);
		spec = max(spec, 0.0);
		spec = pow(spec, 100.0);
		color += vec3(spec);
	}
	
	gl_FragColor.rgba = vec4(color,1.0);
}