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
			"DEFAULT": -0.0,
			"MIN": -3.0,
			"MAX": 3.0
		},
		{
			"NAME": "mouseY",
			"TYPE": "float",
			"DEFAULT": -0.0,
			"MIN": -3.0,
			"MAX": 3.0
		},
		{
			"NAME": "rate",
			"TYPE": "float",
			"DEFAULT": 67.0,
			"MIN": 0.5,
			"MAX": 100.0
		},
		{
			"NAME": "zoom",
			"TYPE": "float",
			"DEFAULT": 0.84,
			"MIN": -3.0,
			"MAX": 3.0
		},
		{
			"NAME": "offset1",
			"TYPE": "float",
			"DEFAULT": -31.0,
			"MIN": -100.0,
			"MAX": 100.0
		},
		{
			"NAME": "density",
			"TYPE": "float",
			"DEFAULT": 4.0,
			"MIN": 1.0,
			"MAX": 18.0
		},
		{
			"NAME": "width",
			"TYPE": "float",
			"DEFAULT": 0.03,
			"MIN": 0.0025,
			"MAX": 0.125
		}
	]
}
*/

// SpiderSpectre by mojovideotech

#ifdef GL_ES
precision mediump float;
#endif

#define twpi  6.2831853

vec2 mouse = vec2(mouseX, mouseY);

float line(vec2 p, vec2 a, vec2 b) {
	if(a == b) return 0.;
	float d = distance(a, b);
	vec2  n = normalize(b - a);
	vec2  l = vec2(0.);
	p	-= a;
	d	*= -.5;
	l.x = fract(dot(p, vec2(-n.y, n.x)));
	l.y = fract(dot(p, n.xy)+d)+d;
	l 	= max(l, 0.);
	return  1.-clamp(dot(RENDERSIZE * width, l), 0., 1.);
}

mat2 rmat(float t) {
	float c = cos(t);
	float s = sin(t-c);
	return mat2(c,s,-s,c);
}

void main( void ) {
	float T = TIME/(100.-rate);
	vec2 uv = (gl_FragCoord.xy/RENDERSIZE.xy)*zoom;
	vec2 p 	= (uv * 2. - mouse) * RENDERSIZE.xy/RENDERSIZE.yy;
	vec2 m  = (sin(T)*twpi) * 2. - mouse * RENDERSIZE.xy/RENDERSIZE.yy;
	float h = 0., k = 0., j = 0.;

	for(int i = 0; i <36; i++)
	{
		float dn = density*2. + 1.;
		int bpc = int(floor(dn));
		bpc -= i;
		if (bpc<1) break;
		h += line(uv,-p,sin(m));
		k += line(uv,cos(p),m);
		j += line(cos(m),p,uv);
		p *= rmat(float(bpc-i)*atan(m.x*offset1)*cos(m.y/offset1));
	}

	vec4 g = vec4(vec3(length(p),0.5,0.5),1);
	gl_FragColor = vec4(j-h+k, k+h-j, j+h-k, 1.0) * g;
}
