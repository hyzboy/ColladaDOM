/*

 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#ifndef __COLLADA_DOM__DAE_WIN32_PLATFORM_H__
#define __COLLADA_DOM__DAE_WIN32_PLATFORM_H__
							
#if _MSC_VER <= 1200
#error Sorry, MSVC6 is too old for COLLADA-DOM 2.5.
typedef int intptr_t;
#elif _MSC_VER >= 1800 && _MSC_VER < 1900
#define COLLADA_MSVC2013
//TODO: Restrict to MSVC2013?
#define COLLADA_NOEXCEPT _NOEXCEPT //C2694
#endif

#define COLLADA_DOM_EXPORT __declspec(dllexport)
#define COLLADA_DOM_IMPORT __declspec(dllimport)

#define COLLADA_ALIGN(X) __declspec(align(X))

/** Ask politley for a CALL instruction. */
#define COLLADA_NOINLINE __declspec(noinline)

/** Compiler optimization hint. */
#define COLLADA_ASSUME(x) assert(x); __assume(x);

/**WARNING
 * @warning the GCC profile doesn't have this macro setup. if it can do
 * anything meaningful in this vein, then that would be a good addition.
 *
 * In theory this lets compilers hold onto/reorder exported API results. 
 * Anyway, how it's being used, is to mark COLLADA_DOM_LINKAGE APIs that
 * don't really alter anything.
 *
 * @remarks It's very hard to tell what "noalias" is meant to address, but
 * "I've" at least seen instances of it in examples that decorate external
 * declarations and no-argument functions. A document called "Optimization
 * Best Practices" seems to be a clearer guide than the "noalias" document.
 */
#define COLLADA_NOALIAS __declspec(noalias)

//TODO: How to move to WARNING.HPP with scoping?
//let the warnings fly when doing Release builds
#ifdef _DEBUG 
/** Suppresses Visual Studio's warnings. */
#define COLLADA_SUPPRESS_C(xxxx) __pragma(warning(suppress:xxxx))
//NOTE: COLLADA_SUPPRESS_C doesn't work with these warnings:
//warning C4520: "inherits via dominance" There's no spot fix.
//warning C4351: Array 0 initialization going back to MVSC2005.
//warning C4554: Arbitrary precedence warning on shift operator.
//warning C4224: Forbids "formal parameter" using name of a type.
//warning C4522: Warns if const applies to operator=() definition.
#pragma warning(disable:4250 4351 4554 4224 4522)
#define COLLADA_DEPRECATED(hint) __declspec(deprecated(hint))
//std::equal/copy are used by ZAE's CPP files.
#ifdef BUILDING_COLLADA_DOM
#define _SCL_SECURE_NO_WARNINGS
#endif
#endif //_DEBUG

#ifndef COLLADA_H
#define COLLADA_H(h) __pragma(message("COLLADA-DOM: In " COLLADA_STRINGIZE(h) " ..."))
#endif

#endif //__COLLADA_DOM__DAE_WIN32_PLATFORM_H__
/*C1071*/
