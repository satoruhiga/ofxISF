/*
{
	"INPUTS" : [
		{
			"NAME": "zoom",
			"TYPE": "float",
			"DEFAULT": 0.3,
			"MIN": 0.0,
			"MAX": 1.0

		},
		{
			"NAME": "scale",
			"TYPE": "float",
			"DEFAULT": 0.6,
			"MIN": 0.0,
			"MAX": 1.0

		},
		{
			"NAME": "rotSpeed",
			"TYPE": "float",
			"DEFAULT": 0.05,
			"MIN": 0.0,
			"MAX": 0.2
		}
	]
}
*/

vec3 roty(vec3 p,float a)
{ return mat3(cos(a),0,-sin(a), 0,1,0, sin(a),0,cos(a)) * p; }

float map(in vec3 p)
{
	vec3 c=p; float res=0.;
	for (int i=0; i < 4; i++) {
		p= abs(p)/dot(p,p) - zoom;
		p.yz= vec2(p.y*p.y-p.z*p.z,2.*p.y*p.z);
		res += exp(-10. * abs(dot(p,c)));
	}
	return res * 0.5;
}

vec3 raymarch(vec3 ro, vec3 rd)
{
	float t=5.0;
	vec3 col=vec3(0); float c=0.;
	for( int i=0; i < 32; i++ )	{
		t+= exp(c* -2.0) *0.01;
		c= map(t * rd + ro);
		col= vec3(c*c, c, c*c*c) *0.16 + col *0.96; //green
		col= vec3(c*c*c, c*c, c) *0.16 + col *0.96; //blue
		col= vec3(c, c*c*c, c*c) *0.16 + col *0.96; //red

	}
	return col;
}

void main()
{
    vec2 p = (gl_FragCoord.xy - RENDERSIZE * 0.5) / RENDERSIZE.y;
    vec3 ro= roty(vec3(3.0), TIME * rotSpeed);
    vec3 uu= normalize(cross(ro, vec3(0.0, 1.0, 0.0)));
    vec3 vv= normalize(cross(uu, ro));
    vec3 rd= normalize(p.x*uu + p.y*vv - ro * scale);
    //gl_FragColor.rgb= log(raymarch(ro,rd) +1.0) * 0.5;
    vec3 col = log(raymarch(ro,rd) +1.0) * 0.5;
    //gl_FragColor.a= 1.0;
    gl_FragColor = vec4(col.r * 1.0, col.g * 0.0, col.g * 0.0, 1.0);
}
