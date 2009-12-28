#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <JavaScriptCore/JavaScriptCore.h>

#define HAS_CONTEXT(self, name) hv_exists((HV *) SvRV(self), name, strlen(name))

struct Context {
    const char *name;
};

typedef struct Context Context;

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
        Newz(0, ctx, 1, Context);
        ctx->name = savepv(name);
        RETVAL = ctx;
    OUTPUT:
        RETVAL
        
void
DESTROY(self)
    SV *self;
    CODE:
        {
        }

MODULE = JavaScript::Backend::JSC       PACKAGE = JavaScript::Backend::JSC::Context

void
DESTROY(self)
    JavaScript::Backend::JSC::Context self;
    CODE:
        if (self && self->name) {
            Safefree(self->name);
            self->name = NULL;
        }
        
MODULE = JavaScript::Backend::JSC		PACKAGE = JavaScript::Backend::JSC		

