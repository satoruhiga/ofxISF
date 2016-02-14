/*{
	"CREDIT": "by mojovideotech",
	"DESCRIPTION": "",
	"CATEGORIES": [
	],
	"INPUTS": [
	 {
            "NAME": "shifth",
            "TYPE": "float",
           "DEFAULT": 0.0,
            "MIN": -3.0,
            "MAX": 3.0
        },
        {
            "NAME":"shiftv",
            "TYPE": "float",
           "DEFAULT": 0.0,
            "MIN": -2.0,
            "MAX": 2.0
        },
                {
            "NAME": "speed",
            "TYPE": "float",
           "DEFAULT": 16.0,
            "MIN": 0.1,
            "MAX": 24.0
        },
		{
            "NAME": "density",
            "TYPE": "float",
           "DEFAULT": 64.0,
            "MIN": 6.0,
            "MAX": 256.0
        },
        {
            "NAME": "sectors",
            "TYPE": "float",
           "DEFAULT": 64,
            "MIN": 1,
            "MAX": 256
        },
         {
            "NAME": "shape",
            "TYPE": "float",
           "DEFAULT": 32,
            "MIN": 16,
            "MAX": 256
        },
         {
            "NAME": "smooth",
            "TYPE": "float",
           "DEFAULT": 0.05,
            "MIN": 0.0,
            "MAX": 1.0
        },
         {
            "NAME": "vanishingpoint",
            "TYPE": "float",
           "DEFAULT": 0.0,
            "MIN": 0.0,
            "MAX": 2.0
        }
	]
}*/

// UltimateSpiral by mojovideotech

 #define PI 3.14159265359
 #define AA 2.0

float cell (float t, float w, float p) {	
	return min (step (t-w/2.0,p), 1.0 - step (t+w/2.0,p));
}

mat2 rotate (float fi) {
	float cfi = cos (fi);
	float sfi = sin (fi);	
	return mat2 (-sfi, cfi, cfi, sfi);	
}

vec4 spiral (vec2 uv, float TIME) {
	float fi = length (uv) * density;
	float g = atan (uv.y, uv.x);
	uv *= rotate (TIME*speed);
	uv *= sin (g*floor(sectors));
	uv *= rotate (fi);
	
	
	return mix (vec4 (0.99), vec4 (0.0), min (
		step (smooth, uv.x), 
		cell (vanishingpoint, fi/shape, uv.y)));
}
	

void main ( void ) {
	float unit =  1.0/min (RENDERSIZE.x, RENDERSIZE.y);
	vec2 uv = (2.0*gl_FragCoord.xy - RENDERSIZE.xy) * unit;
	
	#ifdef AA
	vec4 ac = vec4 (0.0);
	for (float y = 0.0; y < AA; ++y) {
		for (float x = 0.0; x < AA; ++x) {			
			vec2 c =  vec2 (
				(x-AA/2.0)*(unit/AA)+shifth,
				(y-AA/2.0)*(unit/AA)+shiftv);
			ac += spiral (uv + c, (TIME)) / (AA*AA);
		}
	}	
	gl_FragColor = vec4(ac.r, 0.0, 0.0, 1.0);
	#else 
	gl_FragColor = spiral (uv*1.0, cos(TIME));		
	#endif		
}