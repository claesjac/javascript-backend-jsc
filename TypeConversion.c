#include "TypeConversion.h"
#include "Object.h"
#include "Array.h"

#define CONSTRUCTOR_PROPERTY "constructor"
#define LENGTH_PROPERTY "length"

static const char *GetJSObjectRefClassName(Context *ctx, JSObjectRef jsobj, JSValueRef *exception) {
    /* This is a ugly hack until JSC gets a more propriate way of retrieving a class name.
        In short, get the constructor and stringify it.. that'll output something 
        like "function Foo()...". Then scanf it to get the part after function */
    
    size_t l;
    char *b, *clazz;
    JSStringRef propertyName = NULL, s = NULL;
    JSValueRef constr = NULL;

    *exception = NULL;

    /* Figure out what class this belongs to */
    propertyName = JSStringCreateWithUTF8CString(CONSTRUCTOR_PROPERTY);
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
    
    /* scanf with %[] includes the first non-matching character which is why
       we need to move the NULL terminator */
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
static void ConvertJSObjectRefToSV(Context *ctx, JSValueRef value, SV **sv, JSValueRef *exception) {
    *exception = NULL;
    size_t i, c, l;
    const char *clazz;
    char key[256];
    SV *v;
    
    JSObjectRef jsobj = JSValueToObject(ctx->ctx, value, exception);
    if (*exception) {
        fprintf(stderr, "Failed to convert to object\n");
        sv_setsv(*sv, &PL_sv_undef);
    }

    clazz = GetJSObjectRefClassName(ctx, jsobj, exception);    
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
            }
            JSPropertyNameArrayRelease(properties);
            
            sv_setsv(*sv, newRV_noinc((SV *) hv));
        }
        else {
            CreateObject(ctx, value, jsobj, sv);
        }
    }
    else if (strEQ(clazz, "Array")) {
        if (ctx->options.convertArrays) {
            unsigned idx;
            JSStringRef lengthProperty = JSStringCreateWithUTF8CString(LENGTH_PROPERTY);
            JSValueRef length = JSObjectGetProperty(ctx->ctx, jsobj, lengthProperty, exception);
            JSStringRelease(lengthProperty);

            l = (size_t) JSValueToNumber(ctx->ctx, length, exception);
            
            AV *av = (AV *) sv_2mortal((SV *) newAV());
            av_extend(av, (I32) l);
            
            for (idx = 0; idx < l; idx++) {
                JSValueRef property = JSObjectGetPropertyAtIndex(ctx->ctx, jsobj, idx, exception);
                v = sv_newmortal();
                ConvertJSValueRefToSV(ctx, property, &v, exception);
                av_push(av, SvREFCNT_inc(v));
            }
            
            sv_setsv(*sv, newRV_noinc((SV *) av));
        }
        else {
            CreateArray(ctx, value, jsobj, sv);
        }
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

void ConvertJSValueRefToSV(Context *ctx, JSValueRef value, SV **sv, JSValueRef *exception) {
    *exception = NULL;
    
    switch (JSValueGetType(ctx->ctx, value)) {        
        case kJSTypeNumber: {  
            double d = JSValueToNumber(ctx->ctx, value, exception);
            if (*exception) {
                sv_setsv(*sv, &PL_sv_undef);
            }
            else {
                sv_setnv(*sv, d);
            }
        }
        break;
        
        case kJSTypeString: {
            JSStringRef s = JSValueToStringCopy(ctx->ctx, value, exception);
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
        
        case kJSTypeObject:
            ConvertJSObjectRefToSV(ctx, value, sv, exception);
        break;
            
        case kJSTypeBoolean:
            sv_setsv(*sv, JSValueToBoolean(ctx->ctx, value) == TRUE ? &PL_sv_yes : &PL_sv_no);
        break;
        
        case kJSTypeUndefined:
            sv_setsv(*sv, &PL_sv_undef);
        break;
        
        default:
            croak("Unable to convert type: %d to perl", JSValueGetType(ctx->ctx, value));
    }
}