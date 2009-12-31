#ifndef __ARRAY_H__
#define __ARRAY_H__

#ifdef _cplusplus
extern "C"
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "Context.h"
#include "Object.h"
#include <JavaScriptCore/JavaScriptCore.h>

void CreateArray(Context *, JSValueRef, JSObjectRef, SV **);
void DestroyArray(Object *);

#ifdef _cplusplus
}
#endif

#endif