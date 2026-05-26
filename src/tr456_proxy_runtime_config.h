#ifndef TR456_PROXY_RUNTIME_CONFIG_H
#define TR456_PROXY_RUNTIME_CONFIG_H

/*
 * Runtime config contract for the proxy.
 *
 * Owned by tr456_proxy_runtime_config.inc:
 * - INI parsing and typed lookup from the [Water] section.
 * - Cached runtime feature flags and tuning values.
 * - External GLSL loading through configured_shader(), used by the separate
 *   shader source factory module.
 * - Shader #define generation and injection.
 *
 * Private to tr456_proxy_runtime_config.inc:
 * - Shader define cache internals.
 * - Clamp/default grouping helpers.
 * - Runtime file timestamp reload policy.
 */

static int ini_int(const char *key, int fallback);
static float ini_float(const char *key, float fallback);
static void ini_string(const char *key, const char *fallback, char *out,
                       size_t out_size);

static void load_runtime_config(void);
char *configured_shader(const char *file, const char *label);

#endif
