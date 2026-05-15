#version 150

#ifndef TR456_WATER_GRID_SUBDIVISION
#define TR456_WATER_GRID_SUBDIVISION 8
#endif
#ifndef TR456_WATER_GRID_STRENGTH
#define TR456_WATER_GRID_STRENGTH 1.0
#endif
#ifndef TR456_WATER_CONTACT_WAVE_STRENGTH
#define TR456_WATER_CONTACT_WAVE_STRENGTH 1.12
#endif
#ifndef TR456_WATER_CONTACT_WAVE_RADIUS
#define TR456_WATER_CONTACT_WAVE_RADIUS 1.0
#endif
#ifndef TR456_WATER_CONTACT_WAVE_SPEED
#define TR456_WATER_CONTACT_WAVE_SPEED 0.98
#endif
#ifndef TR456_WATER_CONTACT_VERTEX_STRENGTH
#define TR456_WATER_CONTACT_VERTEX_STRENGTH 0.46
#endif
#ifndef TR456_WATER_CONTACT_NORMAL_STRENGTH
#define TR456_WATER_CONTACT_NORMAL_STRENGTH 0.95
#endif
#ifndef TR456_WATER_CONTACT_COORD_MODE
#define TR456_WATER_CONTACT_COORD_MODE 1
#endif
#ifndef TR456_WATER_RAIN_RIPPLE
#define TR456_WATER_RAIN_RIPPLE 1.06
#endif
#ifndef TR456_WATER_SIM_STRENGTH
#define TR456_WATER_SIM_STRENGTH 1.0
#endif
#ifndef TR456_WATER_SIM_SCALE
#define TR456_WATER_SIM_SCALE 1.0
#endif
#ifndef TR456_WATER_SIM_SPEED
#define TR456_WATER_SIM_SPEED 1.0
#endif
#ifndef TR456_WATER_SIM_GERSTNER
#define TR456_WATER_SIM_GERSTNER 0.78
#endif
#ifndef TR456_WATER_SIM_SHALLOW
#define TR456_WATER_SIM_SHALLOW 0.62
#endif
#ifndef TR456_WATER_SIM_CONTACT
#define TR456_WATER_SIM_CONTACT 1.0
#endif
#ifndef TR456_WATER_SIM_RAIN
#define TR456_WATER_SIM_RAIN 0.72
#endif
#ifndef TR456_WATER_SIM_FLOW
#define TR456_WATER_SIM_FLOW 0.84
#endif
#ifndef TR456_WATER_SIM_DAMPING
#define TR456_WATER_SIM_DAMPING 0.985
#endif
#ifndef TR456_WATER_SIM_GRID_STEP
#define TR456_WATER_SIM_GRID_STEP 260.0
#endif

layout(triangles) in;
layout(triangle_strip, max_vertices = 80) out;

uniform mat4 uProjMatrix;
uniform vec4 uViewMatrix[4];
uniform vec4 uModelMatrix[4];
uniform vec4 uContacts[16];
uniform vec4 uTrWaterCaptureInfo;
uniform vec4 uTrWaterToggle2;
uniform vec4 uParams;
uniform vec4 uTrWaterGridInfo;

in vec3 gGridPos[];
in vec3 gGridWorldPos[];
in vec3 gGridNormal[];
in vec3 gGridLight[];
in vec3 gGridColor[];
in vec2 gGridUv[];

out vec3 vGridWave;
out vec3 vGridColor;
out float vGridFog;
out float vGridFlow;

#define TR_TOGGLE_MESH_DISPLACEMENT uTrWaterToggle2.z
#define TR_TOGGLE_CONTACT_RIPPLES uTrWaterToggle2.w

float sat(float x){ return clamp(x,0.0,1.0); }

float openLaraDrop(float nr){
 float x=max(0.0,1.0-nr);
 return 0.5-cos(x*3.141592653589793)*0.5;
}

float openLaraDropSlope(float nr, float r){
 float x=max(0.0,1.0-nr);
 return -0.5*3.141592653589793*sin(x*3.141592653589793)/max(r,1.0);
}

vec4 clipFromPos(vec3 p){
 return uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p),
                         dot(uViewMatrix[1].xyz,p),
                         dot(uViewMatrix[2].xyz,p),1.0);
}

vec2 screenFromClip(vec4 clip){
 float w=(abs(clip.w)<.00001) ? ((clip.w<0.0) ? -.00001 : .00001) : clip.w;
 return clip.xy/w*.5+.5;
}

vec2 captureInvViewport(){
 float hasInfo=step(.000001,uTrWaterCaptureInfo.x)*step(.000001,uTrWaterCaptureInfo.y);
 return mix(vec2(1.0/1920.0,1.0/1080.0),uTrWaterCaptureInfo.xy,hasInfo);
}

float isScreenContact(vec4 c){
 return 1.0-step(-.001,c.z);
}

float contactAge(vec4 c){
 if(isScreenContact(c)>.5) return max(abs(c.w)-1.0,0.0);
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 return max(packedValue-radiusPacked*512.0-1.0,0.0);
}

float contactRadius(vec4 c){
 if(isScreenContact(c)>.5)
   return max(abs(c.z),14.0)*clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
 float packedValue=abs(c.w);
 float radiusPacked=floor(packedValue*(1.0/512.0));
 return mix(720.0,radiusPacked,step(96.0,radiusPacked))*
   clamp(TR456_WATER_CONTACT_WAVE_RADIUS,0.20,3.0);
}

vec3 contactGridWave(vec3 worldPos, vec4 clip){
 float strength=clamp(TR456_WATER_CONTACT_WAVE_STRENGTH,0.0,2.0);
 float speed=clamp(TR456_WATER_CONTACT_WAVE_SPEED,0.20,3.0);
 vec2 screen=screenFromClip(clip);
 vec2 inv=max(captureInvViewport(),vec2(1.0/8192.0));
 vec2 slope=vec2(0.0);
 float height=0.0;
 float life=170.0;

 for(int i=0;i<16;i++){
   vec4 c=uContacts[i];
   float contactOn=step(.001,dot(abs(c),vec4(1.0)));
   if(contactOn<=.001) continue;
   float screenContact=isScreenContact(c);
   float radius=contactRadius(c);
   vec2 delta;
   float vertical;
   if(screenContact>.5) {
     delta=(screen-c.xy)/inv;
     vertical=1.0;
   } else {
     vec2 deltaXZ=worldPos.xz-c.xz;
     vec2 deltaXY=worldPos.xy-c.xy;
     float dXZ=length(deltaXZ);
     float dXY=length(deltaXY);
     float autoXY=step(dXY,dXZ);
     float mode=float(TR456_WATER_CONTACT_COORD_MODE);
     float useXY=clamp(step(1.5,mode)+(1.0-step(.5,mode))*autoXY,0.0,1.0);
     delta=mix(deltaXZ,deltaXY,useXY);
     vertical=1.0-smoothstep(radius*.16,radius*1.18,abs(worldPos.y-c.y));
   }
   float d=length(delta)+.001;
   vec2 dir=delta/d;
   float age=clamp(contactAge(c),0.0,life);
   float fade=contactOn*(1.0-smoothstep(life*.62,life,age));
   float grow=smoothstep(0.0,life*.72,age);

   if(screenContact>.5) {
     float r=max(radius,1.0);
     float nr=d/r;
     float drop=openLaraDrop(nr);
     float dDrop=openLaraDropSlope(nr,r);
     float ringCenter=mix(.12,.62,grow);
     float ringWidth=mix(.044,.064,grow);
     float leadX=(nr-ringCenter)/ringWidth;
     float troughX=(nr-(ringCenter-ringWidth*1.18))/(ringWidth*1.55);
     float lipX=(nr-.72)/.050;
     float lead=exp(-leadX*leadX);
     float trough=exp(-troughX*troughX);
     float lip=exp(-lipX*lipX)*smoothstep(.18,.80,grow);
     float center=drop*(1.0-smoothstep(1.0,34.0,age));
     float spriteMask=(1.0-smoothstep(.78,.92,nr))*smoothstep(.018,.075,nr);
     float osc=cos(age*(.18+.035*speed));
     float h=(lead*.50-trough*.22+lip*.10)*spriteMask+center*.14*osc;
     float dLead=(-2.0*leadX/(ringWidth*r))*lead;
     float dTrough=(-2.0*troughX/(ringWidth*1.55*r))*trough;
     float dLip=(-2.0*lipX/(.050*r))*lip;
     float ringSlope=(dLead*.50-dTrough*.22+dLip*.10)*spriteMask+dDrop*.14*osc;
     height+=h*fade;
     slope+=dir*ringSlope*fade;
     continue;
   }

   float front=mix(max(68.0,radius*(.50+.18*grow)+age*(1.28+speed*1.24)),
     max(10.0,radius*(.40+.10*grow)+age*(.28+speed*.22)),screenContact);
   float width=mix(max(72.0,radius*(.125+.040*grow)),
     max(9.0,radius*(.050+.018*grow)),screenContact);
   float crestX=(d-front)/width;
   float troughX=(d-(front-width*.92))/(width*1.56);
   float crest=exp(-crestX*crestX);
   float trough=exp(-troughX*troughX);
   float trailX=(d-(front-width*2.16))/(width*1.95);
   float trail=exp(-trailX*trailX)*smoothstep(.14,.72,grow);
   float shell=(1.0-smoothstep(radius*3.00,radius*4.50,d))*vertical;
   float phase=(d-front)*(.026+.003*speed);
   float fine=sin(phase)*crest*.08+sin(phase*1.62+1.17)*crest*.035;
   float ring=(crest*.74-trough*.25+trail*.18+fine)*shell;
   float dCrest=(-2.0*crestX/width)*crest;
   float dTrough=(-2.0*troughX/(width*1.56))*trough;
   float dTrail=(-2.0*trailX/(width*1.95))*trail;
   float dFine=cos(phase)*(.026+.003*speed)*crest*.08+
     cos(phase*1.62+1.17)*(.042+.005*speed)*crest*.035;
   float source=exp(-d/(radius*.34))*(1.0-smoothstep(1.0,42.0,age))*vertical;
   float dSource=-source/(radius*.34);
   height+=(ring+source*.26)*fade;
   slope+=dir*(dCrest*.82-dTrough*.30+dTrail*.22+dFine+dSource*.24)*fade;
 }

 return vec3(clamp(slope*1.20*strength,vec2(-.18),vec2(.18)),
   clamp(height*strength,-1.0,1.0));
}

vec3 ambientGridWave(vec2 p, float time){
 vec2 q=p*.0010;
 vec2 a=normalize(vec2(.92,.38));
 vec2 b=normalize(vec2(-.32,.95));
 vec2 c=normalize(vec2(.58,-.82));
 float pa=dot(q,a)*6.28318+sin(dot(q,b)*3.6+time*.11)*.22+time*.34;
 float pb=dot(q,b)*5.10+sin(dot(q,a)*3.0-time*.08)*.16-time*.23;
 float pc=dot(q,c)*7.50+time*.43+sin(dot(q,vec2(.70,-.42))*2.3)*.10;
 float h=sin(pa)*.38+sin(pb)*.26+sin(pc)*.18+sin((pa+pb)*.52+time*.12)*.10;
 vec2 slope=a*cos(pa)*.30+b*cos(pb)*.21+c*cos(pc)*.17+
   normalize(a+b+vec2(.001))*cos((pa+pb)*.52+time*.12)*.08;
 return vec3(slope*.09,h*.26);
}

float hash12(vec2 p){
 vec3 p3=fract(vec3(p.xyx)*.1031);
 p3+=dot(p3,p3.yzx+33.33);
 return fract((p3.x+p3.y)*p3.z);
}

vec3 rainCircleWave(vec2 p, float time){
 float strength=clamp(TR456_WATER_RAIN_RIPPLE,0.0,2.0);
 vec2 slope=vec2(0.0);
 float height=0.0;
 float cell=680.0;
 vec2 base=floor(p/cell);
 for(int ix=-1;ix<=1;ix++){
   for(int iy=-1;iy<=1;iy++){
     vec2 id=base+vec2(float(ix),float(iy));
     float rnd=hash12(id+vec2(5.1,9.7));
     float contactOn=smoothstep(.46,.96,rnd);
     vec2 center=(id+vec2(hash12(id+vec2(1.3,5.7)),
       hash12(id+vec2(8.2,2.4))))*cell;
     vec2 delta=p-center;
     float d=length(delta)+.001;
     vec2 dir=delta/d;
     float age=fract(time*.18+rnd);
     float fade=smoothstep(.035,.15,age)*(1.0-smoothstep(.68,1.0,age))*contactOn;
     float radius=mix(20.0,280.0,age);
     float width=mix(16.0,42.0,age);
     float crestX=(d-radius)/width;
     float troughX=(d-(radius-width*.84))/(width*1.55);
     float crest=exp(-crestX*crestX);
     float trough=exp(-troughX*troughX);
     float shell=1.0-smoothstep(radius+width*2.0,radius+width*4.0,d);
     float h=(crest*.50-trough*.18)*shell;
     float dCrest=(-2.0*crestX/width)*crest;
     float dTrough=(-2.0*troughX/(width*1.55))*trough;
     height+=h*fade;
     slope+=dir*(dCrest*.50-dTrough*.18)*shell*fade;
   }
 }
 return vec3(clamp(slope*.58*strength,vec2(-.12),vec2(.12)),
   clamp(height*.72*strength,-.65,.65));
}

vec3 gerstnerSample(vec2 p, float time, vec2 dir, float wavelength, float speed,
  float amp, float steepness){
 float k=6.28318530718/max(wavelength,1.0);
 float phase=dot(p,dir)*k+time*speed;
 float s=sin(phase);
 float c=cos(phase);
 return vec3(dir*c*amp*k*steepness,s*amp);
}

vec3 sourceGridWave(vec2 p, float time, vec2 flowDir, float flowMode, float flowLen){
 vec2 side=vec2(-flowDir.y,flowDir.x);
 float scale=max(TR456_WATER_SIM_SCALE,0.25);
 float flowBoost=mix(1.0,1.0+flowLen*.18,flowMode*clamp(TR456_WATER_SIM_FLOW,0.0,2.0));
 float amp=clamp(TR456_WATER_SIM_GERSTNER,0.0,2.0)*flowBoost;
 float speed=clamp(TR456_WATER_SIM_SPEED,0.15,3.0);
 vec2 q=p/scale;

 vec3 w=vec3(0.0);
 w+=gerstnerSample(q,time,flowDir,920.0,1.18*speed,1.00,1.10);
 w+=gerstnerSample(q,time,normalize(flowDir*.78+side*.62),540.0,1.72*speed,.46,1.18);
 w+=gerstnerSample(q,time,normalize(flowDir*.55-side*.83),430.0,1.55*speed,.32,1.05);
 w+=gerstnerSample(q,time,side,1280.0,.72*speed,.22,.82);

 float lanes=sin(dot(q,side)*.0044+sin(dot(q,flowDir)*.0017+time*.42)*.85);
 float breaks=sin(dot(q,flowDir)*.0080-time*2.35+lanes*.70);
 float packet=smoothstep(.38,.95,lanes*.5+.5)*sin(dot(q,flowDir)*.0032-time*1.12);
 w.z+=breaks*.105*flowMode+packet*.18*flowMode;
 w.xy+=flowDir*cos(dot(q,flowDir)*.0080-time*2.35+lanes*.70)*.00078*flowMode;
 w.xy+=side*cos(dot(q,side)*.0044+time*.42)*.00052*flowMode;

 w.xy*=max(scale,0.35);
 return vec3(clamp(w.xy*amp,vec2(-.20),vec2(.20)),clamp(w.z*amp,-1.2,1.2));
}

float sourceGridHeight(vec2 p, float time, vec2 flowDir, float flowMode, float flowLen){
 return sourceGridWave(p,time,flowDir,flowMode,flowLen).z;
}

vec3 shallowWaterStep(vec2 p, float time, vec2 flowDir, float flowMode, float flowLen){
 float stepSize=max(TR456_WATER_SIM_GRID_STEP*max(TR456_WATER_SIM_SCALE,0.25),48.0);
 float damping=clamp(TR456_WATER_SIM_DAMPING,0.82,0.998);
 float center=sourceGridHeight(p,time-.10,flowDir,flowMode,flowLen);
 float east=sourceGridHeight(p+vec2(stepSize,0.0),time,flowDir,flowMode,flowLen);
 float west=sourceGridHeight(p-vec2(stepSize,0.0),time,flowDir,flowMode,flowLen);
 float north=sourceGridHeight(p+vec2(0.0,stepSize),time,flowDir,flowMode,flowLen);
 float south=sourceGridHeight(p-vec2(0.0,stepSize),time,flowDir,flowMode,flowLen);

 float propagated=((east+west+north+south)*0.5-center)*damping;
 vec2 slope=vec2(east-west,north-south)/(stepSize*2.0);
 return vec3(clamp(slope*120.0,vec2(-.22),vec2(.22)),clamp(propagated*.42,-1.15,1.15));
}

vec3 simulationGridWave(vec2 p, float time, vec2 flowDir, float flowMode, float flowLen){
 vec3 source=sourceGridWave(p,time,flowDir,flowMode,flowLen);
 vec3 shallow=shallowWaterStep(p,time,flowDir,flowMode,flowLen);
 float shallowMix=clamp(TR456_WATER_SIM_SHALLOW,0.0,1.0);
 vec3 wave=mix(source,shallow,shallowMix);
 return vec3(clamp(wave.xy,vec2(-.24),vec2(.24)),clamp(wave.z,-1.25,1.25))*
   clamp(TR456_WATER_SIM_STRENGTH,0.0,2.2);
}

void emitGridVertex(vec3 bary){
 vec3 pos=gGridPos[0]*bary.x+gGridPos[1]*bary.y+gGridPos[2]*bary.z;
 vec3 worldPos=gGridWorldPos[0]*bary.x+gGridWorldPos[1]*bary.y+gGridWorldPos[2]*bary.z;
 vec3 normal=normalize(gGridNormal[0]*bary.x+gGridNormal[1]*bary.y+gGridNormal[2]*bary.z);
 vec3 light=gGridLight[0]*bary.x+gGridLight[1]*bary.y+gGridLight[2]*bary.z;
 vec3 color=gGridColor[0]*bary.x+gGridColor[1]*bary.y+gGridColor[2]*bary.z;
 vec2 rawUv=gGridUv[0]*bary.x+gGridUv[1]*bary.y+gGridUv[2]*bary.z;
 float time=uModelMatrix[3].x;
 vec4 clip=clipFromPos(pos);
 float flowMode=clamp(uTrWaterGridInfo.x,0.0,1.0);
 vec2 flowVector=uParams.xy;
 float flowLen=length(flowVector);
 float flowActive=step(.000001,flowLen);
 vec2 flowDir=(flowLen>.000001) ? flowVector/flowLen : vec2(1.0,0.0);
 vec2 authoredUv=rawUv+flowVector*time;
 vec2 gameFlowSample=authoredUv*900.0;
 vec2 simPos=mix(worldPos.xz,gameFlowSample,flowMode);
 float simTime=time*clamp(TR456_WATER_SIM_SPEED,0.15,3.0)*mix(1.0,.06,flowMode)*max(flowActive,1.0-flowMode);
 vec3 contact=contactGridWave(worldPos,clip)*TR_TOGGLE_CONTACT_RIPPLES*
   clamp(TR456_WATER_SIM_CONTACT,0.0,2.0);
 vec3 sim=simulationGridWave(simPos,simTime,flowDir,flowMode,flowLen);
 vec3 ambient=ambientGridWave(worldPos.xz,time)*clamp(TR456_WATER_RAIN_RIPPLE,0.0,2.0)*.34+
   rainCircleWave(worldPos.xz,time)*clamp(TR456_WATER_SIM_RAIN,0.0,2.0);
 vec3 wave=contact+sim+ambient;
 vec3 viewDir=normalize(-pos+normal*.0001);
 float distFade=1.0-smoothstep(5400.0,17000.0,length(pos));
 float angleFade=smoothstep(.04,.24,abs(dot(normal,viewDir)));
 float horizontal=smoothstep(.20,.74,abs(normal.y));
 float edgeFade=smoothstep(.02,.18,min(min(bary.x,bary.y),bary.z));
 float upSign=mix(-1.0,1.0,step(0.0,normal.y));
 vec3 heightDir=vec3(0.0,upSign,0.0);
 float disp=(30.0*clamp(TR456_WATER_CONTACT_VERTEX_STRENGTH,0.0,1.1)*contact.z+
   13.0*sim.z*mix(1.0,1.34,flowMode)+6.0*ambient.z)*
   clamp(TR456_WATER_GRID_STRENGTH,0.0,2.0)*
   distFade*angleFade*horizontal*mix(.18,1.0,edgeFade)*TR_TOGGLE_MESH_DISPLACEMENT;
 pos+=heightDir*disp;
 vGridWave=vec3(wave.xy*mix(1.15,1.46,flowMode),wave.z)*
   distFade*horizontal*mix(.35,1.0,edgeFade);
 vGridColor=clamp(light+color,0.0,1.8);
 vGridFog=clamp(exp(-((length(pos)/15000.0)*(length(pos)/15000.0))),0.0,1.0);
 vGridFlow=flowMode;
 gl_Position=clipFromPos(pos);
 EmitVertex();
}

void main(){
 int n=TR456_WATER_GRID_SUBDIVISION;
 if(n<1) n=1;
 if(n>8) n=8;
 float fn=float(n);

 for(int row=0;row<8;row++){
   if(row>=n) continue;
   int width=n-row;
   for(int col=0;col<8;col++){
     if(col>=width) continue;
     vec3 lower=vec3(float(n-row-col-1),float(col),float(row+1))/fn;
     vec3 upper=vec3(float(n-row-col),float(col),float(row))/fn;
     emitGridVertex(lower);
     emitGridVertex(upper);
   }
   emitGridVertex(vec3(0.0,float(width),float(row))/fn);
   EndPrimitive();
 }
}
