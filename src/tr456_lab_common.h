#ifndef TR456_LAB_COMMON_H
#define TR456_LAB_COMMON_H

static GLuint tr456_lab_compile_shader(GLenum stage, const char *label,
                                       const char *text);
static int tr456_lab_read_mat4(const char *name, GLfloat out[16]);
static int tr456_lab_read_vec4_array(const char *base, int count,
                                     GLfloat *out);

#endif
