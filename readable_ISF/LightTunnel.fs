/*{
	"CREDIT": "by mojovideotech",
	"DESCRIPTION": "",
	"CATEGORIES": [
	],
	"INPUTS": [
		{
            "NAME": "shifth",
            "TYPE": "float",
           "DEFAULT": 0.3,
            "MIN": -1.0,
            "MAX": 1.0
        },
         {
            "NAME":"shiftv",
            "TYPE": "float",
           "DEFAULT": 0.3,
            "MIN": -1.0,
            "MAX": 1.0
        },
		{
			"NAME": "speed",
			"TYPE": "float",
			"DEFAULT": 0.5,
			"MIN": 0.0,
			"MAX": 5.0
		},
		{
			"NAME": "lightBrightness",
			"TYPE": "float",
			"DEFAULT": 0.01,
			"MIN": 0.0,
			"MAX": 0.04
		},
		{
			"NAME": "depth",
			"TYPE": "float",
			"DEFAULT": 0.05,
			"MIN": 0.0,
			"MAX": 0.3
		}
	]
}*/


#ifdef GL_ES
precision mediump float;
#endif


void main( void ) {
	vec3 light_color = vec3(1.2,0.4,0.3);

	float t = TIME*20.0;
	//vec2 position = ( gl_FragCoord.xy -  RENDERSIZE.xy*.5) / RENDERSIZE.x;
	vec2 position = gl_FragCoord.xy/RENDERSIZE.xx*2.-vec2(1.0 + shifth, 0.5 + shiftv);

	float angle = atan(position.y,position.x)/(2.*3.14159265359);
	angle -= floor(angle);
	float rad = length(position);

	float color = 0.0;
	float brightness = lightBrightness;

	for (int i = 0; i < 10; i++) {
		float angleRnd = floor(angle*14.)+1.;
		float angleRnd1 = fract(angleRnd*fract(angleRnd*.7235)*45.1);
		float angleRnd2 = fract(angleRnd*fract(angleRnd*.82657)*13.724);
		float t = t*speed + angleRnd1*100.;
		float radDist = sqrt(angleRnd2+float(i));

		float adist = radDist/rad * depth;
		float dist = (t*.1 + adist);
		dist = abs(fract(dist)-.5);
		color +=  (1.0 / (dist)) * cos(0.7 * (sin(t))) * adist/radDist * brightness;
		angle = fract(angle+.1);
	}
	gl_FragColor = vec4(color,color,color,1.0)*vec4(light_color,1.0);
}
