/*
{
	"INPUTS" : [
		{
			"NAME": "mouseX",
			"TYPE": "float",
			"DEFAULT": 0.0,
			"MIN": 0.0,
			"MAX": 1.0
		},
		{
			"NAME": "mouseY",
			"TYPE": "float",
			"DEFAULT": 0.0,
			"MIN": 0.0,
			"MAX": 1.0
		},
		{
			"NAME": "depth",
			"TYPE": "float",
			"DEFAULT": 70.0,
			"MIN": 1.0,
			"MAX": 150.0
		},
		{
			"NAME": "noiseVal",
			"TYPE": "float",
			"DEFAULT": 0.06125,
			"MIN": 0.0,
			"MAX": 1.0
		},
		{
			"NAME": "level",
			"TYPE": "float",
			"DEFAULT": 0.4,
			"MIN": 0.0,
			"MAX": 1.0
		}
	]
}
*/

vec2 mouse = vec2(mouseX, mouseY);

float Hash2d(vec2 uv){
    float f = uv.x + uv.y * 47.0;
    return fract(cos(f*3.333)*100003.9);
}

float Hash3d(vec3 uv){
    float f = uv.x + uv.y * 37.0 + uv.z * 521.0;
    return fract(cos(f*3.333)*100003.9);
}
float mixP(float f0, float f1, float a){
    return mix(f0, f1, a*a*(3.0-2.0*a));
}
const vec2 zeroOne = vec2(0.0, 1.0);
float noise2d(vec2 uv){
    vec2 fr = fract(uv.xy);
    vec2 fl = floor(uv.xy);
    float h00 = Hash2d(fl);
    float h10 = Hash2d(fl + zeroOne.yx);
    float h01 = Hash2d(fl + zeroOne);
    float h11 = Hash2d(fl + zeroOne.yy);
    return mixP(mixP(h00, h10, fr.x), mixP(h01, h11, fr.x), fr.y);
}

float noise(vec3 uv){
    vec3 fr = fract(uv.xyz);
    vec3 fl = floor(uv.xyz);
    float h000 = Hash3d(fl);
    float h100 = Hash3d(fl + zeroOne.yxx);
    float h010 = Hash3d(fl + zeroOne.xyx);
    float h110 = Hash3d(fl + zeroOne.yyx);
    float h001 = Hash3d(fl + zeroOne.xxy);
    float h101 = Hash3d(fl + zeroOne.yxy);
    float h011 = Hash3d(fl + zeroOne.xyy);
    float h111 = Hash3d(fl + zeroOne.yyy);
    return mixP(
        mixP(mixP(h000, h100, fr.x), mixP(h010, h110, fr.x), fr.y),
        mixP(mixP(h001, h101, fr.x), mixP(h011, h111, fr.x), fr.y)
        , fr.z);
}

float PI=3.14159265;

vec3 saturate(vec3 a){
    return clamp(a, 0.0, 1.0);
}
vec2 saturate(vec2 a){
    return clamp(a, 0.0, 1.0);
}
float saturate(float a){
    return clamp(a, 0.0, 1.0);
}

float Density(vec3 p){
    //float ws = 0.06125*0.125;
    //vec3 warp = vec3(noise(p*ws), noise(p*ws + 111.11), noise(p*ws + 7111.11));
    float final = noise(p * noiseVal);// + sin(TIME)*0.5-1.95 + warp.x*4.0;
    float other = noise(p * noiseVal + 1234.567);
    other -= 0.5;
    final -= 0.5;
    final = 0.1/(abs(final*final*other));
    final += 0.5;
    return final*0.0001;
}

void main() {
    vec2 uv = gl_FragCoord.xy/RENDERSIZE.xy * 2.0 - 1.0;
    vec3 camUp=vec3(0,1,0);
    vec3 camLookat=vec3(0,0.0,0);

    float mx=mouse.x/RENDERSIZE.x*PI*2.0 + TIME * 0.01;
    float my=-mouse.y/RENDERSIZE.y*10.0 + sin(TIME * 0.03)*0.2+0.2;
    vec3 camPos=vec3(cos(my)*cos(mx),sin(my),cos(my)*sin(mx))*(200.2);

    vec3 camVec=normalize(camLookat - camPos);
    vec3 sideNorm=normalize(cross(camUp, camVec));
    vec3 upNorm=cross(camVec, sideNorm);
    vec3 worldFacing=(camPos + camVec);
    vec3 worldPix = worldFacing + uv.x * sideNorm * (RENDERSIZE.x/RENDERSIZE.y) + uv.y * upNorm;
    vec3 relVec = normalize(worldPix - camPos);

    float t = 0.0;
    float inc = 0.02;
    float maxDepth = depth;
    vec3 pos = vec3(0,0,0);
    float density = 0.0;
    for (int i = 0; i < 8; i++){
        if ((t > maxDepth)) break;
        pos = camPos + relVec * t;
        float temp = Density(pos);
        inc = 1.9 + temp * 0.05;
        density += temp * inc;
        t += inc;
    }

    vec3 finalColor = vec3(1.0,0.01,0.0)* density * level;
    gl_FragColor = vec4(sqrt(clamp(finalColor, 0.0, 1.0)),1.0);
}
