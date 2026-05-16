#ifndef TR456_SURFACE_PICKER_H
#define TR456_SURFACE_PICKER_H

static void tr456_surface_picker_end_frame(void);
static void tr456_surface_picker_poll_hotkeys(const char *where);
static void tr456_surface_picker_diag_begin(const char *where);

static void tr456_surface_picker_draw_arrays(const char *call,
  PFNGLDRAWARRAYS draw, GLenum mode, GLint first, GLsizei count);
static void tr456_surface_picker_draw_elements(const char *call,
  PFNGLDRAWELEMENTS draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices);
static void tr456_surface_picker_draw_range_elements(const char *call,
  PFNGLDRAWRANGEELEMENTS draw, GLenum mode, GLuint start, GLuint end,
  GLsizei count, GLenum type, const void *indices);
static void tr456_surface_picker_draw_elements_base_vertex(const char *call,
  PFNGLDRAWELEMENTSBASEVERTEX draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices, GLint base_vertex);
static void tr456_surface_picker_draw_range_elements_base_vertex(
  const char *call, PFNGLDRAWRANGEELEMENTSBASEVERTEX draw, GLenum mode,
  GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices,
  GLint base_vertex);
static void tr456_surface_picker_draw_arrays_instanced(const char *call,
  PFNGLDRAWARRAYSINSTANCED draw, GLenum mode, GLint first, GLsizei count,
  GLsizei instance_count);
static void tr456_surface_picker_draw_elements_instanced(const char *call,
  PFNGLDRAWELEMENTSINSTANCED draw, GLenum mode, GLsizei count, GLenum type,
  const void *indices, GLsizei instance_count);
static void tr456_surface_picker_draw_elements_instanced_base_vertex(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX draw, GLenum mode,
  GLsizei count, GLenum type, const void *indices, GLsizei instance_count,
  GLint base_vertex);
static void tr456_surface_picker_draw_arrays_instanced_base_instance(
  const char *call, PFNGLDRAWARRAYSINSTANCEDBASEINSTANCE draw, GLenum mode,
  GLint first, GLsizei count, GLsizei instance_count, GLuint base_instance);
static void tr456_surface_picker_draw_elements_instanced_base_instance(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCE draw, GLenum mode,
  GLsizei count, GLenum type, const void *indices, GLsizei instance_count,
  GLuint base_instance);
static void tr456_surface_picker_draw_elements_instanced_base_vertex_base_instance(
  const char *call, PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCE draw,
  GLenum mode, GLsizei count, GLenum type, const void *indices,
  GLsizei instance_count, GLint base_vertex, GLuint base_instance);
static void tr456_surface_picker_multi_draw_arrays(const char *call,
  PFNGLDRAWARRAYS draw, GLenum mode, const GLint *first, const GLsizei *count,
  GLsizei draw_count);
static void tr456_surface_picker_multi_draw_elements(const char *call,
  PFNGLDRAWELEMENTS draw, GLenum mode, const GLsizei *count, GLenum type,
  const void * const *indices, GLsizei draw_count);
static void tr456_surface_picker_multi_draw_elements_base_vertex(
  const char *call, PFNGLDRAWELEMENTSBASEVERTEX draw, GLenum mode,
  const GLsizei *count, GLenum type, const void * const *indices,
  GLsizei draw_count, const GLint *base_vertex);
static void tr456_surface_picker_draw_arrays_indirect(const char *call,
  PFNGLDRAWARRAYSINDIRECT draw, GLenum mode, const void *indirect);
static void tr456_surface_picker_draw_elements_indirect(const char *call,
  PFNGLDRAWELEMENTSINDIRECT draw, GLenum mode, GLenum type,
  const void *indirect);
static void tr456_surface_picker_multi_draw_arrays_indirect(const char *call,
  PFNGLMULTIDRAWARRAYSINDIRECT draw, GLenum mode, const void *indirect,
  GLsizei draw_count, GLsizei stride);
static void tr456_surface_picker_multi_draw_elements_indirect(const char *call,
  PFNGLMULTIDRAWELEMENTSINDIRECT draw, GLenum mode, GLenum type,
  const void *indirect, GLsizei draw_count, GLsizei stride);

#endif
