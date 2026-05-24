static PFNGLUSEPROGRAM real_use_program(void) {
  static PFNGLUSEPROGRAM p;
  if(!p) p=(PFNGLUSEPROGRAM)gl_proc("glUseProgram");
  return p;
}

static void APIENTRY hook_glUseProgram(GLuint program) {
  PFNGLUSEPROGRAM real=real_use_program();
  if(real) real(program);
  trshader_compat_report_once("glUseProgram");
  shadow_note_use_program(program);
  g_current_program=program;
  g_current_program_type=program_type(program);
  if(g_current_program_type==SHADER_WATER_REFLECT)
    update_contact_cache_from_program(program);
  if(is_tracked_water_shader_type(g_current_program_type)) {
    apply_effect_toggles(program);
    apply_contact_cache(program);
  }
  diag_poll_insert("glUseProgram");
  diag_log_program_use(program);
  diag_log_contacts(program,"glUseProgram");
  if(g_current_program_type==SHADER_WATER_SSR && !g_logged_use_ssr) {
    char msg[128];
    snprintf(msg,sizeof(msg),"using tracked screen-space water program=%u",program);
    log_line(msg);
    g_logged_use_ssr=1;
  }
}

static void APIENTRY hook_glActiveTexture(GLenum texture) {
  PFNGLACTIVETEXTURE real=real_active_texture();
  if(real) real(texture);
  shadow_note_active_texture(texture);
}

static void APIENTRY hook_glBindTexture(GLenum target, GLuint texture) {
  PFNGLBINDTEXTURE real=real_bind_texture();
  if(real) real(target,texture);
  shadow_note_bind_texture(target,texture);
}

static void APIENTRY hook_glBindFramebuffer(GLenum target, GLuint framebuffer) {
  PFNGLBINDFRAMEBUFFER real=real_bind_framebuffer();
  if(real) real(target,framebuffer);
  shadow_note_bind_framebuffer(target,framebuffer);
}

static void APIENTRY hook_glViewport(GLint x, GLint y, GLsizei width,
                                     GLsizei height) {
  PFNGLVIEWPORT real=real_viewport();
  if(real) real(x,y,width,height);
  shadow_note_viewport(x,y,width,height);
}

static void APIENTRY hook_glEnable(GLenum cap) {
  PFNGLENABLE real=real_enable();
  if(real) real(cap);
  shadow_note_enable(cap,1);
}

static void APIENTRY hook_glDisable(GLenum cap) {
  PFNGLDISABLE real=real_disable();
  if(real) real(cap);
  shadow_note_enable(cap,0);
}

static void APIENTRY hook_glDepthMask(GLboolean flag) {
  PFNGLDEPTHMASK real=real_depth_mask();
  if(real) real(flag);
  shadow_note_depth_mask(flag);
}

static void APIENTRY hook_glDepthFunc(GLenum func) {
  PFNGLDEPTHFUNC real=real_depth_func();
  if(real) real(func);
  shadow_note_depth_func(func);
}

static void APIENTRY hook_glBlendFunc(GLenum sfactor, GLenum dfactor) {
  PFNGLBLENDFUNC real=real_blend_func();
  if(real) real(sfactor,dfactor);
  shadow_note_blend_func(sfactor,dfactor,sfactor,dfactor);
}

static void APIENTRY hook_glBlendFuncSeparate(GLenum src_rgb, GLenum dst_rgb,
                                              GLenum src_alpha,
                                              GLenum dst_alpha) {
  PFNGLBLENDFUNCSEPARATE real=real_blend_func_separate();
  if(real) real(src_rgb,dst_rgb,src_alpha,dst_alpha);
  shadow_note_blend_func(src_rgb,dst_rgb,src_alpha,dst_alpha);
}

static void APIENTRY hook_glUniform1i(GLint location, GLint v0) {
  PFNGLUNIFORM1I real=real_uniform_1i();
  if(real) real(location,v0);
  shadow_note_uniform_1i(location,v0);
}

static void APIENTRY hook_glUniform1iv(GLint location, GLsizei count,
                                       const GLint *value) {
  PFNGLUNIFORM1IV real=real_uniform_1iv();
  if(real) real(location,count,value);
  shadow_note_uniform_1iv(location,count,value);
}

static void APIENTRY hook_glUniform4f(GLint location, GLfloat v0, GLfloat v1,
                                      GLfloat v2, GLfloat v3) {
  PFNGLUNIFORM4F real=real_uniform_4f();
  if(real) real(location,v0,v1,v2,v3);
  shadow_note_uniform_4f(location,v0,v1,v2,v3);
}

static void APIENTRY hook_glUniform4fv(GLint location, GLsizei count,
                                       const GLfloat *value) {
  PFNGLUNIFORM4FV real=real_uniform_4fv();
  if(real) real(location,count,value);
  shadow_note_uniform_4fv(location,count,value);
}

static void APIENTRY hook_glUniformMatrix4fv(GLint location, GLsizei count,
                                             GLboolean transpose,
                                             const GLfloat *value) {
  PFNGLUNIFORMMATRIX4FV real=real_uniform_matrix_4fv();
  if(real) real(location,count,transpose,value);
  shadow_note_uniform_matrix4fv(location,count,transpose,value);
}

__declspec(dllexport) void APIENTRY glBindTexture(GLenum target,
                                                  GLuint texture) {
  hook_glBindTexture(target,texture);
}

__declspec(dllexport) void APIENTRY glViewport(GLint x, GLint y,
                                               GLsizei width,
                                               GLsizei height) {
  hook_glViewport(x,y,width,height);
}

__declspec(dllexport) void APIENTRY glEnable(GLenum cap) {
  hook_glEnable(cap);
}

__declspec(dllexport) void APIENTRY glDisable(GLenum cap) {
  hook_glDisable(cap);
}

__declspec(dllexport) void APIENTRY glDepthMask(GLboolean flag) {
  hook_glDepthMask(flag);
}

__declspec(dllexport) void APIENTRY glDepthFunc(GLenum func) {
  hook_glDepthFunc(func);
}

__declspec(dllexport) void APIENTRY glBlendFunc(GLenum sfactor,
                                                GLenum dfactor) {
  hook_glBlendFunc(sfactor,dfactor);
}

static PFNGLDRAWELEMENTS real_draw_elements(void) {
  static PFNGLDRAWELEMENTS p;
  if(!p) p=(PFNGLDRAWELEMENTS)gl_proc("glDrawElements");
  return p;
}

static PFNGLCOMPRESSEDTEXIMAGE2D real_compressed_tex_image_2d(void) {
  static PFNGLCOMPRESSEDTEXIMAGE2D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE2D)gl_proc("glCompressedTexImage2D");
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE2D)gl_proc("glCompressedTexImage2DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXSUBIMAGE2D real_compressed_tex_sub_image_2d(void) {
  static PFNGLCOMPRESSEDTEXSUBIMAGE2D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE2D)gl_proc("glCompressedTexSubImage2D");
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE2D)gl_proc("glCompressedTexSubImage2DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXIMAGE3D real_compressed_tex_image_3d(void) {
  static PFNGLCOMPRESSEDTEXIMAGE3D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE3D)gl_proc("glCompressedTexImage3D");
  if(!p) p=(PFNGLCOMPRESSEDTEXIMAGE3D)gl_proc("glCompressedTexImage3DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXSUBIMAGE3D real_compressed_tex_sub_image_3d(void) {
  static PFNGLCOMPRESSEDTEXSUBIMAGE3D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE3D)gl_proc("glCompressedTexSubImage3D");
  if(!p) p=(PFNGLCOMPRESSEDTEXSUBIMAGE3D)gl_proc("glCompressedTexSubImage3DARB");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE2D real_compressed_texture_sub_image_2d(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE2D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE2D)gl_proc("glCompressedTextureSubImage2D");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE3D real_compressed_texture_sub_image_3d(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE3D p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE3D)gl_proc("glCompressedTextureSubImage3D");
  return p;
}

static PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT real_compressed_texture_image_2d_ext(void) {
  static PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT)gl_proc("glCompressedTextureImage2DEXT");
  return p;
}

static PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT real_compressed_texture_image_3d_ext(void) {
  static PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT)gl_proc("glCompressedTextureImage3DEXT");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT real_compressed_texture_sub_image_2d_ext(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT)gl_proc("glCompressedTextureSubImage2DEXT");
  return p;
}

static PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT real_compressed_texture_sub_image_3d_ext(void) {
  static PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT p;
  if(!p) p=(PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT)gl_proc("glCompressedTextureSubImage3DEXT");
  return p;
}

static PFNGLTEXIMAGE3D real_tex_image_3d(void) {
  static PFNGLTEXIMAGE3D p;
  if(!p) p=(PFNGLTEXIMAGE3D)gl_proc("glTexImage3D");
  return p;
}

static PFNGLTEXSUBIMAGE3D real_tex_sub_image_3d(void) {
  static PFNGLTEXSUBIMAGE3D p;
  if(!p) p=(PFNGLTEXSUBIMAGE3D)gl_proc("glTexSubImage3D");
  return p;
}

static PFNGLTEXTURESUBIMAGE3D real_texture_sub_image_3d(void) {
  static PFNGLTEXTURESUBIMAGE3D p;
  if(!p) p=(PFNGLTEXTURESUBIMAGE3D)gl_proc("glTextureSubImage3D");
  return p;
}

static PFNGLTEXTUREIMAGE3DEXT real_texture_image_3d_ext(void) {
  static PFNGLTEXTUREIMAGE3DEXT p;
  if(!p) p=(PFNGLTEXTUREIMAGE3DEXT)gl_proc("glTextureImage3DEXT");
  return p;
}

static PFNGLTEXTURESUBIMAGE3DEXT real_texture_sub_image_3d_ext(void) {
  static PFNGLTEXTURESUBIMAGE3DEXT p;
  if(!p) p=(PFNGLTEXTURESUBIMAGE3DEXT)gl_proc("glTextureSubImage3DEXT");
  return p;
}

static void APIENTRY hook_glCompressedTexImage2D(GLenum target, GLint level,
    GLenum internalformat, GLsizei width, GLsizei height, GLint border,
    GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXIMAGE2D real=real_compressed_tex_image_2d();
  note_compressed_texture_upload(target,level,0,1,image_size,data);
  if(real) real(target,level,internalformat,width,height,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTexSubImage2D(GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXSUBIMAGE2D real=real_compressed_tex_sub_image_2d();
  note_compressed_texture_upload(target,level,0,1,image_size,data);
  if(real) real(target,level,xoffset,yoffset,width,height,format,
    image_size,data);
}

static void APIENTRY hook_glCompressedTexImage3D(GLenum target, GLint level,
    GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXIMAGE3D real=real_compressed_tex_image_3d();
  note_compressed_texture_upload(target,level,0,depth,image_size,data);
  if(real) real(target,level,internalformat,width,height,depth,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTexSubImage3D(GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLsizei image_size,
    const void *data) {
  PFNGLCOMPRESSEDTEXSUBIMAGE3D real=real_compressed_tex_sub_image_3d();
  note_compressed_texture_upload(target,level,zoffset,depth,image_size,data);
  if(real) real(target,level,xoffset,yoffset,zoffset,width,height,depth,
    format,image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage2D(GLuint texture,
    GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2D real=real_compressed_texture_sub_image_2d();
  note_compressed_texture_upload_for_texture(texture,GL_TEXTURE_2D,level,0,1,
    image_size,data);
  if(real) real(texture,level,xoffset,yoffset,width,height,format,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage3D(GLuint texture,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLsizei image_size,
    const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3D real=real_compressed_texture_sub_image_3d();
  note_compressed_texture_upload_for_texture(texture,GL_TEXTURE_2D_ARRAY,
    level,zoffset,depth,image_size,data);
  if(real) real(texture,level,xoffset,yoffset,zoffset,width,height,depth,
    format,image_size,data);
}

static void APIENTRY hook_glCompressedTextureImage2DEXT(GLuint texture,
    GLenum target, GLint level, GLenum internalformat, GLsizei width,
    GLsizei height, GLint border, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTUREIMAGE2DEXT real=real_compressed_texture_image_2d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,0,1,
    image_size,data);
  if(real) real(texture,target,level,internalformat,width,height,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLenum internalformat, GLsizei width,
    GLsizei height, GLsizei depth, GLint border, GLsizei image_size,
    const void *data) {
  PFNGLCOMPRESSEDTEXTUREIMAGE3DEXT real=real_compressed_texture_image_3d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,0,depth,
    image_size,data);
  if(real) real(texture,target,level,internalformat,width,height,depth,border,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage2DEXT(GLuint texture,
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXT real=real_compressed_texture_sub_image_2d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,0,1,
    image_size,data);
  if(real) real(texture,target,level,xoffset,yoffset,width,height,format,
    image_size,data);
}

static void APIENTRY hook_glCompressedTextureSubImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format,
    GLsizei image_size, const void *data) {
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXT real=real_compressed_texture_sub_image_3d_ext();
  note_compressed_texture_upload_for_texture(texture,target,level,zoffset,
    depth,image_size,data);
  if(real) real(texture,target,level,xoffset,yoffset,zoffset,width,height,
    depth,format,image_size,data);
}

static void APIENTRY hook_glTexImage3D(GLenum target, GLint level,
    GLint internalformat, GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLenum format, GLenum type, const void *data) {
  PFNGLTEXIMAGE3D real=real_tex_image_3d();
  note_uncompressed_texture_upload(target,level,0,width,height,depth,format,
    type,data);
  if(real) real(target,level,internalformat,width,height,depth,border,format,
    type,data);
}

static void APIENTRY hook_glTexSubImage3D(GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXSUBIMAGE3D real=real_tex_sub_image_3d();
  note_uncompressed_texture_upload(target,level,zoffset,width,height,depth,
    format,type,data);
  if(real) real(target,level,xoffset,yoffset,zoffset,width,height,depth,
    format,type,data);
}

static void APIENTRY hook_glTextureSubImage3D(GLuint texture, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXTURESUBIMAGE3D real=real_texture_sub_image_3d();
  note_uncompressed_texture_upload_for_texture(texture,GL_TEXTURE_2D_ARRAY,
    level,zoffset,width,height,depth,format,type,data);
  if(real) real(texture,level,xoffset,yoffset,zoffset,width,height,depth,
    format,type,data);
}

static void APIENTRY hook_glTextureImage3DEXT(GLuint texture, GLenum target,
    GLint level, GLint internalformat, GLsizei width, GLsizei height,
    GLsizei depth, GLint border, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXTUREIMAGE3DEXT real=real_texture_image_3d_ext();
  note_uncompressed_texture_upload_for_texture(texture,target,level,0,width,
    height,depth,format,type,data);
  if(real) real(texture,target,level,internalformat,width,height,depth,border,
    format,type,data);
}

static void APIENTRY hook_glTextureSubImage3DEXT(GLuint texture, GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  PFNGLTEXTURESUBIMAGE3DEXT real=real_texture_sub_image_3d_ext();
  note_uncompressed_texture_upload_for_texture(texture,target,level,zoffset,
    width,height,depth,format,type,data);
  if(real) real(texture,target,level,xoffset,yoffset,zoffset,width,height,
    depth,format,type,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexImage2D(GLenum target,
    GLint level, GLenum internalformat, GLsizei width, GLsizei height,
    GLint border, GLsizei image_size, const void *data) {
  hook_glCompressedTexImage2D(target,level,internalformat,width,height,border,
    image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexSubImage2D(GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    GLenum format, GLsizei image_size, const void *data) {
  hook_glCompressedTexSubImage2D(target,level,xoffset,yoffset,width,height,
    format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexImage3D(GLenum target,
    GLint level, GLenum internalformat, GLsizei width, GLsizei height,
    GLsizei depth, GLint border, GLsizei image_size, const void *data) {
  hook_glCompressedTexImage3D(target,level,internalformat,width,height,depth,
    border,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTexSubImage3D(GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLsizei image_size,
    const void *data) {
  hook_glCompressedTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,
    height,depth,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage2D(
    GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLsizei image_size, const void *data) {
  hook_glCompressedTextureSubImage2D(texture,level,xoffset,yoffset,width,
    height,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage3D(
    GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format,
    GLsizei image_size, const void *data) {
  hook_glCompressedTextureSubImage3D(texture,level,xoffset,yoffset,zoffset,
    width,height,depth,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureImage2DEXT(
    GLuint texture, GLenum target, GLint level, GLenum internalformat,
    GLsizei width, GLsizei height, GLint border, GLsizei image_size,
    const void *data) {
  hook_glCompressedTextureImage2DEXT(texture,target,level,internalformat,
    width,height,border,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureImage3DEXT(
    GLuint texture, GLenum target, GLint level, GLenum internalformat,
    GLsizei width, GLsizei height, GLsizei depth, GLint border,
    GLsizei image_size, const void *data) {
  hook_glCompressedTextureImage3DEXT(texture,target,level,internalformat,
    width,height,depth,border,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage2DEXT(
    GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset,
    GLsizei width, GLsizei height, GLenum format, GLsizei image_size,
    const void *data) {
  hook_glCompressedTextureSubImage2DEXT(texture,target,level,xoffset,yoffset,
    width,height,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glCompressedTextureSubImage3DEXT(
    GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset,
    GLint zoffset, GLsizei width, GLsizei height, GLsizei depth,
    GLenum format, GLsizei image_size, const void *data) {
  hook_glCompressedTextureSubImage3DEXT(texture,target,level,xoffset,yoffset,
    zoffset,width,height,depth,format,image_size,data);
}

__declspec(dllexport) void APIENTRY glTexImage3D(GLenum target, GLint level,
    GLint internalformat, GLsizei width, GLsizei height, GLsizei depth,
    GLint border, GLenum format, GLenum type, const void *data) {
  hook_glTexImage3D(target,level,internalformat,width,height,depth,border,
    format,type,data);
}

__declspec(dllexport) void APIENTRY glTexSubImage3D(GLenum target,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  hook_glTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,height,
    depth,format,type,data);
}

__declspec(dllexport) void APIENTRY glTextureSubImage3D(GLuint texture,
    GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
    GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  hook_glTextureSubImage3D(texture,level,xoffset,yoffset,zoffset,width,height,
    depth,format,type,data);
}

__declspec(dllexport) void APIENTRY glTextureImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLint internalformat, GLsizei width,
    GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type,
    const void *data) {
  hook_glTextureImage3DEXT(texture,target,level,internalformat,width,height,
    depth,border,format,type,data);
}

__declspec(dllexport) void APIENTRY glTextureSubImage3DEXT(GLuint texture,
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
    const void *data) {
  hook_glTextureSubImage3DEXT(texture,target,level,xoffset,yoffset,zoffset,
    width,height,depth,format,type,data);
}
