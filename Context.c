#include "Context.h"

Context *CreateContext(const char *name) {
    Context *ctx;
    
    Newz(0, ctx, 1, Context);
    if (!ctx) {
        goto CLEANUP;
    }
    
    ctx->ctx = JSGlobalContextCreate(NULL);
    if (!ctx->ctx) {
        goto CLEANUP;
    }
        
    ctx->name = savepv(name);
    if (!ctx->name) {
        goto CLEANUP;
    }
    
    ctx->options.convertObjects = FALSE;
    ctx->options.convertArrays = FALSE;

    return ctx;
    
CLEANUP:
    if (ctx && ctx->name) {
        Safefree(ctx->name);
    }
    if (ctx && ctx->ctx) {
        JSGlobalContextRelease(ctx->ctx);
    }
    if (ctx) {
        Safefree(ctx);
    }
    return NULL;
}

void SetContextOption(Context *ctx, const char *name, SV *value) {
    if (strEQ(name, "ConvertObjects")) {
        ctx->options.convertObjects = SvTRUE(value) ? TRUE : FALSE;
    }
    else if (strEQ(name, "ConvertArrays")) {
        ctx->options.convertArrays = SvTRUE(value) ? TRUE : FALSE;        
    }
}

void SetContextOptions(Context *ctx, HV *options) {
    I32 c, l;
    HE *entry;
    const char *option;
    SV *value;
    
    c = hv_iterinit(options);
    
    while ((entry = hv_iternext(options)) != NULL) {
        option = hv_iterkey(entry, &l);
        value = hv_iterval(options, entry);
        SetContextOption(ctx, option, value);
    }
}

void GetContextOption(Context *ctx, const char *name, SV **v) {    
    if (strEQ(name, "ConvertObjects")) {
        *v = ctx->options.convertObjects == TRUE ? &PL_sv_yes : &PL_sv_no;
    }
    else if (strEQ(name, "ConvertArrays")) {
         *v = ctx->options.convertArrays ? &PL_sv_yes : &PL_sv_no;
    }
    else {
        *v = &PL_sv_undef;
    }
}