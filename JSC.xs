#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "Context.h"
#include "TypeConversion.h"
#include <JavaScriptCore/JavaScriptCore.h>

typedef Context * JavaScript__Backend__JSC__Context;

SV *gSharedRuntime = NULL;


MODULE = JavaScript::Backend::JSC       PACKAGE = JavaScript::Backend::JSC::Runtime

SV *
new(pkg)
    const char *pkg;
    PREINIT:
        HV *contexts;
    CODE:
        if (!gSharedRuntime) {
            /* Create runtime here */
            contexts = (HV *) sv_2mortal((SV *) newHV());
            gSharedRuntime = newRV((SV *) contexts);
            sv_bless(gSharedRuntime, gv_stashpv(pkg, GV_ADD));
            SvREFCNT_inc(gSharedRuntime);
        }
        RETVAL = gSharedRuntime;
    OUTPUT:
        RETVAL

bool
has_context(self, name)
    SV *self;
    const char *name;
    CODE:
    RETVAL = HAS_CONTEXT(self, name);
    OUTPUT:
        RETVAL

JavaScript::Backend::JSC::Context
_create_named_context(self, name)
    SV *self;
    const char *name;
    PREINIT:
        Context *ctx;
    CODE:
        if (HAS_CONTEXT(self, name)) {
            croak("A context with the name '%s' already exists in this runtime", name);
        }
        ctx = CreateContext(name);
        if (ctx == NULL) {
            
        }
        RETVAL = ctx;
    OUTPUT:
        RETVAL
        
MODULE = JavaScript::Backend::JSC       PACKAGE = JavaScript::Backend::JSC::Context

void
DESTROY(self)
    JavaScript::Backend::JSC::Context self;
    CODE:
        if (self && self->ctx) {
            JSGlobalContextRelease(self->ctx);
            self->ctx = NULL;
        }
        if (self && self->name) {
            Safefree(self->name);
            self->name = NULL;
        }

void
set_option(self, option, value)
    JavaScript::Backend::JSC::Context self;
    const char * option;
    SV * value;
    CODE:
        SetContextOption(self, option, value);
    
SV *
get_option(self, option)
    JavaScript::Backend::JSC::Context self;
    const char * option;
    PREINIT:
        SV * s;
    CODE:
        s = sv_newmortal();
        GetContextOption(self, option, &s);
        RETVAL = s;
    OUTPUT:
        RETVAL
        
JSValueRef
eval(self,source,options=NULL)
    JavaScript::Backend::JSC::Context self;
    SV *source;
    HV *options;
    PREINIT:
        JSValueRef      exception = NULL;
        JSStringRef     script;
        JSObjectRef     thisObject;
        JSStringRef     sourceURL;
        int             startingLineNumber;
        JSValueRef      result;
        SV *            r;
        ContextOptions  savedOptions;
    CODE:
        if (!SvPOK(source)) {
            croak("Invalid source, can't eval");
        }
        /* JSC always assumes UTF8 source so upgrade if necessary */
        if (SvUTF8(source)) {
            script = JSStringCreateWithUTF8CString(SvPVutf8_nolen(source));
        }
        else {
            SV *c = sv_2mortal(newSVsv(source));
            script = JSStringCreateWithUTF8CString(SvPVutf8_nolen(c));
        }
        
        if (options != NULL) {
            /* Make a dup of the the options from the context */
            Copy(&(self->options), &savedOptions, 1, ContextOptions);
            SetContextOptions(self, options);
        }
        
        result = JSEvaluateScript(self->ctx, script, NULL, NULL, 0, &exception);
        
        if (exception) {
            /* An exception was thrown, wrap this */
            r = sv_newmortal();
//            ConvertJSValueRefToSV(self, exception, &r, NULL);
            sv_setsv(ERRSV, r);
            XSRETURN_UNDEF;
        }
        
        /* Convert js value to perl */
        RETVAL = result;
        JSStringRelease(script);
    OUTPUT:
        RETVAL
    CLEANUP:
        if (options != NULL) {
            /* Restore options */
            Copy(&savedOptions, &(self->options), 1, ContextOptions);
        }
        
MODULE = JavaScript::Backend::JSC		PACKAGE = JavaScript::Backend::JSC		

