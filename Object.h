#ifndef __OBJECT_H__
#define __OBJECT_H__

#ifdef _cplusplus
extern "C"
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "Context.h"
#include <JavaScriptCore/JavaScriptCore.h>

struct Object {
    Context *   ctx;
    JSValueRef  value;
    JSObjectRef object;
};

typedef struct Object Object;

void CreateObject(Context *, JSValueRef, JSObjectRef, SV **);
void DestroyObject(Object *);

#ifdef _cplusplus
}
#endif

#endif