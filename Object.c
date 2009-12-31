#include "Object.h"

void CreateObject(Context *ctx, JSValueRef value, JSObjectRef object, SV **sv) {
    Object *o;
    
    Newz(1, o, 1, Object);
    
    /* Protect value from being GCd */
    o->object = object;
    o->value = value;
    JSValueProtect(ctx->ctx, value);

    /* Increase refcount on the context so it won't get destoyed prematurely */
    o->ctx = ctx;
    SvREFCNT_inc(ctx->sv);
    
    /* Next, create a SV that holds this */
    *sv = sv_newmortal();
    sv_setref_pv(*sv, "JavaScript::Backend::JSC::Object", (void *) o);
}

void DestroyObject(Object *object) {
    JSValueUnprotect(object->ctx->ctx, object->value);
    SvREFCNT_dec(object->ctx->sv);
    
    Safefree(object);
}