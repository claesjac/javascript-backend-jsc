#include "TypeConversion.h"

void ConvertJSValueRefToSV(Context * ctx, JSValueRef jsval, SV **sv, JSValueRef *exception) {
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