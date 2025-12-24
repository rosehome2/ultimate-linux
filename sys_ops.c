#include "quickjs-libc.h"

#include <sys/mount.h>
#include <unistd.h>
#include <errno.h>

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

// Helper for error reporting
static JSValue js_get_err(JSContext *ctx) {
    return JS_NewInt32(ctx, -errno);
}

// C Implementation of mount()
static JSValue js_mount(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    const char *source = JS_ToCString(ctx, argv[0]);
    const char *target = JS_ToCString(ctx, argv[1]);
    const char *type = JS_ToCString(ctx, argv[2]);

    // flags=0, data=NULL for a basic mount
    int res = mount(source, target, type, 0, NULL);
    
    JS_FreeCString(ctx, source);
    JS_FreeCString(ctx, target);
    JS_FreeCString(ctx, type);

    if (res < 0) return js_get_err(ctx);
    return JS_NewInt32(ctx, 0);
}

static const JSCFunctionListEntry js_sys_ops_funcs[] = {
    JS_CFUNC_DEF("mount", 3, js_mount),
};

static int js_sys_ops_init(JSContext *ctx, JSModuleDef *m) {
    return JS_SetModuleExportList(ctx, m, js_sys_ops_funcs, countof(js_sys_ops_funcs));
}

JSModuleDef *js_init_module_js_init_module_sys_ops(JSContext *ctx, const char *module_name) {
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_sys_ops_init);
    if (!m) return NULL;
    JS_AddModuleExportList(ctx, m, js_sys_ops_funcs, countof(js_sys_ops_funcs));
    return m;
}