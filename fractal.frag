#version 140

varying vec3 vColor;
uniform highp float[15] coeffs;
uniform highp vec2 resolution;
uniform highp vec2 offset;
uniform highp vec2 zoom;
uniform int steps;
uniform highp float seed;
uniform int lenCoeffs;

const highp float M_PI = 3.14159265358979323846;

highp float rand(highp vec2 co) {return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}
highp float rand(highp vec2 co, highp float l) {return rand(vec2(rand(co), l));}
highp float rand(highp vec2 co, highp float l, highp float t) {return rand(vec2(rand(co, l), t));}

highp float perlin(highp vec2 p, highp float dim, highp float time) {
  highp vec2 pos = floor(p * dim);
  highp vec2 posx = pos + vec2(1.0, 0.0);
  highp vec2 posy = pos + vec2(0.0, 1.0);
  highp vec2 posxy = pos + vec2(1.0);

  highp float c = rand(pos, dim, time);
  highp float cx = rand(posx, dim, time);
  highp float cy = rand(posy, dim, time);
  highp float cxy = rand(posxy, dim, time);

  highp vec2 d = fract(p * dim);
  d = -0.5 * cos(d * M_PI) + 0.5;

  highp float ccx = mix(c, cx, d.x);
  highp float cycxy = mix(cy, cxy, d.x);
  highp float center = mix(ccx, cycxy, d.y);

  return center * 2.0 - 1.0;
}

highp float perlin(highp vec2 p, highp float dim) {
  return perlin(p, dim, 0.0);
}

//vec3 rgb2hsv(vec3 c)
//{
//    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

//    float d = q.x - min(q.w, q.y);
//    float e = 1.0e-10;
//    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
//}

highp vec3 hsv2rgb(highp vec3 c)
{
    highp vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    highp vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}



highp float absolute(highp vec2 a) {
  return sqrt(a.x*a.x+a.y*a.y);
}

highp float atan2(highp float y, highp float x)
{
    bool s = (abs(x) > abs(y));
    return mix(M_PI/2.0 - atan(x,y), atan(y,x), s);
}

highp vec2 mul(highp vec2 x, highp vec2 y) {
  return vec2(x.x*y.x - x.y*y.y, x.y*y.x + x.x*y.y);
}

highp float arg(highp vec2 a) {
  return atan2(a.y, a.x);
}

highp vec2 div(highp vec2 x, highp vec2 y) {
  return vec2(x.x*y.x+y.y*x.y, y.x*x.y-x.x*y.y)/(y.x*y.x+y.y*y.y);
}

highp vec2 divExp(highp vec2 x, highp vec2 y) {
  highp float r1 = absolute(x);
  highp float t1 = arg(x);
  highp float r2 = absolute(y);
  highp float t2 = arg(y);
  r1 /= r2;
  t1 -= t2;
  return vec2(r1*cos(t1), r1*sin(t1));
}

highp vec2 mulExp(highp vec2 x, highp vec2 y) {
  highp float r1 = absolute(x);
  highp float t1 = arg(x);
  highp float r2 = absolute(y);
  highp float t2 = arg(y);
  r1 *= r2;
  t1 += t2;
  return vec2(r1*cos(t1), r1*sin(t1));
}

highp vec2 powc(highp vec2 x, highp float p) {
  highp float r = absolute(x);
  highp float t = arg(x);
  // z = r*e^(i*t)
  // z^p = r^p*(i*t*p)
  r = pow(r, p);
  t *= p;
  return vec2(r*cos(t), r*sin(t));
}

highp vec2 computeStep(highp vec2 start) {
  highp vec2 val = vec2(0., 0.);
  highp vec2 der = vec2(0., 0.);
  highp vec2 pow = vec2(1., 0.);
  highp vec2 pre = vec2(0., 0.);
  for (int ci = lenCoeffs-1; ci >= 0; ci--) {
    val += pow*coeffs[ci];
    der += pre*coeffs[ci]*float(ci);
    pre = pow;
    pow = mulExp(pow, start);
  }
  highp vec2 res = -divExp(val, der);
  if(isnan(res.x) || isinf(res.x)) {
    res.x = -0.001;
  }
  if(isnan(res.y) || isinf(res.y)) {
    res.y = -0.001;
  }
  return res;
}

void main(void)
{
  highp vec2 xy = (vec2(float(gl_FragCoord.x), float(gl_FragCoord.y))/resolution-vec2(0.5, 0.5))/zoom+offset;
  for(int i = 0; i < steps; ++i) {
    xy += computeStep(xy);
  }
  gl_FragColor.rgb = hsv2rgb(vec3(perlin(normalize(vec2(0., xy.x)), seed), 1., 1.));
}
