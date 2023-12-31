#version 430 core
#pragma optionNV (unroll all)	
#define LOCAL_GROUP_SIZE 32
#extension GL_ARB_compute_shader: enable
#extension GL_ARB_shader_image_load_store: enable
// This is set at the limit of providing unnatural results for sharpening.
#define FSR_RCAS_LIMIT (0.25 - (1.0 / 16.0))
layout (local_size_x = LOCAL_GROUP_SIZE, local_size_y = LOCAL_GROUP_SIZE) in;
layout (rgba32f, binding = 3) uniform writeonly image2D u_OutputRCASImage;

uniform sampler2D u_RASUTexture;
uniform vec4 u_Con0;
vec4 fsrRcasLoadF(ivec2 p) { return texelFetch(u_RASUTexture, ivec2(p), 0); }
float max3F1(float x,float y,float z){return max(x,max(y,z));}
float min3F1(float x,float y,float z){return min(x,min(y,z));}

void fsrRcasF(out float pixR,out float pixG,out float pixB,ivec2 ip) { // Constant generated by RcasSetup().
	  // Algorithm uses minimal 3x3 pixel neighborhood.
	  //    b 
	  //  d e f
	  //    h
	 ivec2 P = ivec2(ip);
	 vec3 b = fsrRcasLoadF(P + ivec2(0, -1)).rgb;
	 vec3 d = fsrRcasLoadF(P + ivec2(-1, 0)).rgb;
	 vec3 e = fsrRcasLoadF(P).rgb;
	 vec3 f = fsrRcasLoadF(P + ivec2(1, 0)).rgb;
	 vec3 h = fsrRcasLoadF(P + ivec2(0, 1)).rgb;
	 // Rename (32-bit) or regroup (16-bit).
	 float bR = b.r;
	 float bG = b.g;
	 float bB = b.b;
	 float dR = d.r;
	 float dG = d.g;
	 float dB = d.b;
	 float eR = e.r;
	 float eG = e.g;
	 float eB = e.b;
	 float fR = f.r;
	 float fG = f.g;
	 float fB = f.b;
	 float hR = h.r;
	 float hG = h.g;
	 float hB = h.b;
	 // Luma times 2.
	 float bL = bB * 0.5f + (bR * 0.5f + bG);
	 float dL = dB * 0.5f + (dR * 0.5f + dG);
	 float eL = eB * 0.5f + (eR * 0.5f + eG);
	 float fL = fB * 0.5f + (fR * 0.5f + fG);
	 float hL = hB * 0.5f + (hR * 0.5f + hG);
	 // Min and max of ring.
	 float Min4R = min(min3F1(bR, dR, fR), hR);
	 float Min4G = min(min3F1(bG, dG, fG), hG);
	 float Min4B = min(min3F1(bB, dB, fB), hB);
	 float Max4R = max(max3F1(bR, dR, fR), hR);
	 float Max4G = max(max3F1(bG, dG, fG), hG);
	 float Max4B = max(max3F1(bB, dB, fB), hB);
	 // Immediate constants for peak range.
	 vec2 PeakC = vec2(1.0, -1.0 * 4.0);
	 // Limiters, these need to be high precision RCPs.
	 float HitMinR = Min4R * (1.0f / (4.0f * Max4R));
	 float HitMinG = Min4G * (1.0f / (4.0f * Max4G));
	 float HitMinB = Min4B * (1.0f / (4.0f * Max4B));
	 float HitMaxR = (PeakC.x - Max4R) * (1.0f / (4.0f * Min4R + PeakC.y));
	 float HitMaxG = (PeakC.x - Max4G) * (1.0f / (4.0f * Min4G + PeakC.y));
	 float HitMaxB = (PeakC.x - Max4B) * (1.0f / (4.0f * Min4B + PeakC.y));
	 float LobeR = max(-HitMinR, HitMaxR);
	 float LobeG = max(-HitMinG, HitMaxG);
	 float LobeB = max(-HitMinB, HitMaxB);
	 float Lobe = max(float(-FSR_RCAS_LIMIT), min(max3F1(LobeR, LobeG, LobeB), 0.0)) * (u_Con0.x);
	 // Resolve, which needs the medium precision rcp approximation to avoid visible tonality changes.
	 float RcpL = 1.0f / (4.0f * Lobe + 1.0f);
	 pixR = (Lobe * bR + Lobe * dR + Lobe * hR + Lobe * fR + eR) * RcpL;
	 pixG = (Lobe * bG + Lobe * dG + Lobe * hG + Lobe * fG + eG) * RcpL;
	 pixB = (Lobe * bB + Lobe * dB + Lobe * hB + Lobe * fB + eB) * RcpL;
 }

void main()
{
	ivec2 FragPos = ivec2(gl_GlobalInvocationID.xy);
	vec3 Color = vec3(0);
	fsrRcasF(Color.r,Color.g,Color.b,FragPos);
	imageStore(u_OutputRCASImage, FragPos, vec4(Color, 1));
}