#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#ifdef _cplusplus
extern "C"
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <JavaScriptCore/JavaScriptCore.h>

#define HAS_CONTEXT(self, name) hv_exists((HV *) SvRV(self), name, strlen(name))

struct ContextOptions {
    /* Determines wheter Object should be translated to perl hashes or 
       boxed as JavaScript::Object
    */
    bool    convertObjects;

    /* Determines wheter Array should be translated to perl array or 
       boxed as JavaScript::Array
    */
    bool    convertArrays;
};

typedef struct ContextOptions ContextOptions;

struct Context {
    SV *                sv;
    JSGlobalContextRef  ctx;
    const char *        name;
  
    ContextOptions      options;
};

typedef struct Context Context;

extern Context *CreateContext(const char *);
extern void SetContextOption(Context *, const char *, SV *);
extern void SetContextOptions(Context *, HV *);
extern void GetContextOption(Context *, const char *, SV **);

#ifdef _cplusplus
}
#endif

#endif