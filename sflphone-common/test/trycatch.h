/* Copyright (c) 2000..2009 Michael Stickel <michael@stickel.org>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you
       must not claim that you wrote the original software. If you use
       this software in a product, an acknowledgment in the product
       documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and
       must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. */

/* $Header: /code/trycatch/trycatch.h,v 1.17 2009/01/06 11:06:56 doj Exp $ */

#ifndef __TRYCATCH_H__
#define __TRYCATCH_H__

#include <setjmp.h>
#include <signal.h>

#if defined(__linux__) && !defined(__USE_GNU)
typedef __sighandler_t sighandler_t;
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__)
typedef sig_t sighandler_t;
#endif

/* library internal stuff */

/** this struct stores information about each try block. */
typedef struct
{
  sighandler_t sigs [NSIG];	/**< signal handlers which must be restored when current try block is left  */
  sigjmp_buf env;		/**< saved C environment for siglongjmp()  */
  int signal;			/**< signal number if try block was left with a signal  */
} trycatch_stack_t;

extern int __try_env_sp;
extern trycatch_stack_t *__try_stack;
extern int __catched_signal;

void __TRY_POP(void);
void __TRY_PREPARE_PUSH(void);
void __TRY_PUSH(int *sigs);
int  __TRY_CHECK_SIGNALS(int *sigs);

#define EXCEPTION(e) (NSIG+1+(e))

/* public macros and functions */

#define TRY(a...) \
  __TRY_PREPARE_PUSH(); \
  __try_stack[__try_env_sp].signal=sigsetjmp(__try_stack[__try_env_sp].env, 1); \
  if(__try_stack[__try_env_sp].signal==0) \
    { \
      int A[]={-1,##a,-1}; \
      __TRY_PUSH(&A[1]); \
    } \
  if(__try_stack[__try_env_sp].signal==0)

#define CATCH(a...) \
  __TRY_POP(); \
    { \
      int A[]={-1,##a,-1}; \
      __catched_signal=__TRY_CHECK_SIGNALS(&A[1]); \
    } \
  if(__catched_signal)

#define ELSECATCH(a...) \
    { \
      int A[]={-1,##a,-1}; \
      __catched_signal=__TRY_CHECK_SIGNALS(&A[1]); \
    } \
  if(__catched_signal)

#define TRY_ALL_SIGNALS \
  __TRY_PREPARE_PUSH(); \
  __catched_signal=__try_stack[__try_env_sp].signal=sigsetjmp(__try_stack[__try_env_sp].env, 1); \
  if(__try_stack[__try_env_sp].signal==0) \
    { \
      int A[NSIG+1], i; \
      for(i=0; i<NSIG; ++i) A[i]=i; \
      A[i]=-1; \
      __TRY_PUSH(A); \
    } \
  if(__try_stack[__try_env_sp].signal==0)

#define CATCH_ANY_SIGNAL \
  __TRY_POP(); \
  if(__catched_signal) __catched_signal=__try_stack[__try_env_sp+1].signal; \
  if(__catched_signal)


#define tryreturn(rv) {__TRY_POP(); return rv;}
#define trycontinue {__TRY_POP(); continue;}
#define trybreak {__TRY_POP(); break;}

void THROW(int sig);

#endif
