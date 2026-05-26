#ifndef TR456_PROXY_SHADER_SOURCES_H
#define TR456_PROXY_SHADER_SOURCES_H

/*
 * Shader source entry points.
 *
 * Runtime-loaded water shaders must go through configured_shader() so INI
 * define injection, cache invalidation, preload, and program compilation share
 * the same loading path. Diagnostic shaders may return embedded strings.
 */

char *synthetic_surface_vertex_shader(void);
char *synthetic_surface_shader(void);
char *flow_lite_vertex_shader(void);
char *flow_lite_fragment_shader(void);
char *swap_debug_vertex_shader(void);
char *swap_debug_fragment_shader(void);

#endif
