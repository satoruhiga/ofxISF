/*{
 "CREDIT": "by tado",
  "CATEGORIES" : [
    "plasma"
  ],
  "INPUTS" : [
    {
        "NAME": "inputImage",
        "TYPE": "image"
    },
	{
    	"NAME": "waveFreq",
		"TYPE": "float",
		"MIN": 0.0,
		"MAX": 10.0,
		"DEFAULT": 4.0
    },
    {
    	"NAME": "waveResolution",
		"TYPE": "float",
		"MIN": 1.0,
		"MAX": 1000.0,
		"DEFAULT": 800.0
    },
    {
    	"NAME": "weveDepth",
		"TYPE": "float",
		"MIN": 0.0,
		"MAX": 1.0,
		"DEFAULT": 0.5
    }
    ,
    {
    	"NAME": "timeScale",
		"TYPE": "float",
		"MIN": 0.1,
		"MAX": 3.0,
		"DEFAULT": 1.0
    }
    ,
    {
    	"NAME": "colorShift",
		"TYPE": "float",
		"MIN": 0.0,
		"MAX": 1.0,
		"DEFAULT": 0.0
    }
  ]
}
*/


void main()
{
	vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy;
	float time = TIME;
	float depth = sin(uv.y*2.0+sin(time*timeScale)*1.5+1.0+sin(uv.x*waveFreq*1.5+time*1.2*timeScale))*cos(uv.y*waveFreq+time*timeScale)+sin((uv.x*waveFreq*1.5+time));
	depth = depth * weveDepth + (1.0 - weveDepth);
	float texey = (uv.x-0.5);
	float xband = sin(sqrt(uv.y/uv.y)*5.0/(depth)+time*1.0);
	float final = (
		sin(texey/abs(depth)*(1000.0 - waveResolution)+time*16.0)*(depth)*xband
	);

	gl_FragColor = vec4(-final*colorShift, final * (1.0 - colorShift), final*colorShift-final*(1.0 - colorShift), 1.0)*1.5;
}
