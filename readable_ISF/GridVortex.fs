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
           "DEFAULT": 0.1,
            "MIN": 0.01,
            "MAX": 0.9
        },
         {
            "NAME": "rotation",
            "TYPE": "float",
           "DEFAULT": 0.05,
            "MIN": 0.005,
            "MAX": 0.5
        },
         {
            "NAME": "density",
            "TYPE": "float",
           "DEFAULT": 0.3,
            "MIN": 0.05,
            "MAX": 0.99
        },
         {
            "NAME": "sectors",
            "TYPE": "float",
           "DEFAULT": 211,
            "MIN": 1,
            "MAX": 221
        },
         {
            "NAME": "offsetx",
            "TYPE": "float",
           "DEFAULT": 0.1,
            "MIN": 0.1,
            "MAX": 0.9
        },
         {
            "NAME": "offsetzx",
            "TYPE": "float",
           "DEFAULT": 0.05,
            "MIN": 0.005,
            "MAX": 0.05
        },
         {
            "NAME": "offsety",
            "TYPE": "float",
           "DEFAULT": 0.1,
            "MIN": 0.1,
            "MAX": 0.9
        },
         {
            "NAME": "offsetzy",
            "TYPE": "float",
           "DEFAULT": 0.05,
            "MIN": 0.005,
            "MAX": 0.05
        },
         {
            "NAME": "r",
            "TYPE": "float",
           "DEFAULT": 0.6,
            "MIN": 0.0,
            "MAX": 0.9
        },
         {
            "NAME": "g",
            "TYPE": "float",
           "DEFAULT": 0.4,
            "MIN": 0.0,
            "MAX": 0.9
        },
         {
            "NAME": "b",
            "TYPE": "float",
           "DEFAULT": 0.1,
            "MIN": 0.0,
            "MAX": 0.9
        }
  ]
}
*/

void main(void) {
	vec2 p = gl_FragCoord.xy/RENDERSIZE.xx*2.-vec2(1.0+shifth,0.5+shiftv);
	gl_FragColor = length(p)*vec4(vec3((mod(density/length(p)+TIME*speed,offsetx)>offsetzx)^^(mod(atan(p.x/p.y)*7./floor(222.-sectors)+TIME*(0.0-rotation),offsety)>offsetzy)),3.)*0.5;
	gl_FragColor *= vec4(0.1+r, 0.1+g, 0.1+b, 1.0);
    gl_FragColor /= vec4(1.0-r, 1.0-g, 1.0-b, 1.0);
}