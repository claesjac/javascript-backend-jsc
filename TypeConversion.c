#include "TypeConversion.h"

static const char *GetJSObjectRefClassName(Context *ctx, JSObjectRef jsobj, JSValueRef *exception) {
    /* This is a ugly hack until JSC gets a more propriate way of retrieving a class name */
    size_t l;
    char *b, *clazz;
    JSStringRef propertyName = NULL, s = NULL;
    JSValueRef constr = NULL;

    *exception = NULL;

    /* Figure out what class this belongs to */
    propertyName = JSStringCreateWithUTF8CString("constructor");
    constr = JSObjectGetProperty(ctx->ctx, jsobj, propertyName, exception);
    if (*exception || !constr) goto CLEANUP;
    s = JSValueToStringCopy(ctx->ctx, constr, exception);
    if (*exception || !s) goto CLEANUP;

    l = JSStringGetMaximumUTF8CStringSize(s);
    Newz(0, b, l, char);
    l = JSStringGetUTF8CString(s, b, l);
    Newz(1, clazz, 256, char);
    if (sscanf(b, "function %255[^)]", clazz) < 1) {
        /* Report that we failed to get class name */
        goto CLEANUP;
    }    
    clazz[strlen(clazz) - 1] = '\0';

    return clazz;
    
CLEANUP:
    if (b)
        Safefree(b);

    if (s)
        JSStringRelease(s);

    if (propertyName)
        JSStringRelease(propertyName);
    
    return NULL;
}

/* Tmp hack */
static void ConvertJSObjectRefToSV(Context *ctx, JSObjectRef jsobj, SV **sv, JSValueRef *exception) {
    *exception = NULL;
    size_t i, c, l;
    const char *clazz = GetJSObjectRefClassName(ctx, jsobj, exception);
    char key[256];
    SV *v;
    
    if (*exception)
        goto CLEANUP;
    if (!clazz)
        goto CLEANUP;
    
    if (strEQ(clazz, "Object")) {
        if (ctx->options.convertObjects) {
            HV *hv = (HV *) sv_2mortal((SV *) newHV());

            /* Iterate over all properties */
            JSPropertyNameArrayRef properties = JSObjectCopyPropertyNames(ctx->ctx, jsobj);
            c = JSPropertyNameArrayGetCount(properties);
            for (i = 0; i < c; i++) {
                JSStringRef name = JSPropertyNameArrayGetNameAtIndex(properties, i);
                JSValueRef property = JSObjectGetProperty(ctx->ctx, jsobj, name, exception);

                l = JSStringGetUTF8CString(name, key, 256);
                v = sv_newmortal();
                ConvertJSValueRefToSV(ctx, property, &v, exception);
                hv_store(hv, key, l - 1, SvREFCNT_inc(v), 0);
                JSStringRelease(name);
            }
            JSPropertyNameArrayRelease(properties);
            
            sv_setsv(*sv, newRV_noinc((SV *) hv));
        }
    }
    else if (strEQ(clazz, "Array")) {
        
    }
    else {
        /* Box the object */
    }
    
    return;
    
CLEANUP:
    sv_setsv(*sv, &PL_sv_undef);
    if (clazz)
        Safefree(clazz);
}

void ConvertJSValueRefToSV(Context *ctx, JSValueRef jsval, SV **sv, JSValueRef *exception) {
    *exception = NULL;
    
    switch (JSValueGetType(ctx->ctx, jsval)) {        
        case kJSTypeNumber: {  
            double d = JSValueToNumber(ctx->ctx, jsval, exception);
            if (*exception) {
                sv_setsv(*sv, &PL_sv_undef);
            }
            else {
                sv_setnv(*sv, d);
            }
        }
        break;
        
        case kJSTypeString: {
            JSStringRef s = JSValueToStringCopy(ctx->ctx, jsval, exception);
            if (*exception) {
                sv_setsv(*sv, &PL_sv_undef);
            }
            else {
                size_t l = JSStringGetMaximumUTF8CStringSize(s);
                char *b;
                Newz(0, b, l, char);
                l = JSStringGetUTF8CString(s, b, l);
                JSStringRelease(s);
                sv_setpv(*sv, b);
                SvUTF8_on(*sv);
                Safefree(b);
            }
        }
        break;
        
        case kJSTypeObject: {
            JSObjectRef obj = JSValueToObject(ctx->ctx, jsval, exception);
            if (*exception) {
                fprintf(stderr, "Failed to convert to object\n");
                sv_setsv(*sv, &PL_sv_undef);
            }
            else {
                ConvertJSObjectRefToSV(ctx, obj, sv, exception);
            }
        }
        break;
            
        case kJSTypeBoolean:
            sv_setsv(*sv, JSValueToBoolean(ctx->ctx, jsval) == TRUE ? &PL_sv_yes : &PL_sv_no);
        break;
        
        case kJSTypeUndefined:
            sv_setsv(*sv, &PL_sv_undef);
        break;
        
        default:
            croak("Unable to convert type: %d to perl", JSValueGetType(ctx->ctx, jsval));
    }
}