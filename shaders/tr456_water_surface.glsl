#version 150
#ifndef TR456_WATER_DEBUG_MODE
#define TR456_WATER_DEBUG_MODE 0
#define TR456_WATER_REFLECTION_QUALITY 1
#define TR456_WATER_SURFACE_WAVE 1.0
#define TR456_WATER_PIXEL_WAVE_STRENGTH 1.55
#define TR456_WATER_REFRACTION_WAVE_STRENGTH 1.45
#define TR456_WATER_DEEP_CAUSTICS_STRENGTH 1.0
#define TR456_WATER_VOLUME_STRENGTH 1.0
#define TR456_WATER_SHORELINE_STRENGTH 0.75
#define TR456_WATER_GAME_RIPPLE_STRENGTH 0.85
#define TR456_WATER_REFRACT_STRENGTH 1.0
#define TR456_WATER_REFLECT_STRENGTH 1.0
#define TR456_WATER_SSR_STRENGTH 1.0
#define TR456_WATER_GLINT_STRENGTH 1.0
#define TR456_WATER_FOAM_STRENGTH 0.75
#define TR456_WATER_CHROMA_STRENGTH 0.55
#define TR456_WATER_TINT_STRENGTH 1.0
#define TR456_WATER_OPACITY 0.62
#define TR456_WATER_FORCE_REFLECTION 0.65
#define TR456_WATER_SCENE_REFLECTION 0.78
#define TR456_WATER_CAUSTICS_STRENGTH 1.10
#define TR456_WATER_DEPTH_STRENGTH 1.0
#define TR456_WATER_RIPPLE_STRENGTH 0.65
#define TR456_WATER_RIPPLE_CENTER_X 0.50
#define TR456_WATER_RIPPLE_CENTER_Y 0.62
#define TR456_WATER_SURFACE_RELIEF 1.0
#define TR456_WATER_WAKE_STRENGTH 1.0
#define TR456_WATER_WAKE_WIDTH 0.42
#define TR456_WATER_WAKE_LENGTH 0.58
#define TR456_WATER_CONTACT_WAVE_STRENGTH 0.70
#define TR456_WATER_CONTACT_WAVE_RADIUS 1.0
#define TR456_WATER_CONTACT_WAVE_SPEED 1.20
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.75
#define TR456_WATER_CONTACT_COORD_MODE 1
#define TR456_WATER_MICRO_RIPPLE 1.25
#define TR456_WATER_MICRO_SCALE 1.0
#define TR456_WATER_MIRROR_ROUGHNESS 1.0
#define TR456_WATER_SWELL_STRENGTH 0.85
#define TR456_WATER_SWELL_SCALE 1.0
#define TR456_WATER_WAKE_WAVE 1.0
#define TR456_WATER_EDGE_WAVE 0.75
#define TR456_WATER_EDGE_WIDTH 0.09
#define TR456_WATER_REFLECTION_CONTRAST 1.45
#define TR456_WATER_BOTTOM_CAUSTICS 0.85
#define TR456_WATER_CONTACT_EDGE 0.72
#define TR456_WATER_DEPTH_ABSORPTION 0.88
#define TR456_WATER_TEXTURE_STRENGTH 1.0
#define TR456_WATER_FBO_REFLECTION 0
#endif

uniform sampler2D uTrWaterScene;
uniform vec4 uTrWaterCaptureInfo;
uniform sampler3D sNoise;
uniform sampler2DArray sTex0_wrap;
uniform vec4 uFogColor;
uniform vec4 uContacts[16];
uniform vec4 uModelMatrix[4];
uniform vec4 uParams;
uniform vec4 uColor;
in vec2 vTexCoord;
in vec3 vColor;
in vec3 vLight;
in float vLayer;
in float vFog;
in vec3 vNormal;
in vec3 vPos;
in vec3 vWorldPos;
in vec3 vContactWave;
out vec4 fragColor;

float sat(float x){ return clamp(x,0.0,1.0); }
float luma(vec3 c){ return dot(c,vec3(.3333)); }

vec3 waterVolumeAbsorption(vec3 c, float body, float ndv){
 float path=sat(body*TR456_WATER_DEPTH_ABSORPTION*(.88+.24*(1.0-ndv)));
 vec3 absorbed=c*exp(-vec3(.62,.26,.13)*path);
 float y=luma(absorbed);
 absorbed=mix(absorbed,vec3(y)*vec3(.66,.86,.91),sat(path*.28));
 vec3 deepTint=vec3(.002,.034,.044)*TR456_WATER_TINT_STRENGTH;
 return mix(absorbed,deepTint,sat(path*.14*TR456_WATER_OPACITY));
}

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

vec3 captureColor(vec2 uv){
 return texture(uTrWaterScene,clamp(uv,vec2(0.0),vec2(1.0))).rgb;
}

vec3 reflectionGrade(vec3 c){
 c=max(c-vec3(.010),vec3(0.0))*TR456_WATER_REFLECTION_CONTRAST;
 c=mix(c,c*vec3(.84,.95,1.04),.16);
 return clamp(c,vec3(0.0),vec3(3.0));
}

vec4 pixelWaveField(vec2 p, float time){
 p*=.92;
 vec2 a=normalize(vec2(.88,.47));
 vec2 b=normalize(vec2(-.36,.93));
 vec2 c=normalize(vec2(.22,.98));
 float bendA=sin(dot(p,b)*1.22+time*.20)*.30;
 float bendB=sin(dot(p,a)*.86-time*.14)*.22;
 float phA=dot(p,a)*6.10+bendA+time*.62;
 float phB=dot(p,b)*4.25+bendB-time*.42;
 float phC=dot(p,c)*8.40+sin(dot(p,vec2(.71,-.70))*1.15+time*.12)*.16+time*.78;
 float h=sin(phA)*.50+sin(phA*2.0+.55)*.10+sin(phB)*.27+sin(phC)*.13;
 vec2 grad=a*(cos(phA)*.50+cos(phA*2.0+.55)*.20)+
   b*(cos(phB)*.27)+c*(cos(phC)*.13);
 float crest=sat(abs(h)*.72+.18);
 float longBand=pow(sat(1.0-abs(fract(phA*.159+sin(phB)*.045)-.5)*2.0),4.8);
 return vec4(grad*.0180*TR456_WATER_PIXEL_WAVE_STRENGTH,
   sat(crest*.62+longBand*.38)*TR456_WATER_PIXEL_WAVE_STRENGTH,h);
}

float deepCausticField(vec2 p, float time, float depthMask, vec2 waveBend){
 p+=waveBend*.18+vec2(time*.018,-time*.011);
 float n0=texture(sNoise,vec3(p*.92+vec2(.17,.43),time*.017)).x;
 float n1=texture(sNoise,vec3(p*1.71+vec2(.53,.11),time*.013)).x;
 float n2=texture(sNoise,vec3(p.yx*2.38+vec2(.29,.71),time*.011)).x;
 float web=1.0-abs((n0*.50+n1*.34+n2*.16)-.54)*4.7;
 float vein=1.0-abs(n0-n2)*3.8;
 float scatter=texture(sNoise,vec3(p*.44+vec2(.11,.29),time*.007)).x;
 float broken=smoothstep(.42,.86,texture(sNoise,vec3(p*2.85+vec2(.67,.23),time*.019)).x)*
   (.55+.45*smoothstep(.22,.82,scatter));
 float soft=smoothstep(.24,.86,depthMask)*(1.0-smoothstep(.82,1.0,depthMask)*.42);
 float lines=pow(sat(web),2.25)*pow(sat(vein),1.20);
 lines=smoothstep(.015,.52,lines)*lines;
 return lines*broken*soft*
   TR456_WATER_DEEP_CAUSTICS_STRENGTH*TR456_WATER_BOTTOM_CAUSTICS;
}

vec2 microRipple(vec2 p, float time){
 p*=max(TR456_WATER_MICRO_SCALE,.10);
 vec2 a=vec2(sin(p.x*37.0+p.y*16.0+time*1.25),
             cos(p.x*19.0-p.y*31.0-time*1.05));
 vec2 b=vec2(sin((p.x+p.y)*58.0+time*1.70),
             cos((p.x-p.y)*49.0-time*1.30));
 vec2 c=vec2(sin(p.x*91.0+p.y*13.0-time*.85),
             sin(p.y*83.0-p.x*21.0+time*1.15));
 return (a*.008+b*.0045+c*.0025)*TR456_WATER_MICRO_RIPPLE;
}

vec3 surfaceSwell(vec2 p, float time){
 p*=max(TR456_WATER_SWELL_SCALE,.10);
 float phaseA=dot(p,vec2(1.30,.42))*6.28318+time*.32;
 float phaseB=dot(p,vec2(-.55,1.12))*6.28318-time*.24;
 vec2 center=p-vec2(.35,-.25);
 float phaseC=length(center)*7.4-time*.38;
 float h=sin(phaseA)*.46+sin(phaseB)*.34+sin(phaseC)*.20;
 vec2 radial=normalize(center+vec2(.0001));
 vec2 grad=vec2(cos(phaseA)*1.30+cos(phaseB)*(-.55*.72),
                cos(phaseA)*.42+cos(phaseB)*(1.12*.72));
 grad+=radial*cos(phaseC)*.58;
 float crest=sat(abs(h)*.74+.10);
 return vec3(grad*.0065*TR456_WATER_SWELL_STRENGTH,crest*TR456_WATER_SWELL_STRENGTH);
}

vec3 edgeRippleLayer(vec2 p, vec2 screen, float time){
 float scale=mix(.70,1.35,sat(TR456_WATER_EDGE_WIDTH*9.0));
 p*=scale;
 float a=sin(dot(p,vec2(1.55,.38))*18.0-time*2.4);
 float b=sin(dot(p,vec2(-.42,1.18))*24.0+time*1.9);
 float c=sin((p.x+p.y)*37.0-time*3.1);
 vec2 grad=vec2(cos(dot(p,vec2(1.55,.38))*18.0-time*2.4)*1.55+
                cos((p.x+p.y)*37.0-time*3.1)*.45,
                cos(dot(p,vec2(-.42,1.18))*24.0+time*1.9)*1.18+
                cos((p.x+p.y)*37.0-time*3.1)*.45);
 float viewFade=smoothstep(.015,.070,screen.x)*smoothstep(.015,.070,screen.y)*
   smoothstep(.015,.070,1.0-screen.x)*smoothstep(.015,.070,1.0-screen.y);
 float crest=sat(abs(a)*.30+abs(b)*.23+abs(c)*.14-.18);
 return vec3(grad*.0020*TR456_WATER_EDGE_WAVE*viewFade,
   crest*.18*TR456_WATER_EDGE_WAVE*viewFade);
}

vec3 playerWake(vec2 screen, float time){
 vec2 p=(screen-vec2(TR456_WATER_RIPPLE_CENTER_X,TR456_WATER_RIPPLE_CENTER_Y))*vec2(1.35,1.0);
 float d=length(p)+.0001;
 float width=max(TR456_WATER_WAKE_WIDTH,.05);
 float wakeLen=max(TR456_WATER_WAKE_LENGTH,.08);
 float ringA=sin(d*86.0-time*7.2)*exp(-d*4.6)*smoothstep(.020,.20,d);
 float ringB=sin(d*46.0-time*4.1+p.x*3.0)*exp(-d*2.5)*smoothstep(.070,.58,d);
 float trail=(1.0-smoothstep(width*.14,width,abs(p.x)))*
   smoothstep(-.34,.02,p.y)*(1.0-smoothstep(.08,wakeLen,p.y));
 float wake=sin((p.y-time*.30)*74.0+sin(p.x*18.0))*trail*exp(-abs(p.x)*5.4);
 float stepPhase=sin(time*6.3);
 vec2 footL=p-vec2(-.085,.018+stepPhase*.018);
 vec2 footR=p-vec2(.085,.018-stepPhase*.018);
 float foot=exp(-dot(footL,footL)*115.0)*(stepPhase*.5+.5)+
   exp(-dot(footR,footR)*115.0)*(.5-stepPhase*.5);
 float footWave=(sin(d*128.0-time*10.5)*exp(-d*7.2)*foot+
   sin((d-time*.42)*72.0)*trail*.56)*TR456_WATER_WAKE_WAVE;
 vec2 dir=normalize(p);
 vec2 flow=dir*(ringA*.036+ringB*.018+foot*.026+footWave*.034)+vec2(p.x,-p.y)*(wake*.016);
 float crest=sat(abs(ringA)*.80+abs(ringB)*.42+abs(wake)*.55+foot*.95+abs(footWave)*.85);
 return vec3(flow*TR456_WATER_RIPPLE_STRENGTH*TR456_WATER_WAKE_STRENGTH,crest*TR456_WATER_WAKE_STRENGTH);
}

float isScreenContact(vec4 c){
 return 1.0-step(-.001,c.z);
}

float contactAge(vec4 c){
 if(isScreenContact(c)>.5) return max(abs(c.w)-1.0,0.0);
 float packed=abs(c.w);
 float r=floor(packed*(1.0/512.0));
 return max(packed-r*512.0-1.0,0.0);
}

float contactRadius(vec4 c){
 if(isScreenContact(c)>.5)
   return max(abs(c.z),14.0)*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
 float packed=abs(c.w);
 float r=floor(packed*(1.0/512.0));
 float fallback=720.0;
 return mix(fallback,r,step(96.0,r))*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec3 contactWaveFieldPixel(vec3 pos, vec2 screen, vec2 invViewport){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 float life=138.0;
 vec2 slope=vec2(0.0);
 float crest=0.0;
 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float active=step(.001,dot(abs(c),vec4(1.0)));
   float radius=contactRadius(c);
   float screenContact=isScreenContact(c);
   vec2 delta;
   float vertical;
   if(screenContact>.5) {
     vec2 inv=max(invViewport,vec2(1.0/8192.0));
     delta=(screen-c.xy)/inv;
     vertical=1.0;
   } else {
     vec2 deltaXZ=pos.xz-c.xz;
     vec2 deltaXY=pos.xy-c.xy;
     float dXZ=length(deltaXZ);
     float dXY=length(deltaXY);
     float autoXY=step(dXY,dXZ);
     float mode=float(TR456_WATER_CONTACT_COORD_MODE);
     float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
     delta=mix(deltaXZ,deltaXY,useXY);
     vertical=1.0-smoothstep(radius*.22,radius*1.24,abs(pos.y-c.y));
   }
   float d=length(delta)+.001;
   vec2 dir=delta/d;
   float age=clamp(contactAge(c),0.0,life);
   float fade=active*(1.0-smoothstep(life*.72,life,age));
   float grow=smoothstep(0.0,life*.72,age);
   float front=mix(max(54.0,radius*(.54+.18*grow)+age*(1.8+speed*1.6)),
     max(9.0,radius*(.48+.10*grow)+age*(.40+speed*.34)),screenContact);
   float width=mix(max(34.0,radius*(.060+.022*grow)),
     max(5.5,radius*(.028+.010*grow)),screenContact);
   float crestX=(d-front)/width;
   float troughX=(d-(front-width*.94))/(width*1.36);
   float crestRing=exp(-crestX*crestX);
   float trough=exp(-troughX*troughX);
   float shell=(1.0-smoothstep(radius*2.80,radius*4.05,d))*vertical;
   float phase=(d-front)*(.034+.004*speed);
   float rim=(crestRing*.86-trough*.34+
     sin(phase)*crestRing*.18+sin(phase*1.86+1.25)*crestRing*.08)*shell;
   float dCrest=(-2.0*crestX/width)*crestRing;
   float dTrough=(-2.0*troughX/(width*1.36))*trough;
   float dFine=cos(phase)*(.034+.004*speed)*crestRing*.18+
     cos(phase*1.86+1.25)*(.063+.007*speed)*crestRing*.08;
   float dWave=(dCrest*.86-dTrough*.34+dFine)*shell;
   float source=exp(-d/(radius*.33))*(1.0-smoothstep(1.0,34.0,age))*vertical;
   float waterline=source*(.55+.45*sin(d*.080-age*.42+
     texture(sNoise,vec3(pos.xz*.0018,float(i)*.13)).x*2.4));
   float dSource=-source/(radius*.33);
   float energy=fade*(.88+.12*sin(float(i)*1.37));
   slope+=dir*(dWave*mix(1.0,2.65,screenContact)+dSource*.34+waterline*.018)*energy;
   crest+=sat(abs(rim)*.95+source*.22+abs(waterline)*.28)*energy;
 }
 return vec3(slope*1.95*strength,crest*strength);
}

vec4 authoredRippleField(vec2 uv, float layer){
 vec2 sx=vec2(.0034,0.0);
 vec2 sy=vec2(0.0,.0034);
 float cc=luma(texture(sTex0_wrap,vec3(uv,layer)).rgb);
 float ll=luma(texture(sTex0_wrap,vec3(uv-sx,layer)).rgb);
 float rr=luma(texture(sTex0_wrap,vec3(uv+sx,layer)).rgb);
 float dd=luma(texture(sTex0_wrap,vec3(uv-sy,layer)).rgb);
 float uu=luma(texture(sTex0_wrap,vec3(uv+sy,layer)).rgb);
 vec2 g=vec2(rr-ll,uu-dd);
 float local=abs(cc-(ll+rr+dd+uu)*.25);
 float ring=smoothstep(.010,.075,local+length(g)*.46);
 float thin=smoothstep(.020,.115,length(g))*(1.0-smoothstep(.18,.50,abs(cc-.50)));
 float mask=sat(ring*.70+thin*.45);
 return vec4(g*mask,mask,cc);
}

void main(){
 vec2 uv=vTexCoord;
 vec3 n=normalize(vNormal);
 vec3 viewVec=normalize(-vPos);
 float ndv=sat(dot(n,viewVec));
 float t=uModelMatrix[3].x*uParams.z;
 vec3 tc=vWorldPos.xyz*(uParams.x/1024.0);

 float a=texture(sNoise,tc*.58+n*t*.16).x;
 float b=texture(sNoise,tc*1.07-n*t*.12+vec3(.17,.07,.31)).x;
 float c=texture(sNoise,tc*1.91+n*t*.08+vec3(.41,.13,0.0)).x;
 float d=texture(sNoise,tc*2.80+vec3(-t*.045,t*.025,t*.012)).x;
 vec2 grad=vec2(a-b,c-a)+vec2(d-c,b-d)*.24;
 vec2 invViewport=captureInvViewport();
 vec2 screen=gl_FragCoord.xy*invViewport;
 vec3 wake=playerWake(screen,t);
 vec3 contactPixel=contactWaveFieldPixel(vWorldPos,screen,invViewport);
 vec3 contactWave=vec3(vContactWave.xy*.25+contactPixel.xy,
   max(abs(vContactWave.z)*.35,contactPixel.z));
 vec2 stableUv=tc.xz*.72+vec2(t*.012,-t*.008);
 vec2 micro=microRipple(tc.xz*.56+vec2(t*.010,-t*.007),t);
 vec3 swell=surfaceSwell(tc.xz*.26,t);
 vec3 edgeRip=edgeRippleLayer(tc.xz*.30,screen,t);
 vec4 pixelWave=pixelWaveField(tc.xz*.42+vec2(t*.006,-t*.004),t);
 vec4 authoredRing=authoredRippleField(uv,vLayer);
 wake.xy+=authoredRing.xy*.155*TR456_WATER_GAME_RIPPLE_STRENGTH;
 wake.z+=authoredRing.z*1.10*TR456_WATER_GAME_RIPPLE_STRENGTH;
 wake.xy+=contactWave.xy*.30;
 wake.z+=abs(contactWave.z)*.48;
 grad+=micro*(.38*TR456_WATER_SURFACE_RELIEF);
 grad+=swell.xy*(.92*TR456_WATER_SURFACE_RELIEF);
 grad+=edgeRip.xy*(.72*TR456_WATER_SURFACE_RELIEF);
 grad+=pixelWave.xy*(2.15*TR456_WATER_SURFACE_RELIEF);
 grad+=authoredRing.xy*(1.85*TR456_WATER_GAME_RIPPLE_STRENGTH*TR456_WATER_SURFACE_RELIEF);
 grad+=contactWave.xy*(1.35*TR456_WATER_CONTACT_NORMAL_STRENGTH*TR456_WATER_SURFACE_RELIEF);
 grad+=wake.xy*(1.05*TR456_WATER_SURFACE_RELIEF);
 float wave=sat((length(grad)*2.45+wake.z*.38+authoredRing.z*.34+swell.z*.18+edgeRip.z*.20+
   pixelWave.z*.52+length(micro)*2.8)*TR456_WATER_SURFACE_RELIEF);
 vec2 warp=(grad*uParams.y*1.42+wake.xy*.56+micro*.14+swell.xy*.44+
   edgeRip.xy*.36+pixelWave.xy*1.35)*(TR456_WATER_SURFACE_WAVE*
   TR456_WATER_SURFACE_RELIEF*TR456_WATER_REFRACT_STRENGTH*
   TR456_WATER_REFRACTION_WAVE_STRENGTH);

 vec2 texUv=stableUv+warp*.36;
 vec4 meshBase=texture(sTex0_wrap,vec3(uv+warp,vLayer));
 vec4 worldBase=texture(sTex0_wrap,vec3(texUv,vLayer));
 vec4 base=mix(meshBase,worldBase,.68);
 base.a=mix(meshBase.a,worldBase.a,.52);
 vec3 r=mix(
   texture(sTex0_wrap,vec3(texUv+warp*(.24+TR456_WATER_CHROMA_STRENGTH*.06)+.0007*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv+warp*(.10+TR456_WATER_CHROMA_STRENGTH*.04)+.0003*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   .34);
 vec3 bl=mix(
   texture(sTex0_wrap,vec3(texUv-warp*(.20+TR456_WATER_CHROMA_STRENGTH*.04)-.0007*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   texture(sTex0_wrap,vec3(texUv-warp*(.09+TR456_WATER_CHROMA_STRENGTH*.03)-.0003*TR456_WATER_CHROMA_STRENGTH,vLayer)).rgb,
   .34);
 vec3 refr=mix(base.rgb,vec3(r.r,base.g,bl.b),.20*TR456_WATER_CHROMA_STRENGTH);
 refr=mix(vec3(luma(refr)),refr,clamp(TR456_WATER_TEXTURE_STRENGTH,.60,1.75));
 float waveShade=clamp(pixelWave.w*.50+.50,0.0,1.0);
 float waveBand=smoothstep(.20,.82,pixelWave.z);
 float depthHint=sat((1.0-vFog)*.72+(1.0-base.a)*.30+wave*.16);
 float materialEdge=sat(abs(meshBase.a-worldBase.a)*1.65+length(meshBase.rgb-worldBase.rgb)*.36+
   authoredRing.z*.82+edgeRip.z*.42);
 float shoreline=sat(smoothstep(.16,.56,materialEdge+(1.0-base.a)*.22)*
   (1.0-smoothstep(.52,.96,depthHint))*TR456_WATER_SHORELINE_STRENGTH*
   TR456_WATER_CONTACT_EDGE);
 float waterlinePattern=sin(dot(tc.xz,vec2(13.7,5.4))+t*2.15)+
   sin(dot(tc.xz,vec2(-7.1,15.8))-t*1.72)*.55+
   (texture(sNoise,vec3(tc.xz*.92+vec2(t*.020,-t*.012),t*.010)).x-.5)*1.15;
 float waterlineRipple=shoreline*smoothstep(.18,1.18,abs(waterlinePattern))*
   TR456_WATER_EDGE_WAVE*(.62+.38*vFog);
 wave=sat(wave+waterlineRipple*.18);
 float volume=sat(smoothstep(.14,.92,depthHint)*TR456_WATER_VOLUME_STRENGTH);
 refr*=1.0+(waveShade-.5)*.18*clamp(TR456_WATER_PIXEL_WAVE_STRENGTH,0.0,2.5);
 refr+=vec3(.030,.075,.078)*waveBand*(.45+.55*vFog);
 float volumeDepth=sat(volume*.80+depthHint*.24+(1.0-vFog)*.10);
 refr=waterVolumeAbsorption(refr,volumeDepth,ndv);

 float fres=.035+.72*pow(1.0-ndv,4.0);
 float ridge=smoothstep(.17,.54,wave)*(1.0-smoothstep(.48,.96,wave));
 float silk=pow(sat(1.0-abs((a*.52+b*.31+c*.17)-.53)*2.8),5.0);
 float flow=pow(1.0-abs(fract(tc.x*.28+tc.z*.19+a*.18+t*.018)-.5)*2.0,9.0);
 float film=pow(1.0-abs(fract((a+b*1.22+c+d*.18+t*.030)*2.15)-.5)*2.0,10.0);
 float microEnergy=sat(length(micro)*58.0);
 float foam=(pow(ridge,1.55)*(0.28+0.38*flow)*.045+wake.z*.026+swell.z*.006+
   edgeRip.z*.012+microEnergy*.003+shoreline*.012+waterlineRipple*.028+
   authoredRing.z*.020)*TR456_WATER_FOAM_STRENGTH;
 float glint=(silk*.010+film*.007+foam*.32+wake.z*.008+swell.z*.005+
   edgeRip.z*.006+microEnergy*.003+shoreline*.004+waterlineRipple*.010)*
   TR456_WATER_GLINT_STRENGTH;
 float causticDepth=smoothstep(.24,.88,depthHint)*(1.0-shoreline*.55);
 float caustics=deepCausticField(tc.xz*.48+warp*.18,t,depthHint,grad+pixelWave.xy);
 caustics=caustics*(.030+.046*causticDepth)*causticDepth*
   TR456_WATER_CAUSTICS_STRENGTH;

 vec3 shallow=vec3(.020,.185,.205);
 vec3 deep=vec3(.004,.045,.060);
 float depth=sat((1.0-vFog)*.85+wave*.18)*TR456_WATER_DEPTH_STRENGTH;
 vec3 tint=mix(deep,shallow,.62+.28*fres-depth*.28)*TR456_WATER_TINT_STRENGTH;
 vec3 light=clamp((vLight+vColor)*1.24,0.0,1.70);
 refr+=vec3(.10,.22,.19)*caustics*(.50+.50*depthHint);
 vec3 col=mix(refr,tint,(.14+.13*fres)*TR456_WATER_OPACITY)*light;
 col=mix(col,(deep*.92+vec3(.000,.016,.022))*light,sat(volume*.08*TR456_WATER_DEPTH_ABSORPTION));
 col=mix(col,col*vec3(.70,.88,.88)+vec3(.006,.030,.026),sat(shoreline*.26));
 col+=vec3(.34,.56,.60)*glint;
 col+=vec3(.030,.090,.095)*waterlineRipple*(.35+.65*fres);
 col+=vec3(.040,.105,.112)*waveBand*(.28+.72*fres);
 col+=vec3(.035,.070,.075)*(flow*.55+film*.45+ridge*.60)*max(TR456_WATER_TEXTURE_STRENGTH-1.0,0.0);
 col+=vec3(.15,.28,.32)*pow(1.0-wave,6.0)*.018;

 vec2 mirrorUv=vec2(screen.x,1.0-screen.y)+vec2(warp.x+micro.x*.32+swell.x*.70+edgeRip.x*.55,-abs(warp.y+micro.y*.25+swell.y*.70+edgeRip.y*.55)*.34)*(.48+.68*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec2 mirrorUv2=mirrorUv+vec2(grad.x+micro.x*.22+swell.x*.82+edgeRip.x*.65,-grad.y-micro.y*.22-swell.y*.82-edgeRip.y*.65)*(.014+.024*fres)*TR456_WATER_MIRROR_ROUGHNESS;
 vec3 sceneRefl=reflectionGrade(captureColor(mirrorUv)*.68+captureColor(mirrorUv2)*.32);
 float reflOk=smoothstep(.004,.040,luma(sceneRefl));
 float reflMask=clamp((.12+fres*.78+ridge*.10)*TR456_WATER_REFLECT_STRENGTH*
   max(TR456_WATER_FORCE_REFLECTION,.25),0.0,.82)*reflOk;
 col=mix(col,sceneRefl,reflMask*(.34+.22*vFog));

 col=mix(col,col*vec3(.58,.80,.88),sat((depth*.36+volume*.24)*TR456_WATER_DEPTH_ABSORPTION));
 col=mix(uFogColor.rgb*base.a,col,vFog);
 float alpha=clamp((base.a*(.54+.06*fres)+foam*.06+waveBand*.018+
   shoreline*.012+waterlineRipple*.020+authoredRing.z*.010)*TR456_WATER_OPACITY,.045,.35);
 alpha=max(alpha,reflMask*.14);

#if TR456_WATER_DEBUG_MODE == 1
 fragColor=vec4(vec3(wave),1.0);
#elif TR456_WATER_DEBUG_MODE == 2
 fragColor=vec4(vec3(fres),1.0);
#elif TR456_WATER_DEBUG_MODE == 3
 fragColor=vec4(vec3(foam*6.0),1.0);
#elif TR456_WATER_DEBUG_MODE == 5
 fragColor=vec4(sceneRefl,1.0);
#elif TR456_WATER_DEBUG_MODE == 6
 fragColor=vec4(reflectionGrade(captureColor(screen)),1.0);
#elif TR456_WATER_DEBUG_MODE == 7
 fragColor=vec4(vec3(reflMask),1.0);
#elif TR456_WATER_DEBUG_MODE == 9
 fragColor=vec4(authoredRing.z,shoreline,waterlineRipple,1.0);
#elif TR456_WATER_DEBUG_MODE == 10
 fragColor=vec4(abs(contactWave.z)*2.0,abs(contactWave.x)*12.0,abs(contactWave.y)*12.0,1.0);
#elif TR456_WATER_DEBUG_MODE == 8
 fragColor=vec4(.0,.85,1.0,1.0);
#else
 fragColor=vec4(col,alpha)*uColor;
#endif
}
