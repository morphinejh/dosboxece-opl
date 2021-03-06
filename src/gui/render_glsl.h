/*
 *  Copyright (C) 2002-2021  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#if C_OPENGL

static const char advinterp2x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform sampler2D rubyTexture;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
""
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)*rubyInputSize;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
""
"vec3 getadvinterp2xtexel(vec2 coord) {\n"
"	vec2 base = floor(coord/vec2(2.0))+vec2(0.5);\n"
"	vec3 c4 = texture2D(rubyTexture, base/rubyTextureSize).xyz;\n"
"	vec3 c1 = texture2D(rubyTexture, (base-vec2(0.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c7 = texture2D(rubyTexture, (base+vec2(0.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c3 = texture2D(rubyTexture, (base-vec2(1.0,0.0))/rubyTextureSize).xyz;\n"
"	vec3 c5 = texture2D(rubyTexture, (base+vec2(1.0,0.0))/rubyTextureSize).xyz;\n"
""
"	bool outer = c1 != c7 && c3 != c5;\n"
"	bool c3c1 = outer && c3==c1;\n"
"	bool c1c5 = outer && c1==c5;\n"
"	bool c3c7 = outer && c3==c7;\n"
"	bool c7c5 = outer && c7==c5;\n"
""
"	vec3 l00 = mix(c3,c4,c3c1?3.0/8.0:1.0);\n"
"	vec3 l01 = mix(c5,c4,c1c5?3.0/8.0:1.0);\n"
"	vec3 l10 = mix(c3,c4,c3c7?3.0/8.0:1.0);\n"
"	vec3 l11 = mix(c5,c4,c7c5?3.0/8.0:1.0);\n"
""
"	coord = max(floor(mod(coord, 2.0)), 0.0);\n"
"	/* 2x2 output:\n"
"	 *    |x=0|x=1\n"
"	 * y=0|l00|l01\n"
"	 * y=1|l10|l11\n"
"	 */\n"
""
"	return mix(mix(l00,l01,coord.x), mix(l10,l11,coord.x), coord.y);\n"
"}\n"
""
"void main()\n"
"{\n"
"	vec2 coord = v_texCoord;\n"
"#if defined(OPENGLNB)\n"
"	gl_FragColor = getadvinterp2xtexel(coord);\n"
"#else\n"
"	coord -= 0.5;\n"
"	vec3 c0 = getadvinterp2xtexel(coord);\n"
"	vec3 c1 = getadvinterp2xtexel(coord+vec2(1.0,0.0));\n"
"	vec3 c2 = getadvinterp2xtexel(coord+vec2(0.0,1.0));\n"
"	vec3 c3 = getadvinterp2xtexel(coord+vec2(1.0));\n"
""
"	coord = fract(max(coord,0.0));\n"
"	gl_FragColor = vec4(mix(mix(c0,c1,coord.x), mix(c2,c3,coord.x), coord.y), 1.0);\n"
"#endif\n"
"}\n"
"#endif";

static const char advinterp3x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform sampler2D rubyTexture;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
""
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize*3.0;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
""
"vec3 getadvinterp3xtexel(vec2 coord) {\n"
"	vec2 base = floor(coord/vec2(3.0))+vec2(0.5);\n"
"	vec3 c0 = texture2D(rubyTexture, (base-vec2(1.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c1 = texture2D(rubyTexture, (base-vec2(0.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c2 = texture2D(rubyTexture, (base-vec2(-1.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c3 = texture2D(rubyTexture, (base-vec2(1.0,0.0))/rubyTextureSize).xyz;\n"
"	vec3 c4 = texture2D(rubyTexture, base/rubyTextureSize).xyz;\n"
"	vec3 c5 = texture2D(rubyTexture, (base+vec2(1.0,0.0))/rubyTextureSize).xyz;\n"
"	vec3 c6 = texture2D(rubyTexture, (base+vec2(-1.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c7 = texture2D(rubyTexture, (base+vec2(0.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c8 = texture2D(rubyTexture, (base+vec2(1.0,1.0))/rubyTextureSize).xyz;\n"
""
"	bool outer = c1 != c7 && c3 != c5;\n"
""
"	vec3 l00 = mix(c3,c4,(outer && c3==c1) ? 3.0/8.0:1.0);\n"
"	vec3 l01 = (outer && ((c3==c1&&c4!=c2)||(c5==c1&&c4!=c0))) ? c1 : c4;\n"
"	vec3 l02 = mix(c5,c4,(outer && c5==c1) ? 3.0/8.0:1.0);\n"
"	vec3 l10 = (outer && ((c3==c1&&c4!=c6)||(c3==c7&&c4!=c0))) ? c3 : c4;\n"
"	vec3 l11 = c4;\n"
"	vec3 l12 = (outer && ((c5==c1&&c4!=c8)||(c5==c7&&c4!=c2))) ? c5 : c4;\n"
"	vec3 l20 = mix(c3,c4,(outer && c3==c7) ? 3.0/8.0:1.0);\n"
"	vec3 l21 = (outer && ((c3==c7&&c4!=c8)||(c5==c7&&c4!=c6))) ? c7 : c4;\n"
"	vec3 l22 = mix(c5,c4,(outer && c5==c7) ? 3.0/8.0:1.0);\n"
""
"	coord = mod(coord, 3.0);\n"
"	bvec2 l = lessThan(coord, vec2(1.0));\n"
"	bvec2 h = greaterThanEqual(coord, vec2(2.0));\n"
""
"	if (h.x) {\n"
"		l01 = l02;\n"
"		l11 = l12;\n"
"		l21 = l22;\n"
"	}\n"
"	if (h.y) {\n"
"		l10 = l20;\n"
"		l11 = l21;\n"
"	}\n"
"	if (l.x) {\n"
"		l01 = l00;\n"
"		l11 = l10;\n"
"	}\n"
"	return l.y ? l01 : l11;\n"
"}\n"
""
"void main()\n"
"{\n"
"	vec2 coord = v_texCoord;\n"
"#if defined(OPENGLNB)\n"
"	gl_FragColor = getadvinterp3xtexel(coord);\n"
"#else\n"
"	coord -= 0.5;\n"
"	vec3 c0 = getadvinterp3xtexel(coord);\n"
"	vec3 c1 = getadvinterp3xtexel(coord+vec2(1.0,0.0));\n"
"	vec3 c2 = getadvinterp3xtexel(coord+vec2(0.0,1.0));\n"
"	vec3 c3 = getadvinterp3xtexel(coord+vec2(1.0));\n"
""
"	coord = fract(max(coord,0.0));\n"
"	gl_FragColor = vec4(mix(mix(c0,c1,coord.x), mix(c2,c3,coord.x), coord.y), 1.0);\n"
"#endif\n"
"}\n"
"#endif";

static const char advmame2x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform sampler2D rubyTexture;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
""
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)*rubyInputSize;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
""
"vec3 getadvmame2xtexel(vec2 coord) {\n"
"	vec2 base = floor(coord/vec2(2.0))+vec2(0.5);\n"
"	vec3 c4 = texture2D(rubyTexture, base/rubyTextureSize).xyz;\n"
"	vec3 c1 = texture2D(rubyTexture, (base-vec2(0.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c7 = texture2D(rubyTexture, (base+vec2(0.0,1.0))/rubyTextureSize).xyz;\n"
"	vec3 c3 = texture2D(rubyTexture, (base-vec2(1.0,0.0))/rubyTextureSize).xyz;\n"
"	vec3 c5 = texture2D(rubyTexture, (base+vec2(1.0,0.0))/rubyTextureSize).xyz;\n"
""
"	bool outer = c1 != c7 && c3 != c5;\n"
"	bool c3c1 = outer && c3==c1;\n"
"	bool c1c5 = outer && c1==c5;\n"
"	bool c3c7 = outer && c3==c7;\n"
"	bool c7c5 = outer && c7==c5;\n"
""
"	vec3 l00 = mix(c4,c3,c3c1?1.0:0.0);\n"
"	vec3 l01 = mix(c4,c5,c1c5?1.0:0.0);\n"
"	vec3 l10 = mix(c4,c3,c3c7?1.0:0.0);\n"
"	vec3 l11 = mix(c4,c5,c7c5?1.0:0.0);\n"
""
"	coord = max(floor(mod(coord, 2.0)), 0.0);\n"
"	/* 2x2 output:\n"
"	 *    |x=0|x=1\n"
"	 * y=0|l00|l01\n"
"	 * y=1|l10|l11\n"
"	 */\n"
""
"	return mix(mix(l00,l01,coord.x), mix(l10,l11,coord.x), coord.y);\n"
"}\n"
""
"void main()\n"
"{\n"
"	vec2 coord = v_texCoord;\n"
"#if defined(OPENGLNB)\n"
"	gl_FragColor = getadvmame2xtexel(coord);\n"
"#else\n"
"	coord -= 0.5;\n"
"	vec3 c0 = getadvmame2xtexel(coord);\n"
"	vec3 c1 = getadvmame2xtexel(coord+vec2(1.0,0.0));\n"
"	vec3 c2 = getadvmame2xtexel(coord+vec2(0.0,1.0));\n"
"	vec3 c3 = getadvmame2xtexel(coord+vec2(1.0));\n"
""
"	coord = fract(max(coord,0.0));\n"
"	gl_FragColor = vec4(mix(mix(c0,c1,coord.x), mix(c2,c3,coord.x), coord.y), 1.0);\n"
"#endif\n"
"}\n"
"#endif";

static const char advmame3x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform sampler2D rubyTexture;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
""
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize*3.0;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
""
"vec3 getadvmame3xtexel(vec2 coord) {\n"
"	vec2 base = floor(coord/vec2(3.0))+vec2(0.5);\n"
"	coord = mod(coord, 3.0);\n"
"	bvec2 l = lessThan(coord, vec2(1.0));\n"
"	bvec2 h = greaterThanEqual(coord, vec2(2.0));\n"
"	bvec2 m = equal(l,h);\n"
""
"	vec2 left = vec2(h.x?1.0:-1.0, 0.0);\n"
"	vec2 up = vec2(0.0, h.y?1.0:-1.0);\n"
"	if (l==h) left.x = 0.0; // hack for center pixel, will ensure outer==false\n"
"	if (m.y) {\n"
"		// swap\n"
"		left -= up;\n"
"		up += left;\n"
"		left = up - left;\n"
"	}\n"
""
"	vec3 c0 = texture2D(rubyTexture, (base+up+left)/rubyTextureSize).xyz;\n"
"	vec3 c1 = texture2D(rubyTexture, (base+up)/rubyTextureSize).xyz;\n"
"	vec3 c2 = texture2D(rubyTexture, (base+up-left)/rubyTextureSize).xyz;\n"
"	vec3 c3 = texture2D(rubyTexture, (base+left)/rubyTextureSize).xyz;\n"
"	vec3 c4 = texture2D(rubyTexture, base/rubyTextureSize).xyz;\n"
"	vec3 c5 = texture2D(rubyTexture, (base-left)/rubyTextureSize).xyz;\n"
"	vec3 c7 = texture2D(rubyTexture, (base-up)/rubyTextureSize).xyz;\n"
""
"	bool outer = c1 != c7 && c3 != c5;\n"
"	bool check1 = c3==c1 && (!any(m) || c4!=c2);\n"
"	bool check2 = any(m) && c5==c1 && c4!=c0;\n"
""
"	return (outer && (check1 || check2)) ? c1 : c4;\n"
"}\n"
""
"void main()\n"
"{\n"
"	vec2 coord = v_texCoord;\n"
"#if defined(OPENGLNB)\n"
"	gl_FragColor = getadvmame3xtexel(coord);\n"
"#else\n"
"	coord -= 0.5;\n"
"	vec3 c0 = getadvmame3xtexel(coord);\n"
"	vec3 c1 = getadvmame3xtexel(coord+vec2(1.0,0.0));\n"
"	vec3 c2 = getadvmame3xtexel(coord+vec2(0.0,1.0));\n"
"	vec3 c3 = getadvmame3xtexel(coord+vec2(1.0));\n"
""
"	coord = fract(max(coord,0.0));\n"
"	gl_FragColor = vec4(mix(mix(c0,c1,coord.x), mix(c2,c3,coord.x), coord.y), 1.0);\n"
"#endif\n"
"}\n"
"#endif";

static const char rgb2x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform sampler2D rubyTexture;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
""
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize * 2.0;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
""
"vec4 getRGB2xtexel(vec2 coord) {\n"
"	vec2 mid = vec2(0.5);\n"
"	vec4 s = texture2D(rubyTexture, (floor(coord/vec2(2.0))+mid)/rubyTextureSize);\n"
""
"	coord = max(floor(mod(coord, 2.0)), 0.0);\n"
"	/* 2x2 output:\n"
"	 *    |x=0|x=1\n"
"	 * y=0| r | g\n"
"	 * y=1| b |rgb\n"
"	 */\n"
""
"	s.r *= 1.0 - abs(coord.x - coord.y);\n"
"	s.g *= coord.x;\n"
"	s.b *= coord.y;\n"
"	return s;\n"
"}\n"
""
"void main()\n"
"{\n"
"	vec2 coord = v_texCoord;\n"
"#if defined(OPENGLNB)\n"
"	gl_FragColor = getRGB2xtexel(coord);\n"
"#else\n"
"	coord -= 0.5;\n"
"	vec4 c0 = getRGB2xtexel(coord);\n"
"	vec4 c1 = getRGB2xtexel(coord+vec2(1.0,0.0));\n"
"	vec4 c2 = getRGB2xtexel(coord+vec2(0.0,1.0));\n"
"	vec4 c3 = getRGB2xtexel(coord+vec2(1.0));\n"
""
"	coord = fract(max(coord,0.0));\n"
"	gl_FragColor = mix(mix(c0,c1,coord.x), mix(c2,c3,coord.x), coord.y);\n"
"#endif\n"
"}\n"
"#endif";

static const char rgb3x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform sampler2D rubyTexture;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
""
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize * 3.0;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
""
"vec4 getRGB3xtexel(vec2 coord) {\n"
"	vec2 mid = vec2(0.5);\n"
"	vec4 s = texture2D(rubyTexture, (floor(coord/vec2(3.0))+mid)/rubyTextureSize);\n"
""
"	coord = max(floor(mod(coord, 3.0)), 0.0);\n"
"	/* 3x3 output:\n"
"	 *  | l | m | h\n"
"	 * l|rgb| g | b\n"
"	 * m| g | r |rgb\n"
"	 * h|rgb| b | r\n"
"	 */\n"
"	vec2 l = step(0.0, -coord);\n"
"	vec2 m = vec2(1.0) - abs(coord-1.0);\n"
"	vec2 h = step(2.0, coord);\n"
""
"	s.r *= l.x + m.y - 2.0*l.x*m.y + h.x*h.y;\n"
"	s.g *= l.x + l.y*m.x + h.x*m.y;\n"
"	s.b *= l.x*l.y + h.x + h.y - 2.0*h.x*h.y;\n"
"	return s;\n"
"}\n"
""
"void main()\n"
"{\n"
"	vec2 coord = v_texCoord;\n"
"#if defined(OPENGLNB)\n"
"	gl_FragColor = getRGB3xtexel(coord);\n"
"#else\n"
"	coord -= 0.5;\n"
"	vec4 c0 = getRGB3xtexel(coord);\n"
"	vec4 c1 = getRGB3xtexel(coord+vec2(1.0,0.0));\n"
"	vec4 c2 = getRGB3xtexel(coord+vec2(0.0,1.0));\n"
"	vec4 c3 = getRGB3xtexel(coord+vec2(1.0));\n"
""
"	coord = fract(max(coord,0.0));\n"
"	gl_FragColor = mix(mix(c0,c1,coord.x), mix(c2,c3,coord.x), coord.y);\n"
"#endif\n"
"}\n"
"#endif";

static const char scan2x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
"uniform sampler2D rubyTexture;\n"
""
"void main() {\n"
"  vec2 prescale = vec2(2.0);\n"
"  vec2 texel = v_texCoord;\n"
"  vec2 texel_floored = floor(texel);\n"
"  vec2 s = fract(texel);\n"
"  vec2 region_range = vec2(0.5) - vec2(0.5) / prescale;\n"
""
"  vec2 center_dist = s - vec2(0.5);\n"
"  vec2 f = (center_dist - clamp(center_dist, -region_range, region_range)) * prescale + vec2(0.5);\n"
""
"  vec2 mod_texel = min(texel_floored + f, rubyInputSize-0.5);\n"
"  vec4 p = texture2D(rubyTexture, mod_texel/rubyTextureSize);\n"
"  float ss = abs(s.y*2.0-1.0);\n"
"  p -= p*ss;\n"
""
"  gl_FragColor = p;\n"
"}\n"
"#endif";

static const char scan3x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
"uniform sampler2D rubyTexture;\n"
""
"void main() {\n"
"  vec2 prescale = vec2(3.0);\n"
"  vec2 texel = v_texCoord;\n"
"  vec2 texel_floored = floor(texel);\n"
"  vec2 s = fract(texel);\n"
"  vec2 region_range = vec2(0.5) - vec2(0.5) / prescale;\n"
""
"  vec2 center_dist = s - 0.5;\n"
"  vec2 f = (center_dist - clamp(center_dist, -region_range, region_range)) * prescale + vec2(0.5);\n"
""
"  vec2 mod_texel = min(texel_floored + f, rubyInputSize-0.5);\n"
"  vec4 p = texture2D(rubyTexture, mod_texel/rubyTextureSize);\n"
"  float m = s.y*6.0;\n"
"  m -= clamp(m, 2.0, 4.0);\n"
"  m = abs(m/2.0);\n"
"  gl_FragColor = p - p*m;\n"
"}\n"
"#endif";

static const char tv2x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
"uniform sampler2D rubyTexture;\n"
""
"void main() {\n"
"  vec2 prescale = vec2(2.0);\n"
"  vec2 texel = v_texCoord;\n"
"  vec2 texel_floored = floor(texel);\n"
"  vec2 s = fract(texel);\n"
"  vec2 region_range = vec2(0.5) - vec2(0.5) / prescale;\n"
""
"  vec2 center_dist = s - 0.5;\n"
"  vec2 f = (center_dist - clamp(center_dist, -region_range, region_range)) * prescale + vec2(0.5);\n"
""
"  vec2 mod_texel = min(texel_floored + f, rubyInputSize-0.5);\n"
"  vec4 p = texture2D(rubyTexture, mod_texel/rubyTextureSize);\n"
"  float ss = abs(s.y*2.0-1.0);\n"
"  p -= p*ss*3.0/8.0;\n"
""
"  gl_FragColor = p;\n"
"}\n"
"#endif";

static const char tv3x_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize;\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
"uniform sampler2D rubyTexture;\n"
""
"void main() {\n"
"  vec2 prescale = vec2(3.0);\n"
"  vec2 texel = v_texCoord;\n"
"  vec2 texel_floored = floor(texel);\n"
"  vec2 s = fract(texel);\n"
"  vec2 region_range = vec2(0.5) - vec2(0.5) / prescale;\n"
""
"  vec2 center_dist = s - 0.5;\n"
"  vec2 f = (center_dist - clamp(center_dist, -region_range, region_range)) * prescale + vec2(0.5);\n"
""
"  vec2 mod_texel = min(texel_floored + f, rubyInputSize-0.5);\n"
"  vec4 p = texture2D(rubyTexture, mod_texel/rubyTextureSize);\n"
"  float ss = abs(s.y*2.0-1.0);\n"
"  p -= p*ss*11.0/16.0;\n"
""
"  gl_FragColor = p;\n"
"}\n"
"#endif";

static const char sharp_glsl[] =
"varying vec2 v_texCoord;\n"
"uniform vec2 rubyInputSize;\n"
"uniform vec2 rubyOutputSize;\n"
"uniform vec2 rubyTextureSize;\n"
"varying vec2 prescale; // const set by vertex shader\n"
""
"#if defined(VERTEX)\n"
"attribute vec4 a_position;\n"
"void main() {\n"
"  gl_Position = a_position;\n"
"  v_texCoord = vec2(a_position.x+1.0,1.0-a_position.y)/2.0*rubyInputSize;\n"
"  prescale = ceil(rubyOutputSize / rubyInputSize);\n"
"}\n"
""
"#elif defined(FRAGMENT)\n"
"uniform sampler2D rubyTexture;\n"
""
"void main() {\n"
"  const vec2 halfp = vec2(0.5);\n"
"  vec2 texel_floored = floor(v_texCoord);\n"
"  vec2 s = fract(v_texCoord);\n"
"  vec2 region_range = halfp - halfp / prescale;\n"
""
"  vec2 center_dist = s - halfp;\n"
"  vec2 f = (center_dist - clamp(center_dist, -region_range, region_range)) * prescale + halfp;\n"
""
"  vec2 mod_texel = min(texel_floored + f, rubyInputSize-halfp);\n"
"  gl_FragColor = texture2D(rubyTexture, mod_texel / rubyTextureSize);\n"
"}\n"
"#endif";


#endif

