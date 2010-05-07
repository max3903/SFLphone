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

/* $Header: /code/trycatch/trycatch.c,v 1.18 2009/01/03 06:42:40 doj Exp $ */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "trycatch.h"

/*lint -save -e1924 allow C-style casts*/

/** this function is invoked when a signal is emmited in the TRY block.
    @param sig the signal number
*/
void THROW(int sig)
{
  siglongjmp (__try_stack[__try_env_sp].env, sig); /* does not return */
}


/*****************************************************************************/
/* private section */
/*****************************************************************************/

/** global stack pointer for __try_stack */
int __try_env_sp=0;
/** global stack to store signals and environment for TRY/CATCH blocks */
trycatch_stack_t *__try_stack=0;
/** contains the signal number in CATCH blocks */
int __catched_signal=0;

/** rewinds the __try_stack. <b>internal use only!</b> */
void __TRY_POP()
{
  /*lint --e{534,611} ignore return value, ignore suspicious cast*/

  int i;

  for(i=0; i<NSIG; i++)
    if(__try_stack[__try_env_sp].sigs[i] != 0)
      signal(i, __try_stack[__try_env_sp].sigs[i]);

  if(--__try_env_sp<0)
    {
      syslog(LOG_EMERG, "trycatch: try stack underflow\n");
      exit(EXIT_FAILURE);
    }
}

/** checks if another try block can be allocated. program is terminated
    if this is not possible */
void __TRY_PREPARE_PUSH()
{
  static int stack_size=1;

  if(++__try_env_sp>=stack_size)
    {
      stack_size*=2;
      /*lint --e{737} ignore sign loss*/
      __try_stack=(trycatch_stack_t*) realloc(__try_stack, stack_size*sizeof(trycatch_stack_t));
      if(__try_stack==NULL)
	{
	  syslog(LOG_EMERG, "trycatch: realloc failed: %s\n", strerror(errno));
	  exit(EXIT_FAILURE);
	}

      syslog(LOG_INFO, "trycatch: stack realloc %i -> %i (%u bytes)\n", stack_size/2, stack_size, (unsigned)(stack_size*sizeof(trycatch_stack_t)));
    }
  syslog(LOG_DEBUG, "trycatch: stack depth %i\n", __try_env_sp);
}

/*lint -esym(818,sigs) ignore sigs beeing non const*/
/** pushes the signals onto the stack
    @param sigs the signals to try (watch)
*/
void __TRY_PUSH(int *sigs)
{
  /*lint --e{611} ignore suspicious cast*/
  int i;
  for(i=0; i<NSIG; i++)
    __try_stack[__try_env_sp].sigs[i]=0;

  while(*sigs != -1)
    {
      const int s=*sigs++;
      if(s<NSIG && s>0)
	__try_stack[__try_env_sp].sigs[s]=signal(s, THROW);
    }
}

/** checks if the signal which was received is included in sigs
    @param sigs list of sigs to check against the received signal
    @return 1 if included, 0 if not
*/
int __TRY_CHECK_SIGNALS(int *sigs)
{
  const int s=__try_stack[__try_env_sp+1].signal;
  while(*sigs != -1)
    if(*sigs++ == s)
      return s;

  return 0;
}

/*lint -restore*/
