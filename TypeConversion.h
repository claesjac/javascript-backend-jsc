#ifndef __TYPECONVERSION_H__
#define __TYPECONVERSION_H__

#ifdef _cplusplus
extern "C"
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "Context.h"
#include <JavaScriptCore/JavaScriptCore.h>

extern void ConvertJSValueRefToSV(Context *, JSValueRef, SV **, JSValueRef *);

#ifdef _cplusplus
}
#endif

#endif