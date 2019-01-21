/*{
 "CREDIT": "by tado",
 "CATEGORIES" : [
 "flame"
 ],
 "INPUTS" : [
 {
 "NAME": "inputImage",
 "TYPE": "image"
 },
 {
 "NAME": "rippleSpeed",
 "LABEL": "Ripple Speed",
 "TYPE": "float",
 "MIN": 0.0,
 "MAX": 3.0,
 "DEFAULT": 1.0
 },
 {
 "NAME": "rippleSize",
 "LABEL": "Ripple Size",
 "TYPE": "float",
 "MIN": 0.0,
 "MAX": 100.0,
 "DEFAULT": 50.0
 }
 ],
 }
 */


// good old interference modded. By @dennishjorth
#ifdef GL_ES
precision mediump float;
#endif


float rings(vec2 pp, float time2)
{
    vec2 p = pp;
    p.x += cos(time2*rippleSpeed+pp.y+pp.x);
    p.y += sin(time2*rippleSpeed+pp.x+pp.y);
    return cos(length(p)* (100.0 - rippleSize) - time2);
}

void main() {
    vec2 pos = (gl_FragCoord.xy*2.0 -RENDERSIZE) / RENDERSIZE.y;
    vec2 pos2 = pos;
    vec2 pos3 = pos2;
    pos.x += cos(TIME/7.0) * 1.0;
    pos.y += sin(TIME/7.0) * 1.0;
    pos2.x += cos(TIME/10.0) * 1.5;
    pos2.y += sin(TIME/10.0) * 1.5;
    pos3.x += cos(TIME/15.0) * 1.7;
    pos3.y += sin(TIME/15.0) * 1.7;
    float colR = rings(pos,TIME);
    float colG = rings(pos2,TIME);
    float colB = rings(pos3,TIME);
    gl_FragColor = vec4(colR, colG, colB, 1.0);

    //float col = (colR + colG + colB) / 3.0;
    //gl_FragColor = vec4(col, col, col, 1.0);
}
