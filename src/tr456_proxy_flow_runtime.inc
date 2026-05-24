typedef enum {
  WATER_PROFILE_UNKNOWN=0,
  WATER_PROFILE_STANDING=1,
  WATER_PROFILE_STANDING_SURFACE=2,
  WATER_PROFILE_FLOW_SURFACE=3
} WaterDrawProfileId;

typedef struct {
  WaterDrawProfileId id;
  GLuint texture_object;
  int texture_checked;
  int texture_match;
  GLint texture_layer;
  int texture_mixed_layers;
  const char *name;
  GLfloat foam_scale;
  GLfloat opacity_scale;
  GLfloat reflection_scale;
} WaterDrawProfile;

static void flow_profile_set_surface(WaterDrawProfile *p, const char *name) {
  if(!p) return;
  p->id=WATER_PROFILE_FLOW_SURFACE;
  p->texture_object=0;
  p->texture_checked=0;
  p->texture_match=0;
  p->texture_layer=-1;
  p->texture_mixed_layers=0;
  p->name=name ? name : "flow surface";
  p->foam_scale=0.82f;
  p->opacity_scale=0.95f;
  p->reflection_scale=0.78f;
}

static void flow_profile_set_original(WaterDrawProfile *p, const char *name) {
  flow_profile_set_surface(p,name);
  if(!p) return;
  p->id=WATER_PROFILE_UNKNOWN;
  p->texture_match=0;
  p->foam_scale=0.0f;
  p->opacity_scale=0.0f;
  p->reflection_scale=0.0f;
}

static int water_draw_mode_supported(GLenum mode) {
  return mode==GL_TRIANGLES || mode==GL_TRIANGLE_STRIP || mode==GL_TRIANGLE_FAN;
}

static GLuint current_flow_texture_object(void) {
  if(g_current_program_type!=SHADER_WATER_FLOW || !g_current_program)
    return 0;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location)
    return 0;

  ProgramTrack *track=program_track(g_current_program,0);
  if(!track)
    return 0;
  if(track->flow_sampler_loc==-2) {
    track->flow_sampler_loc=gl->get_uniform_location(g_current_program,
      "sTex0_wrap");
    if(track->flow_sampler_loc<0)
      track->flow_sampler_loc=gl->get_uniform_location(g_current_program,
        "sTex0");
  }
  GLint loc=track->flow_sampler_loc;
  if(loc<0) return 0;

  GLint unit=0;
  if(!shadow_read_uniform_int(g_current_program,loc,&unit)) {
    if(!gl->get_uniform_iv) return 0;
    gl->get_uniform_iv(g_current_program,loc,&unit);
  }
  if(unit<0 || unit>31) return 0;

  GLuint cached=0;
  if(shadow_get_bound_texture(GL_TEXTURE_2D_ARRAY,unit,&cached))
    return cached;

  if(!gl->get_integer || !gl->active_texture) return 0;
  GLint old_active=GL_TEXTURE0;
  shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
  shadow_call_active_texture(gl,(GLenum)(GL_TEXTURE0+unit));
  GLint tex=0;
  gl->get_integer(GL_TEXTURE_BINDING_2D_ARRAY,&tex);
  if(tex>0)
    shadow_note_bind_texture(GL_TEXTURE_2D_ARRAY,(GLuint)tex);
  if(old_active)
    shadow_call_active_texture(gl,(GLenum)old_active);
  return tex>0 ? (GLuint)tex : 0;
}

typedef struct {
  const char *name;
  uint64_t base_hash;
  uint64_t payload_hash;
  GLsizei base_size;
  GLsizei payload_size;
} FlowTextureSignature;

static FlowTextureSignature g_flow_texture_signatures[]={
  {"1/TEX/994.DDS",0x768608304C3F878BULL,0x26DAD8984687EDFBULL,262144,349440},
  {"1/TEX/995.DDS",0xE518CFE30F854BE0ULL,0x95E6849C08FE9281ULL,262144,349440},
  {"2/TEX/2077.DDS",0x768608304C3F878BULL,0x26DAD8984687EDFBULL,262144,349440},
  {"2/TEX/2078.DDS",0x493192825E6F7160ULL,0x8D0D07DD23E1BF91ULL,262144,349440},
  {"3/TEX/226.DDS",0x768608304C3F878BULL,0x26DAD8984687EDFBULL,262144,349440},
  {"3/TEX/227.DDS",0x493192825E6F7160ULL,0x8D0D07DD23E1BF91ULL,262144,349440},
  {"3/TEX/1317.DDS",0xFFCCABEC89B2B59AULL,0x8981CD297A33039BULL,262144,349440},
  {"3/TEX/1318.DDS",0x6C2C5506E7429365ULL,0x19EA21A6029DA8A2ULL,262144,349440},
  {"3/TEX/1319.DDS",0xDEEEF2963F04984BULL,0x51EEBE9D7CE06868ULL,262144,349440},
  {"3/TEX/1320.DDS",0xB21820F721312BBFULL,0xCFB6FA688D1D546AULL,262144,349440},
  {"3/TEX/1321.DDS",0x7D69C41F6D39A0B7ULL,0xDF19A8BE89308D55ULL,262144,349440},
  {"3/TEX/1322.DDS",0x6F7D167CA7217E86ULL,0x31F4858034AE8329ULL,262144,349440},
  {"3/TEX/1323.DDS",0x085DABA69E4E5C78ULL,0xFCE33EE92387EDCDULL,262144,349440},
  {"3/TEX/1324.DDS",0xF0743510F636AB69ULL,0xFCF85836A7F0254DULL,262144,349440},
  {"3/TEX/1325.DDS",0x7DDDA576814CD4A0ULL,0xCBDE7C53572647D8ULL,262144,349440},
  {"3/TEX/1326.DDS",0x752C0DBDA1DE3EDEULL,0xEBCA6FCB79300666ULL,262144,349440},
  {"3/TEX/1327.DDS",0xA437F3B6D12D1F00ULL,0x5828801F44F2E7F0ULL,262144,349440},
  {"3/TEX/1328.DDS",0x4A06B004813D0780ULL,0x55D9F42CA06AF870ULL,262144,349440},
  {"3/TEX/1329.DDS",0xB5878783B6A9C711ULL,0xA8E664551166BC4CULL,262144,349440},
  {"3/TEX/1330.DDS",0x570ECC212541C2ABULL,0x9422B9777927B897ULL,262144,349440},
  {"3/TEX/1331.DDS",0xA64A4498EE319A0DULL,0xA6401081F034D373ULL,262144,349440},
  {"3/TEX/1332.DDS",0xD4E930689D9ACBC1ULL,0x0B9C9F3B6BE2DA77ULL,262144,349440},
  {"3/TEX/1333.DDS",0x32FA9E1BA4768978ULL,0x9649C7F64693CA47ULL,262144,349440},
  {"3/TEX/1334.DDS",0x6C204AAC23C9B4C5ULL,0x5DF1F38960A532FDULL,262144,349440},
  {"3/TEX/1335.DDS",0x867D571DB1CB556EULL,0xD62C4B4AE134EBB1ULL,262144,349440},
  {"3/TEX/1336.DDS",0x2E6FCD6C3FA0586EULL,0x86DF83C8D2C98B3DULL,262144,349440}
};

static int g_flow_texture_signatures_loaded;

static uint64_t fnv1a64_update(uint64_t h, const void *data, size_t size) {
  const unsigned char *p=(const unsigned char*)data;
  if(!p) return h;
  for(size_t i=0;i<size;i++) {
    h^=(uint64_t)p[i];
    h*=1099511628211ULL;
  }
  return h;
}

static uint64_t fnv1a64_bytes(const void *data, size_t size) {
  return fnv1a64_update(14695981039346656037ULL,data,size);
}

static void hash64_parts(uint64_t hash, unsigned int *hi, unsigned int *lo) {
  if(hi) *hi=(unsigned int)(hash>>32);
  if(lo) *lo=(unsigned int)(hash&0xFFFFFFFFu);
}

static int flow_upload_stride_interesting(size_t stride) {
  return stride==262144u || stride==349440u || stride==1048576u;
}

static size_t uncompressed_upload_size_3d(GLsizei width, GLsizei height,
                                          GLsizei depth, GLenum format,
                                          GLenum type) {
  size_t channels=0;
  if(format==GL_RGBA || format==GL_BGRA) channels=4;
  else if(format==GL_RGB || format==GL_BGR) channels=3;
  if(!channels || type!=GL_UNSIGNED_BYTE || width<=0 || height<=0 ||
     depth<=0)
    return 0;
  return (size_t)width*(size_t)height*(size_t)depth*channels;
}

static void log_flow_texture_upload_probe(const char *kind, GLuint texture,
                                          GLint level, GLint zoffset,
                                          GLsizei depth, GLsizei width,
                                          GLsizei height, GLenum format,
                                          GLenum type, size_t total_size,
                                          const void *data) {
  if(!runtime_verbose_log())
    return;
  if(!texture || !data || level!=0 || !total_size ||
     g_flow_texture_upload_probe_logged>=96u)
    return;
  if(total_size>64u*1024u*1024u)
    return;

  const size_t strides[3]={349440u,262144u,1048576u};
  const unsigned char *bytes=(const unsigned char*)data;
  int logged_chunk=0;
  for(size_t si=0;si<sizeof(strides)/sizeof(strides[0]);si++) {
    const size_t stride=strides[si];
    if(!flow_upload_stride_interesting(stride) || total_size<stride ||
       (total_size%stride)!=0)
      continue;
    const size_t chunks=total_size/stride;
    if(depth>0 && chunks!=(size_t)depth)
      continue;
    for(size_t chunk=0;chunk<chunks &&
        g_flow_texture_upload_probe_logged<96u;chunk++) {
      const size_t hash_size=262144u;
      const uint64_t h=fnv1a64_bytes(bytes+chunk*stride,hash_size);
      unsigned int hi=0,lo=0;
      hash64_parts(h,&hi,&lo);
      char msg[320];
      snprintf(msg,sizeof(msg),
        "flow texture upload probe kind=%s tex=%u level=%d layer=%d bytes=%u stride=%u head=%u hash=%08X%08X size=%dx%d fmt=0x%X type=0x%X",
        kind ? kind : "upload",texture,(int)level,
        (int)(zoffset+(GLint)chunk),(unsigned int)total_size,
        (unsigned int)stride,(unsigned int)hash_size,hi,lo,
        (int)width,(int)height,(unsigned int)format,(unsigned int)type);
      log_line(msg);
      g_flow_texture_upload_probe_logged++;
      logged_chunk=1;
    }
    if(logged_chunk)
      return;
  }

  if(g_flow_texture_upload_probe_logged>=96u)
    return;
  if(total_size<262144u && !(width==512 && height==512))
    return;
  const size_t hash_size=total_size<262144u ? total_size : 262144u;
  const uint64_t h=fnv1a64_bytes(data,hash_size);
  unsigned int hi=0,lo=0;
  hash64_parts(h,&hi,&lo);
  char msg[320];
  snprintf(msg,sizeof(msg),
    "flow texture upload probe kind=%s tex=%u level=%d layer=%d depth=%d bytes=%u head=%u hash=%08X%08X size=%dx%d fmt=0x%X type=0x%X",
    kind ? kind : "upload",texture,(int)level,(int)zoffset,(int)depth,
    (unsigned int)total_size,(unsigned int)hash_size,hi,lo,
    (int)width,(int)height,(unsigned int)format,(unsigned int)type);
  log_line(msg);
  g_flow_texture_upload_probe_logged++;
}

static int dds_payload_offset(const unsigned char *data, DWORD size) {
  if(!data || size<128) return 0;
  if(data[0]!='D' || data[1]!='D' || data[2]!='S' || data[3]!=' ')
    return 0;
  if(size>=148 && data[84]=='D' && data[85]=='X' &&
     data[86]=='1' && data[87]=='0')
    return 148;
  return 128;
}

static void load_flow_texture_signatures_from_files(void) {
  if(g_flow_texture_signatures_loaded) return;
  g_flow_texture_signatures_loaded=1;
  load_runtime_config();
  if(!g_runtime_refresh_flow_texture_signatures) return;
  if(!g_dir[0]) return;

  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    FlowTextureSignature *s=&g_flow_texture_signatures[i];
    char path[MAX_PATH];
    if(!join_game_path(path,s->name)) continue;

    HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
      0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(h==INVALID_HANDLE_VALUE) continue;
    DWORD size=GetFileSize(h,0);
    if(size==INVALID_FILE_SIZE || size<148 || size>2*1024*1024) {
      CloseHandle(h);
      continue;
    }
    unsigned char *buf=(unsigned char*)malloc(size);
    if(!buf) {
      CloseHandle(h);
      continue;
    }
    DWORD got=0;
    int ok=ReadFile(h,buf,size,&got,0) && got==size;
    CloseHandle(h);
    if(ok) {
      int offset=dds_payload_offset(buf,size);
      if(offset>0 && (DWORD)offset<size) {
        DWORD payload=size-(DWORD)offset;
        DWORD base=payload<262144u ? payload : 262144u;
        s->base_size=(GLsizei)base;
        s->base_hash=fnv1a64_bytes(buf+offset,base);
        s->payload_size=(GLsizei)payload;
        s->payload_hash=fnv1a64_bytes(buf+offset,payload);
        if(g_flow_surface_texture_logged<16u) {
          char msg[256];
          snprintf(msg,sizeof(msg),
            "flow texture signature loaded path=%s base=%d payload=%d",
            s->name,(int)s->base_size,(int)s->payload_size);
          log_line(msg);
          g_flow_surface_texture_logged++;
        }
      }
    }
    free(buf);
  }
}

static const char *flow_texture_signature_match(GLsizei image_size,
                                                const void *data) {
  load_flow_texture_signatures_from_files();
  if(!data || image_size<=0) return 0;
  GLsizei cached_size[4]={0,0,0,0};
  uint64_t cached_hash[4]={0,0,0,0};
  int cached_count=0;
  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    const FlowTextureSignature *s=&g_flow_texture_signatures[i];
    GLsizei sizes[2]={s->base_size,s->payload_size};
    uint64_t hashes[2]={s->base_hash,s->payload_hash};
    for(int j=0;j<2;j++) {
      GLsizei size=sizes[j];
      if(size<=0 || image_size<size) continue;
      int cache_index=-1;
      for(int k=0;k<cached_count;k++) {
        if(cached_size[k]==size) {
          cache_index=k;
          break;
        }
      }
      if(cache_index<0) {
        if(cached_count>=4) continue;
        cache_index=cached_count++;
        cached_size[cache_index]=size;
        cached_hash[cache_index]=fnv1a64_bytes(data,(size_t)size);
      }
      if(cached_hash[cache_index]==hashes[j])
        return s->name;
    }
  }
  return 0;
}

static GLuint current_bound_texture_for_target(GLenum target) {
  CaptureGL *gl=capture_gl();
  int unit=shadow_active_unit_index();
  GLuint cached=0;
  if(unit>=0 && shadow_get_bound_texture(target,unit,&cached))
    return cached;
  if(!gl || !gl->get_integer) return 0;
  GLint binding=0;
  if(target==GL_TEXTURE_2D_ARRAY)
    gl->get_integer(GL_TEXTURE_BINDING_2D_ARRAY,&binding);
  else if(target==GL_TEXTURE_2D)
    gl->get_integer(GL_TEXTURE_BINDING_2D,&binding);
  if(target==GL_TEXTURE_2D_ARRAY || target==GL_TEXTURE_2D)
    shadow_note_bind_texture(target,binding>0 ? (GLuint)binding : 0u);
  return binding>0 ? (GLuint)binding : 0;
}

static int flow_surface_texture_known(GLuint texture) {
  if(!texture) return 0;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_objects)/
      sizeof(g_flow_surface_texture_objects[0]);i++) {
    if(g_flow_surface_texture_objects[i]==texture)
      return 1;
  }
  return 0;
}

static int flow_surface_texture_layer_known(GLuint texture, GLint layer) {
  if(!texture || layer<0) return 0;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_layers)/
      sizeof(g_flow_surface_texture_layers[0]);i++) {
    FlowTextureLayer *l=&g_flow_surface_texture_layers[i];
    if(l->texture==texture && l->layer==layer)
      return 1;
  }
  return 0;
}

static int flow_surface_texture_has_precise_layers(GLuint texture) {
  if(!texture) return 0;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_layers)/
      sizeof(g_flow_surface_texture_layers[0]);i++) {
    FlowTextureLayer *l=&g_flow_surface_texture_layers[i];
    if(l->texture==texture && l->layer>=0)
      return 1;
  }
  return 0;
}

static void remember_flow_surface_texture(GLuint texture, const char *name) {
  if(!texture || flow_surface_texture_known(texture)) return;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_objects)/
      sizeof(g_flow_surface_texture_objects[0]);i++) {
    if(!g_flow_surface_texture_objects[i]) {
      g_flow_surface_texture_objects[i]=texture;
      if(g_flow_surface_texture_logged<24u) {
        char msg[192];
        snprintf(msg,sizeof(msg),
          "flow water texture matched tex=%u signature=%s",
          texture,name ? name : "water");
        log_line(msg);
        g_flow_surface_texture_logged++;
      }
      return;
    }
  }
}

static void remember_flow_surface_texture_layer(GLuint texture, GLint layer,
                                                const char *name) {
  if(!texture || layer<0) return;
  if(flow_surface_texture_layer_known(texture,layer)) return;
  for(size_t i=0;i<sizeof(g_flow_surface_texture_layers)/
      sizeof(g_flow_surface_texture_layers[0]);i++) {
    FlowTextureLayer *l=&g_flow_surface_texture_layers[i];
    if(!l->texture) {
      l->texture=texture;
      l->layer=layer;
      l->name=name;
      if(g_flow_surface_texture_logged<48u) {
        char msg[224];
        snprintf(msg,sizeof(msg),
          "flow water texture layer matched tex=%u layer=%d signature=%s",
          texture,(int)layer,name ? name : "water");
        log_line(msg);
        g_flow_surface_texture_logged++;
      }
      return;
    }
  }
}

static const char *flow_texture_signature_match_exact(GLsizei image_size,
                                                      const void *data) {
  load_flow_texture_signatures_from_files();
  if(!data || image_size<=0) return 0;
  uint64_t h=0;
  int hashed=0;
  for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
      sizeof(g_flow_texture_signatures[0]);i++) {
    const FlowTextureSignature *s=&g_flow_texture_signatures[i];
    if(s->base_size==image_size) {
      if(!hashed) {
        h=fnv1a64_bytes(data,(size_t)image_size);
        hashed=1;
      }
      if(h==s->base_hash)
        return s->name;
    }
    if(s->payload_size==image_size) {
      if(!hashed) {
        h=fnv1a64_bytes(data,(size_t)image_size);
        hashed=1;
      }
      if(h==s->payload_hash)
        return s->name;
    }
  }
  return 0;
}

static void note_compressed_texture_upload_for_texture(GLuint texture,
                                                       GLenum target,
                                                       GLint level,
                                                       GLint zoffset,
                                                       GLsizei depth,
                                                       GLsizei image_size,
                                                       const void *data) {
  load_flow_texture_signatures_from_files();
  if(!texture) return;
  if(target==GL_TEXTURE_2D_ARRAY && level==0 && depth>0 &&
     image_size>0 && data) {
    const unsigned char *p=(const unsigned char*)data;
    GLsizei strides[8];
    int stride_count=0;
    for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
        sizeof(g_flow_texture_signatures[0]);i++) {
      const FlowTextureSignature *s=&g_flow_texture_signatures[i];
      GLsizei candidates[2]={s->payload_size,s->base_size};
      for(int ci=0;ci<2 && stride_count<8;ci++) {
        GLsizei candidate=candidates[ci];
        int known=0;
        if(candidate<=0) continue;
        for(int si=0;si<stride_count;si++) {
          if(strides[si]==candidate) {
            known=1;
            break;
          }
        }
        if(!known)
          strides[stride_count++]=candidate;
      }
    }
    for(int si=0;si<stride_count;si++) {
      GLsizei stride=strides[si];
      if(image_size<stride || (image_size%stride)!=0)
        continue;
      int chunks=image_size/stride;
      for(int chunk=0;chunk<chunks;chunk++) {
        const unsigned char *chunk_data=p+(size_t)chunk*(size_t)stride;
        uint64_t full_hash=fnv1a64_bytes(chunk_data,(size_t)stride);
        uint64_t base_hash=0;
        int have_base=0;
        for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
            sizeof(g_flow_texture_signatures[0]);i++) {
          const FlowTextureSignature *s=&g_flow_texture_signatures[i];
          int matched=0;
          if(stride==s->payload_size && full_hash==s->payload_hash)
            matched=1;
          if(!matched && stride==s->base_size && full_hash==s->base_hash)
            matched=1;
          if(!matched && stride==s->payload_size && s->base_size>0 &&
             s->base_size<=stride) {
            if(!have_base) {
              base_hash=fnv1a64_bytes(chunk_data,(size_t)s->base_size);
              have_base=1;
            }
            matched=base_hash==s->base_hash;
          }
          if(matched) {
            GLint layer=zoffset;
            if(chunks==depth)
              layer=zoffset+chunk;
            remember_flow_surface_texture_layer(texture,layer,s->name);
          }
        }
      }
    }
    if(!flow_surface_texture_known(texture)) {
      uint64_t base_hash=0;
      int have_base=0;
      for(size_t i=0;i<sizeof(g_flow_texture_signatures)/
          sizeof(g_flow_texture_signatures[0]);i++) {
        const FlowTextureSignature *s=&g_flow_texture_signatures[i];
        if(s->base_size>0 && image_size>s->base_size) {
          if(!have_base) {
            base_hash=fnv1a64_bytes(data,(size_t)s->base_size);
            have_base=1;
          }
          if(base_hash==s->base_hash)
            remember_flow_surface_texture_layer(texture,zoffset,s->name);
        }
      }
    }
    if(!flow_surface_texture_known(texture) &&
       !flow_surface_texture_has_precise_layers(texture))
      log_flow_texture_upload_probe("compressed",texture,level,zoffset,depth,
        0,0,0,0,(size_t)image_size,data);
    return;
  }

  const char *match=flow_texture_signature_match_exact(image_size,data);
  if(match) {
    if(target==GL_TEXTURE_2D_ARRAY && level==0)
      remember_flow_surface_texture_layer(texture,zoffset,match);
    else
      remember_flow_surface_texture(texture,match);
  } else {
    match=flow_texture_signature_match(image_size,data);
    if(match)
      remember_flow_surface_texture(texture,match);
    else if(target==GL_TEXTURE_2D_ARRAY && level==0 && image_size>0)
      log_flow_texture_upload_probe("compressed",texture,level,zoffset,depth,
        0,0,0,0,(size_t)image_size,data);
  }
}

static void note_compressed_texture_upload(GLenum target, GLint level,
                                           GLint zoffset, GLsizei depth,
                                           GLsizei image_size,
                                           const void *data) {
  note_compressed_texture_upload_for_texture(
    current_bound_texture_for_target(target),target,level,zoffset,depth,
    image_size,data);
}

static void note_uncompressed_texture_upload_for_texture(GLuint texture,
                                                         GLenum target,
                                                         GLint level,
                                                         GLint zoffset,
                                                         GLsizei width,
                                                         GLsizei height,
                                                         GLsizei depth,
                                                         GLenum format,
                                                         GLenum type,
                                                         const void *data) {
  if(target!=GL_TEXTURE_2D_ARRAY || level!=0 || !texture || !data)
    return;
  const size_t total=uncompressed_upload_size_3d(width,height,depth,format,
    type);
  if(!total)
    return;
  log_flow_texture_upload_probe("uncompressed",texture,level,zoffset,depth,
    width,height,format,type,total,data);
}

static void note_uncompressed_texture_upload(GLenum target, GLint level,
                                             GLint zoffset, GLsizei width,
                                             GLsizei height, GLsizei depth,
                                             GLenum format, GLenum type,
                                             const void *data) {
  note_uncompressed_texture_upload_for_texture(
    current_bound_texture_for_target(target),target,level,zoffset,width,
    height,depth,format,type,data);
}

static int current_water_attrib_locations(GLint locs[4]);

static int flow_params_are_authored_surface(const GLfloat params[4]) {
  return params && fabsf(params[2])<0.00030f && fabsf(params[3])>=40.0f;
}

static int current_flow_draw_matches_texture_layers(GLuint texture,
                                                    GLsizei count,
                                                    int count_known,
                                                    GLint *layer_out,
                                                    int *mixed_out) {
  if(layer_out) *layer_out=-1;
  if(mixed_out) *mixed_out=0;
  (void)count;
  (void)count_known;
  if(!texture)
    return 0;
  const int precise_layers=flow_surface_texture_has_precise_layers(texture);
  if(mixed_out && precise_layers)
    *mixed_out=1;
  if(precise_layers)
    return 0;
  return flow_surface_texture_known(texture);
}

static int g_flow_profile_cache_valid;
static unsigned int g_flow_profile_cache_frame;
static unsigned int g_flow_profile_cache_draw_serial;
static GLuint g_flow_profile_cache_program;
static GLenum g_flow_profile_cache_mode;
static GLsizei g_flow_profile_cache_count;
static int g_flow_profile_cache_count_known;
static int g_flow_profile_cache_has_params;
static GLfloat g_flow_profile_cache_params[4];
static WaterDrawProfile g_flow_profile_cache_profile;

static unsigned int current_draw_serial(void) {
  ProgramTrack *p=program_track(g_current_program,0);
  return p ? p->frame_draw_count : 0u;
}

static int current_flow_draw_profile(GLenum mode, GLsizei count,
                                     int count_known,
                                     WaterDrawProfile *profile,
                                     GLfloat params[4]) {
  load_runtime_config();
  if(g_current_program_type!=SHADER_WATER_FLOW) return 0;
  const unsigned int serial=current_draw_serial();
  if(!g_flow_profile_cache_valid ||
     g_flow_profile_cache_frame!=g_frame_index ||
     g_flow_profile_cache_draw_serial!=serial ||
     g_flow_profile_cache_program!=g_current_program ||
     g_flow_profile_cache_mode!=mode ||
     g_flow_profile_cache_count!=count ||
     g_flow_profile_cache_count_known!=count_known) {
    GLfloat local_params[4]={0.0f,0.0f,0.0f,0.0f};
    int has_params=read_uniform_vec4_now("uParams",local_params);
    if(has_params) {
      GLuint tex=current_flow_texture_object();
      GLint texture_layer=-1;
      int mixed_layers=0;
      int texture_match=
        current_flow_draw_matches_texture_layers(tex,count,count_known,
          &texture_layer,&mixed_layers);
      if(texture_match) {
        flow_profile_set_surface(&g_flow_profile_cache_profile,
          "flow surface texture");
      } else if(g_runtime_flow_texture_fallback &&
                flow_params_are_authored_surface(local_params)) {
        flow_profile_set_surface(&g_flow_profile_cache_profile,
          "flow material fallback");
        texture_match=2;
      } else {
        flow_profile_set_original(&g_flow_profile_cache_profile,
          "non-flow texture");
      }
      g_flow_profile_cache_profile.texture_object=tex;
      g_flow_profile_cache_profile.texture_checked=1;
      g_flow_profile_cache_profile.texture_match=texture_match;
      g_flow_profile_cache_profile.texture_layer=texture_layer;
      g_flow_profile_cache_profile.texture_mixed_layers=mixed_layers;
      if(texture_match && runtime_verbose_log()) {
        if(g_flow_surface_confirmed_logged<24u) {
          char msg[320];
          snprintf(msg,sizeof(msg),
            "flow surface texture confirmed frame=%u program=%u count=%d tex=%u layer=%d mixed=%d precise=%d params=(%.6f %.6f %.6f %.6f)",
            g_frame_index,g_current_program,(int)count,tex,
            (int)texture_layer,mixed_layers,texture_match,
            (double)local_params[0],(double)local_params[1],
            (double)local_params[2],(double)local_params[3]);
          log_line(msg);
          g_flow_surface_confirmed_logged++;
        }
      }
      if(!texture_match && runtime_verbose_log() &&
         g_flow_material_bypass_logged<32u) {
        char msg[320];
        snprintf(msg,sizeof(msg),
          "flow original by texture frame=%u program=%u count=%d tex=%u layer=%d knownTex=%d preciseLayers=%d params=(%.6f %.6f %.6f %.6f)",
          g_frame_index,g_current_program,(int)count,tex,(int)texture_layer,
          flow_surface_texture_known(tex),
          flow_surface_texture_has_precise_layers(tex),
          (double)local_params[0],(double)local_params[1],
          (double)local_params[2],(double)local_params[3]);
        log_line(msg);
        g_flow_material_bypass_logged++;
      }
    } else {
      WaterDrawProfile fallback;
      flow_profile_set_original(&fallback,"unclassified flow");
      g_flow_profile_cache_profile=fallback;
    }
    memcpy(g_flow_profile_cache_params,local_params,
      sizeof(g_flow_profile_cache_params));
    g_flow_profile_cache_has_params=has_params;
    g_flow_profile_cache_frame=g_frame_index;
    g_flow_profile_cache_draw_serial=serial;
    g_flow_profile_cache_program=g_current_program;
    g_flow_profile_cache_mode=mode;
    g_flow_profile_cache_count=count;
    g_flow_profile_cache_count_known=count_known;
    g_flow_profile_cache_valid=1;
  }
  if(profile) *profile=g_flow_profile_cache_profile;
  if(params) memcpy(params,g_flow_profile_cache_params,
    sizeof(g_flow_profile_cache_params));
  return g_flow_profile_cache_has_params;
}

static int current_flow_draw_is_allowlisted_surface(GLsizei count,
                                                    int count_known,
                                                    const GLfloat params[4],
                                                    const WaterDrawProfile *profile,
                                                    const char **reason_out);

static void update_flow_material_profile_uniform(GLenum mode, GLsizei count,
                                                 int count_known) {
  if(g_current_program_type!=SHADER_WATER_FLOW) return;
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p || !p->type) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->uniform_4f) return;
  if(p->material_profile_loc==-2)
    p->material_profile_loc=gl->get_uniform_location(g_current_program,
      "uTrWaterMaterialProfile");
  if(p->material_profile_loc<0) return;

  GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
  WaterDrawProfile profile;
  current_flow_draw_profile(mode,count,count_known,&profile,params);
  const int allowlisted=current_flow_draw_is_allowlisted_surface(
    count,count_known,params,&profile,0);
  shadow_call_uniform_4f(gl,p->material_profile_loc,
    allowlisted ? (GLfloat)profile.id : (GLfloat)WATER_PROFILE_UNKNOWN,
    allowlisted ? 0.0f : 1.0f,
    allowlisted ? profile.foam_scale : 0.0f,
    allowlisted ? profile.reflection_scale : 0.0f);
}

static void update_flow_inplace_fx_uniforms(void) {
  if(g_current_program_type!=SHADER_WATER_FLOW || !g_runtime_flow_inplace_patch)
    return;
  ProgramTrack *p=program_track(g_current_program,0);
  if(!p || !p->type) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->get_uniform_location || !gl->uniform_4f) return;
  static const char *names[4]={
    "uTrWaterFlowFx0","uTrWaterFlowFx1","uTrWaterFlowFx2",
    "uTrWaterFlowFx3"
  };
  for(int i=0;i<4;i++) {
    if(p->flow_fx_loc[i]==-2)
      p->flow_fx_loc[i]=gl->get_uniform_location(g_current_program,names[i]);
  }
  if(p->flow_fx_loc[0]>=0)
    shadow_call_uniform_4f(gl,p->flow_fx_loc[0],
      g_runtime_flow_fx0[0],g_runtime_flow_fx0[1],
      g_runtime_flow_fx0[2],g_runtime_flow_fx0[3]);
  if(p->flow_fx_loc[1]>=0)
    shadow_call_uniform_4f(gl,p->flow_fx_loc[1],
      g_runtime_flow_fx1[0],g_runtime_flow_fx1[1],
      g_runtime_flow_fx1[2],g_runtime_flow_fx1[3]);
  if(p->flow_fx_loc[2]>=0)
    shadow_call_uniform_4f(gl,p->flow_fx_loc[2],
      g_runtime_flow_fx2[0],g_runtime_flow_fx2[1],
      g_runtime_flow_fx2[2],g_runtime_flow_fx2[3]);
  if(p->flow_fx_loc[3]>=0)
    shadow_call_uniform_4f(gl,p->flow_fx_loc[3],
      g_runtime_flow_fx3[0],g_runtime_flow_fx3[1],
      g_runtime_flow_fx3[2],g_runtime_flow_fx3[3]);
}

static int current_flow_draw_is_allowlisted_surface(GLsizei count,
                                                    int count_known,
                                                    const GLfloat params[4],
                                                    const WaterDrawProfile *profile,
                                                    const char **reason_out) {
  (void)count;
  (void)count_known;
  if(!profile || !profile->texture_match) {
    if(reason_out) *reason_out=profile && profile->name ?
      profile->name : "flow texture";
    return 0;
  }

  const int authored_surface_wave=flow_params_are_authored_surface(params);
  const int allowlisted=authored_surface_wave && profile->texture_match;

  if(!allowlisted && reason_out) {
    *reason_out=!authored_surface_wave ? "flow material" : "unknown";
  }
  return allowlisted;
}

static int current_flow_draw_allows_inplace_effects(GLenum mode, GLsizei count,
                                                    int count_known) {
  if(g_current_program_type!=SHADER_WATER_FLOW || !g_runtime_flow_inplace_patch)
    return 0;
  GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
  WaterDrawProfile profile;
  if(!current_flow_draw_profile(mode,count,count_known,&profile,params))
    return 0;
  return current_flow_draw_is_allowlisted_surface(count,count_known,params,
    &profile,0);
}

static int current_draw_is_synthetic_flow_candidate_raw(GLenum mode, GLsizei count,
                                                        int count_known) {
  load_runtime_config();
  if(!g_runtime_synthetic_surface || !g_runtime_shader_patching) return 0;
  if(!g_runtime_synthetic_flow_surface) return 0;
  if(g_runtime_flow_inplace_patch &&
     !g_runtime_synthetic_flow_replace_original &&
     !g_runtime_flow_lite_surface) {
    if(runtime_verbose_log() && g_flow_surface_gate_logged<48u) {
      char msg[192];
      snprintf(msg,sizeof(msg),
        "flow surface gate skipped: FlowInPlacePatch handles original pass frame=%u program=%u count=%d",
        g_frame_index,g_current_program,(int)count);
      log_line(msg);
      g_flow_surface_gate_logged++;
    }
    return 0;
  }
  if(g_current_program_type!=SHADER_WATER_FLOW) return 0;
  if(effect_toggle_value(0)<=0.001f) {
    if(runtime_verbose_log() && g_flow_surface_gate_logged<48u) {
      char msg[192];
      snprintf(msg,sizeof(msg),
        "flow surface gate disabled by Ctrl+J+1 frame=%u program=%u count=%d",
        g_frame_index,g_current_program,(int)count);
      log_line(msg);
      g_flow_surface_gate_logged++;
    }
    return 0;
  }
  if(count_known && (count<3 || count>262144)) return 0;
  if(!water_draw_mode_supported(mode)) return 0;
  GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
  WaterDrawProfile profile;
  int has_params=current_flow_draw_profile(mode,count,count_known,&profile,
    params);
  if(!has_params)
    return 0;
  const char *reason="unknown";
  const int allowlisted=current_flow_draw_is_allowlisted_surface(count,
    count_known,params,&profile,&reason);
  if(!allowlisted && runtime_verbose_log() && g_flow_surface_gate_logged<48u) {
    char msg[384];
    snprintf(msg,sizeof(msg),
      "flow surface gate original frame=%u program=%u count=%d reason=%s params=(%.6f %.6f %.6f %.6f)",
      g_frame_index,g_current_program,(int)count,
      reason,
      (double)params[0],(double)params[1],
      (double)params[2],(double)params[3]);
    log_line(msg);
    g_flow_surface_gate_logged++;
  }
  return allowlisted;
}

static int current_draw_is_synthetic_standing_candidate_raw(GLenum mode, GLsizei count,
                                                            int count_known) {
  load_runtime_config();
  if(!g_runtime_synthetic_surface || !g_runtime_shader_patching) return 0;
  if(count_known && (count<3 || count>262144)) return 0;
  if(mode!=GL_TRIANGLES && mode!=GL_TRIANGLE_STRIP && mode!=GL_TRIANGLE_FAN)
    return 0;
  if(g_current_program_type!=SHADER_WATER_SURFACE &&
     g_current_program_type!=SHADER_WATER_REFLECT)
    return 0;
  if(g_current_program_type==SHADER_WATER_SURFACE &&
     g_runtime_synthetic_standing_bounds_guard && !count_known)
    return 0;

  GLfloat model3[4];
  int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
  if(g_current_program_type==SHADER_WATER_SURFACE) {
    GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
    const int has_params=read_uniform_vec4_now("uParams",params);
    if(!has_model3 || !has_params || fabsf(params[3])<=0.0001f)
      return 0;
    return 1;
  }
  if(!g_runtime_synthetic_reflect_surface) return 0;
  if(!count_known) return 0;
  if(mode!=GL_TRIANGLES || count>2048 || (count%3)!=0) return 0;
  return has_model3;
}

static const SyntheticDrawDecision *current_synthetic_draw_decision(GLenum mode,
                                                                    GLsizei count,
                                                                    int count_known) {
  const unsigned int serial=current_draw_serial();
  SyntheticDrawDecision *d=&g_synthetic_draw_decision_cache;
  if(d->valid &&
     d->frame==g_frame_index &&
     d->serial==serial &&
     d->program==g_current_program &&
     d->program_type==g_current_program_type &&
     d->mode==mode &&
     d->count==count &&
     d->count_known==count_known) {
    perf_note_decision(1,d,0);
    return d;
  }

  unsigned long long t0=perf_ticks_now();
  memset(d,0,sizeof(*d));
  d->valid=1;
  d->frame=g_frame_index;
  d->serial=serial;
  d->program=g_current_program;
  d->program_type=g_current_program_type;
  d->mode=mode;
  d->count=count;
  d->count_known=count_known;
  switch(g_current_program_type) {
    case SHADER_WATER_FLOW:
      d->synthetic_flow=
        current_draw_is_synthetic_flow_candidate_raw(mode,count,count_known);
      d->synthetic_flow_lite=d->synthetic_flow && g_runtime_flow_lite_surface;
      break;
    case SHADER_WATER_SURFACE:
    case SHADER_WATER_REFLECT:
      d->synthetic_standing=
        current_draw_is_synthetic_standing_candidate_raw(mode,count,count_known);
      break;
    default:
      break;
  }
  d->synthetic_surface=d->synthetic_standing || d->synthetic_flow;
  if(d->synthetic_surface)
    d->synthetic_ready=d->synthetic_flow_lite ?
      ensure_flow_lite_surface_program() :
      ensure_synthetic_surface_program();
  load_runtime_config();
  int skip_ready=d->synthetic_ready ||
    (g_current_program_type==SHADER_WATER_SSR && g_synthetic_surface.ready);
  if(g_runtime_synthetic_surface &&
     g_runtime_shader_patching && skip_ready) {
    if(g_runtime_synthetic_standing_replace_original &&
       (g_current_program_type==SHADER_WATER_SURFACE ||
        g_current_program_type==SHADER_WATER_REFLECT) &&
       d->synthetic_standing)
      d->skip_original=1;
    else if(g_runtime_synthetic_flow_replace_original &&
            d->synthetic_flow && d->synthetic_ready)
      d->skip_original=1;
    else if(g_runtime_synthetic_standing_replace_original &&
            g_current_program_type==SHADER_WATER_SSR)
      d->skip_original=1;
  }
  d->capture_reason=d->synthetic_flow ? "synthetic flow surface" :
    "synthetic water surface";
  perf_note_decision(0,d,perf_ticks_elapsed(t0));
  return d;
}

static void prepare_program_frame_draw_state(ProgramTrack *p) {
  if(!p) return;
  if(p->frame_draw_frame!=g_frame_index) {
    p->frame_draw_frame=g_frame_index;
    p->frame_draw_count=0;
    p->frame_last_mode=0;
    p->frame_last_count=-2147483647;
    p->current_duplicate_pass=0;
  }
}

static void note_draw(const char *name, GLenum mode, GLsizei count) {
  poll_effect_hotkeys();
  diag_poll_insert(name);
  tr456_wet_lara_note_draw(name,count,
    count>=0 && !(name && strncmp(name,"glMultiDraw",11)==0));
  const int tracked_water=is_tracked_water_shader_type(g_current_program_type);
  perf_note_draw(tracked_water);
  ProgramTrack *p=program_track(g_current_program,
    tracked_water || runtime_verbose_log());
  if(g_current_program_type==SHADER_WATER_REFLECT)
    update_contact_cache_from_program(g_current_program);
  if(tracked_water) {
    apply_effect_toggles(g_current_program);
    apply_contact_cache(g_current_program);
    update_draw_info_uniform(mode,count);
    update_flow_material_profile_uniform(mode,count,count>=0);
    update_flow_inplace_fx_uniforms();
    if(g_current_program_type==SHADER_WATER_FLOW &&
       current_flow_draw_allows_inplace_effects(mode,count,count>=0))
      prepare_scene_capture_for_flow_inplace();
    if(runtime_verbose_log() &&
       g_current_program_type>0 && g_current_program_type<6 &&
       g_water_draw_logged_by_type[g_current_program_type]<10u) {
      GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat toggle0[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat toggle1[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat toggle2[4]={0.0f,0.0f,0.0f,0.0f};
      GLfloat draw_info[4]={0.0f,0.0f,0.0f,0.0f};
      int has_params=read_uniform_vec4_now("uParams",params);
      int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
      int has_toggle0=read_uniform_vec4_now("uTrWaterToggle0",toggle0);
      int has_toggle1=read_uniform_vec4_now("uTrWaterToggle1",toggle1);
      int has_toggle2=read_uniform_vec4_now("uTrWaterToggle2",toggle2);
      int has_draw_info=read_uniform_vec4_now("uTrWaterDrawInfo",draw_info);
      char msg[640];
      snprintf(msg,sizeof(msg),
        "water draw frame=%u call=%s program=%u type=%s mode=0x%X count=%d frame_draw=%u dup=%d params%s=(%.6f %.6f %.6f %.6f) time%s=%.3f toggles%s/%s/%s=(%.0f%.0f%.0f%.0f %.0f%.0f%.0f%.0f %.0f%.0f%.0f%.0f) drawInfo%s=(%.0f %.0f %.0f %.0f)",
        g_frame_index,name ? name : "draw",g_current_program,
        shader_type_name(g_current_program_type),(unsigned int)mode,(int)count,
        p ? p->frame_draw_count+1u : 0u,p ? p->current_duplicate_pass : 0,
        has_params ? "" : "?",
        (double)params[0],(double)params[1],(double)params[2],(double)params[3],
        has_model3 ? "" : "?",(double)model3[0],
        has_toggle0 ? "" : "?",has_toggle1 ? "" : "?",has_toggle2 ? "" : "?",
        (double)toggle0[0],(double)toggle0[1],(double)toggle0[2],(double)toggle0[3],
        (double)toggle1[0],(double)toggle1[1],(double)toggle1[2],(double)toggle1[3],
        (double)toggle2[0],(double)toggle2[1],(double)toggle2[2],(double)toggle2[3],
        has_draw_info ? "" : "?",
        (double)draw_info[0],(double)draw_info[1],(double)draw_info[2],(double)draw_info[3]);
      log_line(msg);
      g_water_draw_logged_by_type[g_current_program_type]++;
    }
  }
  update_ripple_draw_info(count);
  if(g_current_program_type==SHADER_WATER_RIPPLE) {
    const int ripple_contact_candidate=is_water_ripple_draw_count(count);
    unsigned long long perf_contact_t0=perf_ticks_now();
    record_ripple_contact_from_program(count);
    perf_note_ripple_contact(ripple_contact_candidate,
      perf_ticks_elapsed(perf_contact_t0));
    diag_log_cpu_contact_state(name);
  }
  if(tracked_water)
    diag_log_contacts(g_current_program,name);
  if(p) {
    prepare_program_frame_draw_state(p);
    p->last_frame=g_frame_index;
    p->draw_count++;
    p->frame_draw_count++;
    p->frame_last_mode=mode;
    p->frame_last_count=(int)count;
    p->last_mode=mode;
    p->last_count=(int)count;
  }
  if(g_diag_active_frames>0 && g_diag_lines_left>0) {
    char shaders[256];
    program_shader_summary(p,shaders,sizeof(shaders));
    GLint blend=-1;
    GLint depth=-1;
    GLint depth_mask=-1;
    GLint blend_src_rgb=-1;
    GLint blend_dst_rgb=-1;
    GLint blend_src_alpha=-1;
    GLint blend_dst_alpha=-1;
    CaptureGL *gl=capture_gl();
    if(gl->get_integer) {
      shadow_get_integer_or_gl(GL_BLEND,&blend);
      shadow_get_integer_or_gl(GL_DEPTH_TEST,&depth);
      shadow_get_integer_or_gl(GL_DEPTH_WRITEMASK,&depth_mask);
      shadow_get_integer_or_gl(GL_BLEND_SRC_RGB,&blend_src_rgb);
      shadow_get_integer_or_gl(GL_BLEND_DST_RGB,&blend_dst_rgb);
      shadow_get_integer_or_gl(GL_BLEND_SRC_ALPHA,&blend_src_alpha);
      shadow_get_integer_or_gl(GL_BLEND_DST_ALPHA,&blend_dst_alpha);
    }
    GLfloat params[4]={0.0f,0.0f,0.0f,0.0f};
    GLfloat model3[4]={0.0f,0.0f,0.0f,0.0f};
    GLfloat draw_info[4]={0.0f,0.0f,0.0f,0.0f};
    int has_params=read_uniform_vec4_now("uParams",params);
    int has_model3=read_uniform_vec4_index_now("uModelMatrix",3,model3);
    int has_draw_info=read_uniform_vec4_now("uTrWaterDrawInfo",draw_info);
    char msg[1024];
    snprintf(msg,sizeof(msg),
      "diag draw session=%d frame=%u call=%s program=%u type=%s mode=0x%X count=%d blend=%d depth=%d depthMask=%d blendRgb=0x%X/0x%X blendA=0x%X/0x%X params%s=(%.4f %.4f %.4f %.4f) model3%s=(%.1f %.1f %.1f %.1f) drawInfo%s=(%.0f %.0f %.0f %.0f) shaders=[%s]",
      g_diag_session,g_frame_index,name ? name : "draw",g_current_program,
      shader_type_name(p ? p->type : 0),(unsigned int)mode,(int)count,blend,depth,
      depth_mask,(unsigned int)blend_src_rgb,(unsigned int)blend_dst_rgb,
      (unsigned int)blend_src_alpha,(unsigned int)blend_dst_alpha,
      has_params ? "" : "?",(double)params[0],(double)params[1],
      (double)params[2],(double)params[3],
      has_model3 ? "" : "?",(double)model3[0],(double)model3[1],
      (double)model3[2],(double)model3[3],
      has_draw_info ? "" : "?",(double)draw_info[0],(double)draw_info[1],
      (double)draw_info[2],(double)draw_info[3],shaders);
    log_line(msg);
    diag_consume_line();
  }
}

static CaptureGL *capture_gl(void) {
  if(g_capture_gl.tried) return &g_capture_gl;
  g_capture_gl.tried=1;
  g_capture_gl.get_integer=(PFNGLGETINTEGERV)gl_proc("glGetIntegerv");
  g_capture_gl.gen_textures=(PFNGLGENTEXTURES)gl_proc("glGenTextures");
  g_capture_gl.bind_texture=(PFNGLBINDTEXTURE)gl_proc("glBindTexture");
  g_capture_gl.tex_parameter_i=(PFNGLTEXPARAMETERI)gl_proc("glTexParameteri");
  g_capture_gl.tex_image_2d=(PFNGLTEXIMAGE2D)gl_proc("glTexImage2D");
  g_capture_gl.copy_tex_sub_image_2d=(PFNGLCOPYTEXSUBIMAGE2D)gl_proc("glCopyTexSubImage2D");
  g_capture_gl.gen_framebuffers=(PFNGLGENFRAMEBUFFERS)gl_proc("glGenFramebuffers");
  if(!g_capture_gl.gen_framebuffers)
    g_capture_gl.gen_framebuffers=(PFNGLGENFRAMEBUFFERS)gl_proc("glGenFramebuffersEXT");
  g_capture_gl.bind_framebuffer=(PFNGLBINDFRAMEBUFFER)gl_proc("glBindFramebuffer");
  if(!g_capture_gl.bind_framebuffer)
    g_capture_gl.bind_framebuffer=(PFNGLBINDFRAMEBUFFER)gl_proc("glBindFramebufferEXT");
  g_capture_gl.framebuffer_texture_2d=(PFNGLFRAMEBUFFERTEXTURE2D)gl_proc("glFramebufferTexture2D");
  if(!g_capture_gl.framebuffer_texture_2d)
    g_capture_gl.framebuffer_texture_2d=(PFNGLFRAMEBUFFERTEXTURE2D)gl_proc("glFramebufferTexture2DEXT");
  g_capture_gl.check_framebuffer_status=(PFNGLCHECKFRAMEBUFFERSTATUS)gl_proc("glCheckFramebufferStatus");
  if(!g_capture_gl.check_framebuffer_status)
    g_capture_gl.check_framebuffer_status=(PFNGLCHECKFRAMEBUFFERSTATUS)gl_proc("glCheckFramebufferStatusEXT");
  g_capture_gl.blit_framebuffer=(PFNGLBLITFRAMEBUFFER)gl_proc("glBlitFramebuffer");
  if(!g_capture_gl.blit_framebuffer)
    g_capture_gl.blit_framebuffer=(PFNGLBLITFRAMEBUFFER)gl_proc("glBlitFramebufferEXT");
  g_capture_gl.active_texture=(PFNGLACTIVETEXTURE)gl_proc("glActiveTexture");
  g_capture_gl.get_uniform_location=(PFNGLGETUNIFORMLOCATION)gl_proc("glGetUniformLocation");
  g_capture_gl.uniform_1i=(PFNGLUNIFORM1I)gl_proc("glUniform1i");
  g_capture_gl.uniform_4f=(PFNGLUNIFORM4F)gl_proc("glUniform4f");
  g_capture_gl.uniform_4fv=(PFNGLUNIFORM4FV)gl_proc("glUniform4fv");
  g_capture_gl.get_uniform_fv=(PFNGLGETUNIFORMFV)gl_proc("glGetUniformfv");
  g_capture_gl.get_uniform_iv=(PFNGLGETUNIFORMIV)gl_proc("glGetUniformiv");
  g_capture_gl.get_error=(PFNGLGETERROR)gl_proc("glGetError");
  g_capture_gl.ok=g_capture_gl.get_integer && g_capture_gl.gen_textures &&
    g_capture_gl.bind_texture && g_capture_gl.tex_parameter_i &&
    g_capture_gl.tex_image_2d && g_capture_gl.copy_tex_sub_image_2d &&
    g_capture_gl.active_texture && g_capture_gl.get_uniform_location &&
    g_capture_gl.uniform_1i && g_capture_gl.uniform_4f;
  return &g_capture_gl;
}

static int capture_gl_can_downsample(CaptureGL *gl) {
  return gl && gl->gen_framebuffers && gl->bind_framebuffer &&
    gl->framebuffer_texture_2d && gl->check_framebuffer_status &&
    gl->blit_framebuffer;
}

static void advance_frame(void) {
  load_runtime_config();
  tr456_wet_lara_end_frame();
  g_scene_captured=0;
  perf_note_frame_interval();
  perf_note_frame();
  g_frame_index++;
  if(g_runtime_perf_telemetry && g_perf.frames>=300u)
    perf_log_summary("telemetry-window",1);
  if(diag_is_active() && (g_frame_index%120u)==0u)
    perf_log_summary("diag-window",0);
  if(g_diag_active_frames>0) {
    g_diag_active_frames--;
    if(!g_diag_active_frames) {
      perf_log_summary("diag-end",1);
      char msg[128];
      snprintf(msg,sizeof(msg),"diag insert end session=%d frame=%u",g_diag_session,g_frame_index);
      log_line(msg);
    }
  }
  if(!g_frame_index) {
    g_frame_index=1;
    for(size_t i=0;i<sizeof(g_program_tracks)/sizeof(g_program_tracks[0]);i++)
      g_program_tracks[i].uniform_frame=0;
  }
}

static void prepare_scene_capture_internal(const char *reason,
                                           int allow_unknown_program) {
  unsigned long long perf_t0=perf_ticks_now();
  int perf_capture_updated=0;
  int perf_capture_resized=0;
  if(!allow_unknown_program && !g_current_program_type)
    goto perf_done;

  load_runtime_config();
  if(!g_runtime_fbo_reflection)
    goto perf_done;

  CaptureGL *gl=capture_gl();
  if(!gl->ok) {
    if(!g_logged_capture)
      log_line("framebuffer reflection capture skipped: missing required GL entry point");
    goto perf_done;
  }

  GLint viewport[4]={0,0,0,0};
  shadow_get_integer_or_gl(GL_VIEWPORT,viewport);
  if(viewport[2]<=0 || viewport[3]<=0) goto perf_done;
  g_scene_view_w=viewport[2];
  g_scene_view_h=viewport[3];

  int scale=g_runtime_fbo_scale;
  if(scale<1) scale=1;
  if(scale>4) scale=4;
  if(scale>1 && !capture_gl_can_downsample(gl)) {
    if(!g_logged_capture_scale_fallback) {
      log_line("framebuffer reflection downsample disabled: missing framebuffer blit entry point");
      g_logged_capture_scale_fallback=1;
    }
    scale=1;
  }
  int capture_w=viewport[2]/scale;
  int capture_h=viewport[3]/scale;
  if(capture_w<1) capture_w=1;
  if(capture_h<1) capture_h=1;

  GLint old_active=GL_TEXTURE0;
  GLint old_scene_texture_2d=0;
  shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
  shadow_call_active_texture(gl,GL_TEXTURE15);
  shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,&old_scene_texture_2d);

  if(!g_scene_tex) {
    gl->gen_textures(1,&g_scene_tex);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    {
      const unsigned char black[4]={0,0,0,255};
      gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,black);
    }
    g_scene_w=1;
    g_scene_h=1;
    g_scene_scale=1;
    g_scene_has_pixels=1;
  } else {
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
  }

  int warmup=g_runtime_fbo_warmup_frames>0 &&
    g_frame_index<(unsigned int)g_runtime_fbo_warmup_frames;
  int resized=0;
  if(warmup) {
    g_scene_captured=1;
  } else
  if(g_scene_w!=capture_w || g_scene_h!=capture_h || g_scene_scale!=scale) {
    gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,capture_w,capture_h,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
    g_scene_w=capture_w;
    g_scene_h=capture_h;
    g_scene_scale=scale;
    resized=1;
    perf_capture_resized=1;
    g_scene_has_pixels=0;
  }

  GLenum err=0;
  if(!g_scene_captured) {
    int capture_interval=g_runtime_fbo_capture_interval;
    if(capture_interval<1) capture_interval=1;
    int capture_now=resized || !g_scene_has_pixels ||
      capture_interval<=1 ||
      (g_frame_index%(unsigned int)capture_interval)==0u;
    if(capture_now) {
      if(scale>1) {
        GLint old_read_fbo=0;
        GLint old_draw_fbo=0;
        if(!g_scene_fbo)
          gl->gen_framebuffers(1,&g_scene_fbo);
        if(g_scene_fbo) {
          shadow_get_integer_or_gl(GL_READ_FRAMEBUFFER_BINDING,&old_read_fbo);
          shadow_get_integer_or_gl(GL_DRAW_FRAMEBUFFER_BINDING,&old_draw_fbo);
          shadow_call_bind_framebuffer(gl,GL_DRAW_FRAMEBUFFER,g_scene_fbo);
          gl->framebuffer_texture_2d(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,g_scene_tex,0);
          GLenum status=gl->check_framebuffer_status(GL_DRAW_FRAMEBUFFER);
          if(status==GL_FRAMEBUFFER_COMPLETE) {
            shadow_call_bind_framebuffer(gl,GL_READ_FRAMEBUFFER,(GLuint)old_read_fbo);
            gl->blit_framebuffer(viewport[0],viewport[1],
              viewport[0]+viewport[2],viewport[1]+viewport[3],
              0,0,capture_w,capture_h,GL_COLOR_BUFFER_BIT,GL_LINEAR);
            err=(g_runtime_fbo_error_check && gl->get_error) ?
              gl->get_error() : 0;
          } else {
            err=status ? status : 1u;
          }
          shadow_call_bind_framebuffer(gl,GL_READ_FRAMEBUFFER,(GLuint)old_read_fbo);
          shadow_call_bind_framebuffer(gl,GL_DRAW_FRAMEBUFFER,(GLuint)old_draw_fbo);
        } else {
          err=1u;
        }
        if(err) {
          if(!g_logged_capture_scale_fallback) {
            char fallback_msg[160];
            snprintf(fallback_msg,sizeof(fallback_msg),
              "framebuffer reflection downsample fallback to full resolution glerr=0x%04X",
              (unsigned int)err);
            log_line(fallback_msg);
            g_logged_capture_scale_fallback=1;
          }
          shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
          gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,viewport[2],viewport[3],0,GL_RGBA,GL_UNSIGNED_BYTE,0);
          g_scene_w=viewport[2];
          g_scene_h=viewport[3];
          g_scene_scale=1;
          g_runtime_fbo_scale=1;
          capture_w=viewport[2];
          capture_h=viewport[3];
          scale=1;
          gl->copy_tex_sub_image_2d(GL_TEXTURE_2D,0,0,0,viewport[0],viewport[1],viewport[2],viewport[3]);
          err=(g_runtime_fbo_error_check && gl->get_error) ?
            gl->get_error() : 0;
        }
      } else {
        gl->copy_tex_sub_image_2d(GL_TEXTURE_2D,0,0,0,viewport[0],viewport[1],viewport[2],viewport[3]);
        err=(g_runtime_fbo_error_check && gl->get_error) ?
          gl->get_error() : 0;
      }
      perf_capture_updated=1;
      g_scene_has_pixels=err==0;
    }
    g_scene_captured=1;
    if(capture_now && !g_logged_capture) {
      char msg[288];
      snprintf(msg,sizeof(msg),"framebuffer reflection capture enabled size=%dx%d capture=%dx%d scale=%d tex=%u program=%u type=%d reason=%s glerr=0x%04X",
        viewport[2],viewport[3],capture_w,capture_h,scale,g_scene_tex,g_current_program,g_current_program_type,
        reason ? reason : "unknown",(unsigned int)err);
      log_line(msg);
      g_logged_capture=1;
    }
  }

  ProgramTrack *track=program_track(g_current_program,1);
  if(track) {
    if(track->uniform_frame!=g_frame_index || track->uniform_w!=viewport[2] || track->uniform_h!=viewport[3]) {
      if(track->scene_loc==-2)
        track->scene_loc=gl->get_uniform_location(g_current_program,"uTrWaterScene");
      if(track->scene_loc>=0)
        shadow_call_uniform_1i(gl,track->scene_loc,15);
      if(track->info_loc==-2)
        track->info_loc=gl->get_uniform_location(g_current_program,"uTrWaterCaptureInfo");
      if(track->info_loc>=0)
        shadow_call_uniform_4f(gl,track->info_loc,1.0f/(GLfloat)viewport[2],1.0f/(GLfloat)viewport[3],
          (GLfloat)viewport[2],(GLfloat)viewport[3]);
      track->uniform_frame=g_frame_index;
      track->uniform_w=viewport[2];
      track->uniform_h=viewport[3];
    }
  }

  shadow_call_bind_texture(gl,GL_TEXTURE_2D,(GLuint)old_scene_texture_2d);
  shadow_call_active_texture(gl,(GLenum)old_active);

perf_done:
  perf_note_capture(perf_capture_updated,perf_capture_resized,
    perf_ticks_elapsed(perf_t0));
}

static void bind_flow_inplace_scene_texture(void) {
  if(!g_scene_tex) return;
  CaptureGL *gl=capture_gl();
  if(!gl || !gl->ok || !gl->active_texture || !gl->bind_texture)
    return;
  GLint old_active=GL_TEXTURE0;
  shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
  shadow_call_active_texture(gl,GL_TEXTURE15);
  shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_scene_tex);
  shadow_call_active_texture(gl,(GLenum)old_active);
}

static void prepare_scene_capture_for_flow_inplace(void) {
  prepare_scene_capture_internal("flow in-place shader",0);
  bind_flow_inplace_scene_texture();
}

static char *join_sources(GLsizei count, const GLchar * const *strings, const GLint *lengths) {
  if(count<=0 || !strings) return 0;
  size_t total=0;
  for(GLsizei i=0;i<count;i++) {
    if(!strings[i]) continue;
    size_t n=(lengths && lengths[i]>=0) ? (size_t)lengths[i] : strlen(strings[i]);
    if(total+n>4*1024*1024) return 0;
    total+=n;
  }
  char *out=(char*)malloc(total+1);
  if(!out) return 0;
  char *p=out;
  for(GLsizei i=0;i<count;i++) {
    if(!strings[i]) continue;
    size_t n=(lengths && lengths[i]>=0) ? (size_t)lengths[i] : strlen(strings[i]);
    memcpy(p,strings[i],n);
    p+=n;
  }
  *p=0;
  return out;
}

static int flow_shader_dump_seen(uint32_t hash) {
  static uint32_t dumped[16];
  static int dumped_count;
  for(int i=0;i<dumped_count;i++)
    if(dumped[i]==hash) return 1;
  if(dumped_count<(int)(sizeof(dumped)/sizeof(dumped[0])))
    dumped[dumped_count++]=hash;
  return 0;
}

static void dump_flow_shader_source_once(uint32_t hash, const char *src) {
  if(!g_runtime_dump_flow_shader_source || !src || !*src) return;
  if(flow_shader_dump_seen(hash)) return;
  char dir[MAX_PATH];
  snprintf(dir,sizeof(dir),"%s\\shader_dumps",g_mod_dir[0] ? g_mod_dir : g_dir);
  CreateDirectoryA(dir,0);
  const char *stage=is_flow_vertex_shader(hash) ? "vertex" : "fragment";
  char path[MAX_PATH];
  snprintf(path,sizeof(path),"%s\\flow-%s-%08X.glsl",dir,stage,(unsigned int)hash);
  HANDLE h=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,0);
  if(h==INVALID_HANDLE_VALUE) return;
  DWORD written=0;
  const DWORD len=(DWORD)strlen(src);
  WriteFile(h,src,len,&written,0);
  CloseHandle(h);
  char msg[256];
  snprintf(msg,sizeof(msg),"dumped original flow %s shader source hash=0x%08X path=%s",
    stage,(unsigned int)hash,path);
  log_line(msg);
}

static const char *flow_inplace_fragment_shader_source(void) {
  return
    "#version 150\n"
    "uniform sampler3D sNoise;\n"
    "uniform sampler2DArray sTex0;\n"
    "uniform sampler2DArray sTex1;\n"
    "uniform sampler2DArrayShadow sShadow;\n"
    "uniform sampler2DArray sTex0_wrap;\n"
    "uniform sampler2DArray sTex0_mirror;\n"
    "uniform sampler2D uTrWaterScene;\n"
    "uniform mat4 uProjMatrix;\n"
    "uniform vec4 uViewMatrix[4];\n"
    "uniform mat4 uShadowMatrix;\n"
    "uniform vec4 uFogColor;\n"
    "uniform vec4 uContacts[16];\n"
    "uniform vec4 uModelMatrix[4];\n"
    "uniform vec4 uParams;\n"
    "uniform vec4 uJoints[96];\n"
    "uniform vec4 uLightPos[4];\n"
    "uniform vec4 uLightCol[4];\n"
    "uniform vec4 uAmbient[6];\n"
    "uniform vec4 uTrWaterToggle0;\n"
    "uniform vec4 uTrWaterToggle1;\n"
    "uniform vec4 uTrWaterToggle2;\n"
    "uniform vec4 uTrWaterMaterialProfile;\n"
    "uniform vec4 uTrWaterFlowFx0;\n"
    "uniform vec4 uTrWaterFlowFx1;\n"
    "uniform vec4 uTrWaterFlowFx2;\n"
    "uniform vec4 uTrWaterFlowFx3;\n"
    "uniform vec4 uTrWaterCaptureInfo;\n"
    "in vec2 vTexCoord;\n"
    "in vec3 vColor;\n"
    "in vec3 vLight;\n"
    "in float vLayer;\n"
    "in float vFog;\n"
    "in vec3 vNormal;\n"
    "in vec3 vPos;\n"
    "out vec4 fragColor;\n"
    "float trSat(float x){return clamp(x,0.0,1.0);}\n"
    "float trLine(float x,float p){return pow(trSat(1.0-abs(fract(x)-0.5)*2.0),p);}\n"
    "void main(){\n"
    " vec2 uv=vTexCoord;\n"
    " vec4 d=texture(sTex0_wrap,vec3(uv.xy,vLayer));\n"
    " vec3 light=vLight+vColor;\n"
    " d.xyz*=light*2.0;\n"
    " float surfaceGate=(uTrWaterMaterialProfile.x>2.5&&uTrWaterMaterialProfile.y<0.5)?1.0:0.0;\n"
    " surfaceGate*=uTrWaterToggle0.x;\n"
    " if(surfaceGate<=0.001){d.xyz=mix(uFogColor.xyz*d.w,d.xyz,vFog);fragColor=d;return;}\n"
    " vec2 flow=length(uParams.xy)>0.00001?normalize(uParams.xy):vec2(0.0,-1.0);\n"
    " float t=uModelMatrix[3].x;\n"
    " float speed=max(length(uParams.xy),0.20);\n"
    " float travel=t*(0.85+speed*0.18);\n"
    " vec2 flowSide=vec2(-flow.y,flow.x);\n"
    " vec2 worldUv=vec2(dot(vPos.xz,flow),dot(vPos.xz,flowSide))*0.00052;\n"
    " float swellA=sin(worldUv.x*4.10+sin(worldUv.y*1.85+travel*0.18)*0.75-travel*0.46);\n"
    " float swellB=sin(worldUv.x*7.20-worldUv.y*1.35-travel*0.72);\n"
    " float smoothWave=swellA*0.62+swellB*0.28;\n"
    " vec2 tileEdge=abs(fract(uv)-vec2(0.5))*2.0;\n"
    " float seamWidth=clamp(uTrWaterFlowFx3.y,0.004,0.18);\n"
    " float seamMask=max(smoothstep(1.0-seamWidth,1.0,tileEdge.x),smoothstep(1.0-seamWidth,1.0,tileEdge.y))*uTrWaterFlowFx3.x;\n"
    " seamMask=trSat(seamMask);\n"
    " float lane=sin(dot(uv,vec2(18.0,3.0))-travel*1.35)*uTrWaterToggle1.x;\n"
    " float crossToggle=uTrWaterToggle1.z*0.18;\n"
    " float cross=sin(dot(uv,vec2(-2.0,24.0))+travel*0.82)*crossToggle*uTrWaterFlowFx0.w;\n"
    " float fine=sin(dot(uv,vec2(52.0,7.0))-travel*2.15)*uTrWaterToggle1.x*uTrWaterFlowFx2.w;\n"
    " float tensionA=trLine(uv.x*3.8+uv.y*0.35-travel*0.20,3.2);\n"
    " float tensionB=trLine(uv.x*8.0-uv.y*0.62-travel*0.52,5.0);\n"
    " float tension=trSat((tensionA*0.26+tensionB*0.24+abs(cross)*0.14)*uTrWaterFlowFx0.z*crossToggle);\n"
    " float film=trSat(lane*0.20+cross*0.12+fine*0.08+smoothWave*0.24+tension*0.18+seamMask*0.24+0.34);\n"
    " float foam=(trLine(uv.x*7.0-travel*0.36+fine*0.08,10.0)*0.25+trLine(uv.x*15.0+uv.y*1.5-travel*0.82,18.0)*0.18)*uTrWaterToggle0.w*uTrWaterFlowFx1.z*uTrWaterMaterialProfile.z;\n"
    " float glint=pow(trSat(film*(0.50+abs(smoothWave)*0.34)+tension*0.08),14.0)*uTrWaterToggle1.y*uTrWaterFlowFx1.x*0.42;\n"
    " vec2 screen=gl_FragCoord.xy*max(uTrWaterCaptureInfo.xy,vec2(1.0/8192.0));\n"
    " vec2 screenDir=normalize(vec2(flow.x,-flow.y)+vec2(0.0001,0.0003));\n"
    " vec2 screenSide=vec2(-screenDir.y,screenDir.x);\n"
    " vec2 warp=(screenDir*(smoothWave*0.0058+lane*0.0022+fine*0.0010+tension*0.0042+seamMask*0.0048)+screenSide*(cross*0.0022+swellB*0.0026+tension*0.0022+(tileEdge.x-tileEdge.y)*seamMask*0.0028))*uTrWaterToggle0.y*uTrWaterFlowFx0.y*uTrWaterFlowFx1.w;\n"
    " for(int i=0;i<4;i++){\n"
    "  vec4 c=uContacts[i];\n"
    "  float age=max(c.z,0.0);\n"
    "  float r=max(abs(c.w)*0.025,80.0);\n"
    "  vec2 delta=vPos.xz-c.xy;\n"
    "  float hit=trSat(1.0-length(delta)/r)*(1.0-smoothstep(0.0,90.0,age))*uTrWaterToggle2.x;\n"
    "  warp+=screenSide*hit*0.004;\n"
    "  film+=hit*0.10;\n"
    " }\n"
    " vec3 scene=texture(uTrWaterScene,clamp(screen+warp,vec2(0.001),vec2(0.999))).rgb;\n"
    " vec3 base=d.xyz;\n"
    " vec3 n=normalize(vNormal+vec3(warp.x*30.0,smoothWave*0.08,warp.y*30.0));\n"
    " vec3 viewVec=normalize(-vPos);\n"
    " float fres=pow(1.0-trSat(abs(dot(n,viewVec))),2.0);\n"
    " vec2 reflWarp=screenDir*(smoothWave*0.014+film*0.006+tension*0.010)+screenSide*(swellB*0.010+cross*0.004+tension*0.007);\n"
    " vec3 reflA=texture(uTrWaterScene,clamp(screen+reflWarp+vec2(0.0,0.030+fres*0.038),vec2(0.001),vec2(0.999))).rgb;\n"
    " vec3 reflB=texture(uTrWaterScene,clamp(screen-reflWarp*0.72+screenSide*0.014,vec2(0.001),vec2(0.999))).rgb;\n"
    " vec3 reflected=(reflA*0.64+reflB*0.36)*vec3(0.96,1.00,1.04)+vec3(0.004,0.012,0.016);\n"
    " vec3 tint=vec3(0.006,0.030,0.034);\n"
    " float refrAmt=trSat((0.46+seamMask*0.16)*uTrWaterToggle0.y*uTrWaterFlowFx0.y*uTrWaterFlowFx3.z);\n"
    " vec3 refr=mix(base,scene*vec3(0.94,1.01,1.04)+tint,refrAmt);\n"
    " d.xyz=mix(base,refr,surfaceGate);\n"
    " vec3 seamCover=scene*vec3(0.90,1.01,1.05)+tint*0.35+reflected*0.06;\n"
    " d.xyz=mix(d.xyz,seamCover,seamMask*0.42*surfaceGate);\n"
    " float reflMask=trSat((0.055+fres*0.22+film*0.030+abs(smoothWave)*0.035+seamMask*0.025)*uTrWaterToggle0.z*uTrWaterFlowFx0.x*uTrWaterMaterialProfile.w*uTrWaterFlowFx3.w);\n"
    " d.xyz=mix(d.xyz,reflected,reflMask);\n"
    " float relief=trSat(film*0.12+tension*0.16+abs(smoothWave)*0.32+abs(cross)*0.06+abs(fine)*0.04)*uTrWaterFlowFx2.x;\n"
    " d.xyz+=vec3(0.010,0.030,0.032)*(relief*uTrWaterFlowFx2.y+foam*0.22)*surfaceGate;\n"
    " d.xyz+=vec3(0.008,0.018,0.018)*tension*uTrWaterFlowFx2.z*surfaceGate;\n"
    " d.xyz+=vec3(0.12,0.15,0.13)*glint*surfaceGate;\n"
    " d.xyz=mix(uFogColor.xyz*d.w,d.xyz,vFog);\n"
    " fragColor=d;\n"
    "}\n";
}

static const char *flow_inplace_vertex_shader_source(void) {
  return
    "#version 150\n"
    "uniform sampler3D sNoise;\n"
    "uniform mat4 uProjMatrix;\n"
    "uniform vec4 uViewMatrix[4];\n"
    "uniform mat4 uShadowMatrix;\n"
    "uniform vec4 uFogColor;\n"
    "uniform vec4 uContacts[16];\n"
    "uniform vec4 uModelMatrix[4];\n"
    "uniform vec4 uParams;\n"
    "uniform vec4 uJoints[96];\n"
    "uniform vec4 uLightPos[4];\n"
    "uniform vec4 uLightCol[4];\n"
    "uniform vec4 uAmbient[6];\n"
    "uniform vec4 uTrWaterMaterialProfile;\n"
    "out vec2 vTexCoord;\n"
    "out vec3 vColor;\n"
    "out vec3 vLight;\n"
    "out float vLayer;\n"
    "out float vFog;\n"
    "out vec3 vNormal;\n"
    "out vec3 vPos;\n"
    "in vec4 aCoord;\n"
    "in vec4 aNormal;\n"
    "in vec4 aLight;\n"
    "in vec4 aColor;\n"
    "void main(){\n"
    " vec4 coord=vec4(aCoord);\n"
    " vec4 normal=vec4(aNormal);\n"
    " vec4 light=aLight;\n"
    " vec4 color=aColor;\n"
    " vec2 uv=vec2(light.w,color.w);\n"
    " uv+=uParams.xy*uModelMatrix[3].x;\n"
    " vTexCoord=uv;\n"
    " normal.xyz=normalize(normal.xyz-127.0);\n"
    " vNormal=normal.xyz;\n"
    " vLight=pow(light.xyz,vec3(2.2));\n"
    " vColor=pow(color.xyz,vec3(2.2));\n"
    " vLayer=normal.w;\n"
    " vec4 p=vec4(dot(uModelMatrix[0],vec4(coord.xyz,1.0)),dot(uModelMatrix[1],vec4(coord.xyz,1.0)),dot(uModelMatrix[2],vec4(coord.xyz,1.0)),1.0);\n"
    " float surfaceGate=(uTrWaterMaterialProfile.x>2.5&&uTrWaterMaterialProfile.y<0.5)?1.0:0.0;\n"
    " if(surfaceGate<=0.001){\n"
    "  vec3 wp=p.xyz+vec3(uViewMatrix[0].w,uViewMatrix[1].w,uViewMatrix[2].w);\n"
    "  float weight=coord.w/32767.0;\n"
    "  float k=texture(sNoise,wp*uParams.z).x;\n"
    "  float t=uModelMatrix[3].x*1.25;\n"
    "  float offset=sin(k*6.2831853072+t)*weight*uParams.w;\n"
    "  p.xyz+=normal.xyz*offset;\n"
    " }\n"
    " vFog=clamp(exp(-((length(p.xyz)/15000.0)*(length(p.xyz)/15000.0))),0.0,1.0);\n"
    " vPos=p.xyz;\n"
    " gl_Position=uProjMatrix*vec4(dot(uViewMatrix[0].xyz,p.xyz),dot(uViewMatrix[1].xyz,p.xyz),dot(uViewMatrix[2].xyz,p.xyz),p.w);\n"
    "}\n";
}

static PFNGLSHADERSOURCE real_shader_source(const char *name) {
  static PFNGLSHADERSOURCE p;
  if(p) return p;
  p=(PFNGLSHADERSOURCE)icd_proc(name);
  if(p) return p;
  p=(PFNGLSHADERSOURCE)old_proc(name);
  if(p) return p;
  PFNWGLGETPROCADDRESS real_wgl=trshader_real_wgl_get_proc_address();
  if(real_wgl && g_real_wgl_query_depth<=0) {
    g_real_wgl_query_depth++;
    p=(PFNGLSHADERSOURCE)real_wgl(name);
    g_real_wgl_query_depth--;
  }
  if(!trshader_valid_gl_proc((PROC)p))
    p=0;
  return p;
}

static void APIENTRY hook_glShaderSource(GLuint shader, GLsizei count, const GLchar * const *strings, const GLint *lengths) {
  PFNGLSHADERSOURCE real=real_shader_source("glShaderSource");
#if TR456_STARTUP_LOG
  LONG boot_n=InterlockedIncrement(&g_boot_shader_source_count);
  if(boot_n<=24)
    boot_logf("hook_glShaderSource enter #%ld depth=%d shader=%u count=%d real=%p",
      (long)boot_n,g_shader_source_hook_depth,shader,(int)count,(void*)real);
#endif
  if(!real) return;
  if((void*)real==(void*)hook_glShaderSource || g_shader_source_hook_depth>0) {
    PFNGLSHADERSOURCE icd=(PFNGLSHADERSOURCE)icd_proc("glShaderSource");
#if TR456_STARTUP_LOG
    if(boot_n<=24)
      boot_logf("hook_glShaderSource recursion guard #%ld real=%p icd=%p",
        (long)boot_n,(void*)real,(void*)icd);
#endif
    if(!icd || (void*)icd==(void*)hook_glShaderSource)
      return;
    icd(shader,count,strings,lengths);
    return;
  }
  load_runtime_config();
  g_shader_source_hook_depth++;
  unsigned int src_len=0;
  uint32_t src_hash=fnv1a_sources(count,strings,lengths,&src_len);
  char *src=0;
  int type=0;
  const ShaderSourcePatch *patch=find_source_patch_sources(count,strings,lengths,src_hash);
  if(patch)
    type=patch->type;
  const int dump_src=g_runtime_dump_flow_shader_source && type==SHADER_WATER_FLOW;
  const int need_src=runtime_verbose_log() || TR456_DIAG_BUILD || dump_src;
  if(need_src)
    src=join_sources(count,strings,lengths);
  set_shader_info(shader,type,src_hash,src_len,src);
  if(type)
    set_shader_type(shader,type);
  if(dump_src)
    dump_flow_shader_source_once(src_hash,src);
  const int replace_flow_fragment_source=
    g_runtime_flow_inplace_patch &&
    type==SHADER_WATER_FLOW &&
    is_flow_shader(src_hash);
  const int replace_flow_vertex_source=
    g_runtime_flow_inplace_patch &&
    type==SHADER_WATER_FLOW &&
    is_flow_vertex_shader(src_hash);
  const int replace_flow_source=
    replace_flow_fragment_source || replace_flow_vertex_source;
#if TR456_DIAG_BUILD
  {
    LONG n=InterlockedIncrement(&g_diag_shader_source_count);
    if(n<=180) {
      ShaderTrack *tracked=shader_track(shader,0);
      diag_logf("DIAG glShaderSource #%ld shader=%u hash=0x%08X len=%u type=%s patch=%s replacement=%d preview=\"%s\"",
        (long)n,shader,(unsigned int)src_hash,src_len,
        shader_type_name(type),patch ? patch->label : "none",
        replace_flow_source,
        tracked ? tracked->preview : "");
    }
  }
#endif
  if(patch && type!=SHADER_WATER_RIPPLE && runtime_verbose_log()) {
    char msg[176];
    snprintf(msg,sizeof(msg),"tracked original %s shader=%u hash=0x%08X len=%u",
      shader_type_name(type),shader,(unsigned int)src_hash,src_len);
    log_line(msg);
  }
  if(replace_flow_source) {
    static int logged_fragment_replace;
    static int logged_vertex_replace;
    const char *replacement=replace_flow_vertex_source ?
      flow_inplace_vertex_shader_source() :
      flow_inplace_fragment_shader_source();
    GLint replacement_len=(GLint)strlen(replacement);
    int *logged=replace_flow_vertex_source ?
      &logged_vertex_replace : &logged_fragment_replace;
    if(!*logged) {
      char msg[192];
      snprintf(msg,sizeof(msg),
        "replaced original flow %s shader with in-place water effects hash=0x%08X shader=%u",
        replace_flow_vertex_source ? "vertex" : "fragment",
        (unsigned int)src_hash,shader);
      log_line(msg);
      *logged=1;
    }
    real(shader,1,&replacement,&replacement_len);
  } else {
    real(shader,count,strings,lengths);
  }
  g_shader_source_hook_depth--;
#if TR456_STARTUP_LOG
  if(boot_n<=24)
    boot_logf("hook_glShaderSource leave #%ld shader=%u", (long)boot_n,shader);
#endif
  free(src);
}

static PFNGLCOMPILESHADER real_compile_shader(void) {
  static PFNGLCOMPILESHADER p;
  if(!p) p=(PFNGLCOMPILESHADER)gl_proc("glCompileShader");
  if(!p) p=(PFNGLCOMPILESHADER)gl_proc("glCompileShaderARB");
  return p;
}

static PFNGLLINKPROGRAM real_link_program(void) {
  static PFNGLLINKPROGRAM p;
  if(!p) p=(PFNGLLINKPROGRAM)gl_proc("glLinkProgram");
  if(!p) p=(PFNGLLINKPROGRAM)gl_proc("glLinkProgramARB");
  return p;
}

static void prepare_scene_capture_for_synthetic_surface(const char *reason) {
  prepare_scene_capture_internal(reason,1);
}

static const SyntheticDrawDecision *prepare_synthetic_surface_draw_decision(
    GLenum mode, GLsizei count, int count_known,
    const char *flow_reason, const char *standing_reason) {
  const SyntheticDrawDecision *decision=
    current_synthetic_draw_decision(mode,count,count_known);
  if(decision->synthetic_ready)
    prepare_scene_capture_for_synthetic_surface(
      decision->synthetic_flow ? flow_reason : standing_reason);
  return decision;
}

static int ensure_current_synthetic_surface_program(void) {
  load_runtime_config();
  if(g_current_program_type==SHADER_WATER_FLOW && g_runtime_flow_lite_surface)
    return ensure_flow_lite_surface_program();
  return ensure_synthetic_surface_program();
}

static int current_draw_uses_water_underlay_pattern(void) {
  load_runtime_config();
  if(!g_runtime_underlay_pattern || g_runtime_underlay_pattern_strength<=0.001f)
    return 0;
  if(g_current_program_type==SHADER_WATER_FLOW)
    return g_runtime_underlay_flow_pattern!=0;
  return g_current_program_type==SHADER_WATER_SURFACE ||
    g_current_program_type==SHADER_WATER_REFLECT;
}

static int current_draw_uses_reflect_original_mask(void) {
  load_runtime_config();
  return g_current_program_type==SHADER_WATER_REFLECT &&
    g_runtime_synthetic_reflect_surface &&
    !g_runtime_synthetic_standing_replace_original &&
    g_runtime_synthetic_reflect_original_mask>0.001f;
}

static int ensure_underlay_texture(CaptureGL *gl, int width, int height) {
  if(!gl || !gl->gen_textures || !gl->bind_texture || !gl->tex_image_2d ||
     !gl->tex_parameter_i)
    return 0;
  if(width<=0 || height<=0) return 0;

  if(!g_underlay_tex) {
    gl->gen_textures(1,&g_underlay_tex);
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_underlay_tex);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    gl->tex_parameter_i(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    {
      const unsigned char black[4]={0,0,0,255};
      gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,1,1,0,GL_RGBA,
        GL_UNSIGNED_BYTE,black);
    }
    g_underlay_w=1;
    g_underlay_h=1;
    g_underlay_has_pixels=0;
  } else {
    shadow_call_bind_texture(gl,GL_TEXTURE_2D,g_underlay_tex);
  }

  if(g_underlay_w!=width || g_underlay_h!=height) {
    gl->tex_image_2d(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,
      GL_UNSIGNED_BYTE,0);
    g_underlay_w=width;
    g_underlay_h=height;
    g_underlay_has_pixels=0;
  }
  return 1;
}

static void capture_water_underlay_for_synthetic_surface(const char *reason) {
  const int pattern_capture=current_draw_uses_water_underlay_pattern();
  const int reflect_mask_capture=current_draw_uses_reflect_original_mask();
  if(!pattern_capture && !reflect_mask_capture) {
    g_underlay_has_pixels=0;
    return;
  }

  CaptureGL *gl=capture_gl();
  if(!gl->ok)
    return;

  GLint viewport[4]={0,0,0,0};
  shadow_get_integer_or_gl(GL_VIEWPORT,viewport);
  if(viewport[2]<=0 || viewport[3]<=0) return;

  GLint old_active=GL_TEXTURE0;
  GLint old_underlay_texture_2d=0;
  shadow_get_integer_or_gl(GL_ACTIVE_TEXTURE,&old_active);
  shadow_call_active_texture(gl,GL_TEXTURE14);
  shadow_get_integer_or_gl(GL_TEXTURE_BINDING_2D,&old_underlay_texture_2d);

  if(ensure_underlay_texture(gl,viewport[2],viewport[3])) {
    gl->copy_tex_sub_image_2d(GL_TEXTURE_2D,0,0,0,viewport[0],viewport[1],
      viewport[2],viewport[3]);
    GLenum err=gl->get_error ? gl->get_error() : 0;
    g_underlay_has_pixels=err==0;
    g_underlay_view_w=viewport[2];
    g_underlay_view_h=viewport[3];
    if(!g_logged_underlay_capture) {
      char msg[256];
      snprintf(msg,sizeof(msg),
        "water underlay capture enabled size=%dx%d tex=%u program=%u type=%s pattern=%d reflectMask=%d reason=%s glerr=0x%04X",
        viewport[2],viewport[3],g_underlay_tex,g_current_program,
        shader_type_name(g_current_program_type),pattern_capture,
        reflect_mask_capture,
        reason ? reason : "synthetic water underlay",(unsigned int)err);
      log_line(msg);
      g_logged_underlay_capture=1;
    }
  }

  shadow_call_bind_texture(gl,GL_TEXTURE_2D,(GLuint)old_underlay_texture_2d);
  shadow_call_active_texture(gl,(GLenum)old_active);
}
