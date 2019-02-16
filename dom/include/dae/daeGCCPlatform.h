/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_GCC_PLATFORM_H__
#define __COLLADA_DOM__DAE_GCC_PLATFORM_H__
 							
#ifdef __CYGWIN__
#define COLLADA_DOM_EXPORT __attribute__((__dllexport__))
#define COLLADA_DOM_IMPORT __attribute__((__dllimport__))
#else
#define COLLADA_DOM_EXPORT __attribute__((__visibility__("default")))
#define COLLADA_DOM_IMPORT __attribute__((__visibility__("default")))
//Can't roll into COLLADA_DOM_EXPORT if data-export:
//warning: __used__ attribute ignored [-Wattributes]
#define COLLADA_USED __attribute__((__used__))
#endif

#define COLLADA_ALIGN(X) __attribute__((__aligned__(X)))

/** Ask politley for a CALL instruction. */
#define COLLADA_NOINLINE __attribute__((__noinline__))

#define COLLADA_DEPRECATED(hint) __attribute__((__deprecated__(hint)))

//NOTE: It doesn't work to hand _Pragma a string inline.
#define COLLADA_GCC_PRAGMA(x) _Pragma(#x)

//EXPERIMENTAL
#ifndef COLLADA_H
#define COLLADA_H(h) COLLADA_GCC_PRAGMA(message ("COLLADA-DOM: In " COLLADA_STRINGIZE(h) " ..."))
#endif

#endif //__COLLADA_DOM__DAE_GCC_PLATFORM_H__
/*C1071*/
