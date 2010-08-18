/** 
*	2010/08/18
*	Modified by Jeremy Palvadeau <jeremy.palvadeau@savoirfairelinux.com>
*	Modified to make an exemple of DSP use in SFLphone.
*	This file come from ti-dsplink source, please see wiki for more information
*	https://projects.savoirfairelinux.com/wiki/sflphone-c64xplus/SflPhone64x+
*
*  ============================================================================
*  @file   scale.c
*
*  @path   $(DSPLINK)/gpp/src/samples/scale/
*
*  @desc   This is a scale application which sends a buffer across the DSP
*          processor and recieves a buffer back using DSP/BIOS LINK. The DSP
*          scales the returned buffer based on the control message sent by the
*          GPP. The data buffer received is verified aginst the data sent to
*          DSP. This application transfers only one buffer and no buffer(s) are
*          queued.
*
*  @ver    1.64
*  ============================================================================
*  Copyright (C) 2002-2009, Texas Instruments Incorporated -
*  http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*  
*  *  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  
*  *  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  
*  *  Neither the name of Texas Instruments Incorporated nor the names of
*     its contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
*  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
*  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
*  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*  ============================================================================
*/


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <chnl.h>
#include <msgq.h>
#include <pool.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- Application Header              */
#include <scale.h>
#include <scale_os.h>
#include <sfldsp.h>

/*  ----------------------------------- Timer */
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
*  @const  NUM_ARGS
*
*  @desc   Number of arguments specified to the DSP application.
*  ============================================================================
*/
#define NUM_ARGS             2

/** ============================================================================
*  @name   SAMPLE_POOL_ID
*
*  @desc   ID of the POOL used for the sample.
*  ============================================================================
*/
#define SAMPLE_POOL_ID       0

/** ============================================================================
*  @name   NUM_BUF_SIZES
*
*  @desc   Number of buffer pools to be configured for the allocator.
*  ============================================================================
*/
#define NUM_BUF_SIZES        4

/** ============================================================================
*  @const  NUM_BUF_POOL0
*
*  @desc   Number of buffers in first buffer pool.
*  ============================================================================
*/
#define NUM_BUF_POOL0        1

/** ============================================================================
*  @const  NUM_BUF_POOL1
*
*  @desc   Number of buffers in second buffer pool.
*  ============================================================================
*/
#define NUM_BUF_POOL1        2

/** ============================================================================
*  @const  NUM_BUF_POOL2
*
*  @desc   Number of buffers in third buffer pool.
*  ============================================================================
*/
#define NUM_BUF_POOL2        1

/** ============================================================================
*  @name   NUM_BUF_POOL3
*
*  @desc   Number of buffers in fourth buffer pool.
*  ============================================================================
*/
#define NUM_BUF_POOL3        3

/** ============================================================================
*  @name   SAMPLE_CTRLMSG_SIZE
*
*  @desc   Control message size.
*  ============================================================================
*/
#if defined (ZCPY_LINK)
#define SAMPLE_CTRLMSG_SIZE ZCPYMQT_CTRLMSG_SIZE
#endif /* if defined (ZCPY_LINK) */


/*  ============================================================================
*  @name   XFER_CHAR
*
*  @desc   The value used to initialize the output buffer and used for
*          validation against the input buffer received.
*  ============================================================================
*/
#define XFER_CHAR            0x1

/** ============================================================================
*  @const  SCALE_REQUEST_MSGID
*
*  @desc   Message id to denote a scaling change.
*  ============================================================================
*/
#define SCALE_REQUEST_MSGID  1

/** ============================================================================
*  @const  APP_MSG_SIZE
*
*  @desc   Messaging buffer used by the application.
*          Note: This buffer is aligned according to the alignment expected
*          by the platform.
*  ============================================================================
*/
#define APP_MSG_SIZE  DSPLINK_ALIGN (sizeof (SCALE_ScaleMsg), DSPLINK_BUF_ALIGN)

#if defined (DA8XXGEM)
/** ============================================================================
*  @name   dspAddr
*
*  @desc   Address of c_int00 in the DSP executable.
*  ============================================================================
*/
Uint32 SCALE_dspAddr ;

/** ============================================================================
*  @name   shmAddr
*
*  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
*  ============================================================================
*/
Uint32 SCALE_shmAddr ;

/** ============================================================================
*  @name   argsAddr
*
*  @desc   Address of .args section in the DSP executable.
*  ============================================================================
*/
Uint32 SCALE_argsAddr ;

/** ============================================================================
*  @name   LINKCFG_config
*
*  @desc   Extern declaration to the default DSP/BIOS LINK configuration
*          structure.
*  ============================================================================
*/
extern  LINKCFG_Object LINKCFG_config ;
#endif


/** ============================================================================
*  @name   SCALE_ScaleMsg
*
*  @desc   Structure used to pass the scaling factor from the GPP to the DSP.
*
*  @field  msgHeader
*              Required first field of a message.
*  @field  scalingFactor
*              Used to scale the output buffer values.
*  ============================================================================
*/
typedef struct SCALE_ScaleMsg_tag {
	MSGQ_MsgHeader  msgHeader     ;
	double          scalingFactor ;
} SCALE_ScaleMsg ;


/*  ============================================================================
*  @name   SCALE_XferValue
*
*  @desc   The scaling factor for the returned buffer values.
*  ============================================================================
*/
STATIC double  SCALE_XferValue = 1 ;

/*  ============================================================================
*  @name   SCALE_BufferSize
*
*  @desc   Size of buffer to be used for data transfer.
*  ============================================================================
*/
STATIC Uint32  SCALE_BufferSize ;

/*  ============================================================================
*  @name   SCALE_NumIterations
*
*  @desc   Size of buffer to be used for data transfer.
*  ============================================================================
*/
STATIC Uint32  SCALE_NumIterations ;

/** ============================================================================
*  @name   SCALE_Buffers
*
*  @desc   Array of buffers used by input channel.
*          Length of array in this application is 1.
*  ============================================================================
*/
STATIC Char8 * SCALE_Buffers [1] = { NULL };

/** ============================================================================
*  @name   SCALE_IOReq
*
*  @desc   It gives information for adding or reclaiming an input request.
*  ============================================================================
*/
STATIC ChannelIOInfo SCALE_IOReq  ;

/** ============================================================================
*  @const  MsgqDsp1
*
*  @desc   Name of the first MSGQ on the DSP.
*  ============================================================================
*/
STATIC Char8 SampleDspMsgqName [DSP_MAX_STRLEN] = "DSPMSGQ1" ;

/** ============================================================================
*  @name   SampleDspMsgq
*
*  @desc   DSP's MSGQ Object.
*  ============================================================================
*/
STATIC MSGQ_Queue SampleDspMsgq = (Uint32) MSGQ_INVALIDMSGQ ;


#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
*  @func   SCALE_VerifyData
*
*  @desc   This function verifies the data-integrity of given buffer.
*
*  @arg    buf
*              This is the pointer of the buffer whose contents are to be
*              validated.
*
*  @ret    DSP_SOK
*              Operation successfully completed.
*          DSP_EFAIL
*              Contents of the input buffer and the output buffer are
*              different.
*
*  @enter  None
*
*  @leave  None
*
*  @see    SCALE_XferValue
*  ----------------------------------------------------------------------------
*/
STATIC
NORMAL_API
DSP_STATUS
SCALE_VerifyData (IN Char8 * buf) ;
#endif /*  defined (VERIFY_DATA) */



/** ============================================================================
*  @func   SCALE_Create
*
*  @desc   This function allocates and initializes resources used by
*          this application.
*
*  @modif  SCALE_Buffers , SCALE_Buffers
*  ============================================================================
*/
NORMAL_API
DSP_STATUS
SCALE_Create (IN Char8 * dspExecutable,
			IN Char8 * strBufferSize,
			IN Char8 * strNumIterations,
			IN Uint8   processorId)
{
	DSP_STATUS    status                  = DSP_SOK ;
	Uint32        numArgs                 = 0 ;
	Uint32        size    [NUM_BUF_SIZES] = {APP_MSG_SIZE,
											SAMPLE_CTRLMSG_SIZE,
											DSPLINK_ALIGN (
												sizeof (MSGQ_AsyncLocateMsg),
												DSPLINK_BUF_ALIGN)} ;
	Uint32        numBufs [NUM_BUF_SIZES] = {NUM_BUF_POOL0,
											NUM_BUF_POOL1,
											NUM_BUF_POOL2,
											NUM_BUF_POOL3} ;
	ChannelAttrs  chnlAttrInput ;
	ChannelAttrs  chnlAttrOutput ;
	Char8       * args [NUM_ARGS] ;
#if defined (ZCPY_LINK)
	SMAPOOL_Attrs poolAttrs ;
	ZCPYMQT_Attrs mqtAttrs ;
#endif /* if defined (ZCPY_LINK) */
#if defined (PCPY_LINK)
	BUFPOOL_Attrs poolAttrs ;
	PCPYMQT_Attrs mqtAttrs ;
#endif /* if defined (PCPY_LINK) */
#if defined (DA8XXGEM)
	NOLOADER_ImageInfo  imageInfo ;
#endif


	printf ("Entered SCALE_Create ()\n") ;

	/*
	*  Create and initialize the proc object.
	*/
	status = PROC_setup (NULL) ;

	/*
	*  Attach the Dsp with which the transfers have to be done.
	*/
	if (DSP_SUCCEEDED (status)) {
		status = PROC_attach (processorId, NULL) ;
		if (DSP_FAILED (status)) {
			printf ("PROC_attach () failed. Status = [0x%x]\n",
							status) ;
		}
	}
	else {
		printf ("PROC_setup () failed. Status = [0x%x]\n", status) ;
	}

	/*
	*  Open the pool.
	*/
	if (DSP_SUCCEEDED (status)) {
		size [3] = SCALE_BufferSize ;
		poolAttrs.bufSizes      = (Uint32 *) &size ;
		poolAttrs.numBuffers    = (Uint32 *) &numBufs ;
		poolAttrs.numBufPools   = NUM_BUF_SIZES ;
#if defined (ZCPY_LINK)
		poolAttrs.exactMatchReq = TRUE ;
#endif /* if defined (ZCPY_LINK) */
		status = POOL_open (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
							&poolAttrs) ;
		if (DSP_FAILED (status)) {
			printf ("POOL_open () failed. Status = [0x%x]\n", status) ;
		}
	}

	/*
	*  Load the executable on the DSP.
	*/
	if (DSP_SUCCEEDED (status)) {
		numArgs  = NUM_ARGS         ;
		args [0] = strBufferSize    ;
		args [1] = strNumIterations ;

#if defined (DA8XXGEM)
		if  (    (LINKCFG_config.
				dspConfigs [processorId]->dspObject->doDspCtrl)
			==  DSP_BootMode_NoBoot) {
			imageInfo.dspRunAddr  = SCALE_dspAddr ;
			imageInfo.shmBaseAddr = SCALE_shmAddr ;
			imageInfo.argsAddr    = SCALE_argsAddr ;
			imageInfo.argsSize    = 50 ;
			status = PROC_load (processorId, (Char8 *) &imageInfo, numArgs, args) ;
		}
		else
#endif
		{
			status = PROC_load (processorId, dspExecutable, numArgs, args) ;
		}
		if (DSP_FAILED (status)) {
			printf ("PROC_load () failed. Status = [0x%x]\n", status) ;
		}
	}

	/*
	*  Create a channel to DSP
	*/
	if (DSP_SUCCEEDED (status)) {
		chnlAttrOutput.mode      = ChannelMode_Output     ;
		chnlAttrOutput.endianism = Endianism_Default      ;
		chnlAttrOutput.size      = ChannelDataSize_16bits ;

		status = CHNL_create (processorId, CHNL_ID_OUTPUT, &chnlAttrOutput) ;
		if (DSP_FAILED (status)) {
			printf ("CHNL_create () failed. Status = [0x%x]\n",
							status) ;
		}
	}

	/*
	*  Create a channel from DSP
	*/
	if (DSP_SUCCEEDED (status)) {
		chnlAttrInput.mode      = ChannelMode_Input      ;
		chnlAttrInput.endianism = Endianism_Default      ;
		chnlAttrInput.size      = ChannelDataSize_16bits ;

		status = CHNL_create (processorId, CHNL_ID_INPUT, &chnlAttrInput) ;
		if (DSP_FAILED (status)) {
			printf ("CHNL_create () failed. Status = [0x%x]\n",
							status) ;
		}
	}

	/*
	*  Allocate buffer(s) for data transfer to DSP.
	*/
	if (DSP_SUCCEEDED (status)) {
		status = CHNL_allocateBuffer (processorId,
									CHNL_ID_OUTPUT,
									SCALE_Buffers,
									SCALE_BufferSize ,
									1) ;
		if (DSP_FAILED (status)) {
			printf ("CHNL_allocateBuffer () failed. Status = [0x%x]\n",
							status) ;
		}
	}
	
	/*
	*  Start execution on DSP.
	*/
	if (DSP_SUCCEEDED (status)) {
		status = PROC_start (processorId) ;
		if (DSP_FAILED (status)) {
			printf ("PROC_start () failed. Status = [0x%x]\n",
							status) ;
		}
	}

	/*
	*  Open the remote transport.
	*/
	if (DSP_SUCCEEDED (status)) {
		mqtAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID) ;
		status = MSGQ_transportOpen (processorId, &mqtAttrs) ;
		if (DSP_FAILED (status)) {
			printf ("MSGQ_transportOpen () failed. Status = [0x%x]\n",
							status) ;
		}
	}

	printf ("Leaving SCALE_Create ()\n") ;

	return status ;
}


/** ============================================================================
*  @func   SCALE_Execute
*
*  @desc   This function implements the execute phase for this application.
*
*  @modif  None
*  ============================================================================
*/
NORMAL_API
DSP_STATUS
SCALE_Execute (IN Uint32 numIterations, Uint8 processorId)
{
	DSP_STATUS       status = DSP_ENOTFOUND ;
//	Uint8 *          ptr8   = NULL          ;
//	Uint16 *         ptr16  = NULL          ;
//	double *         ptr32  = NULL	    ;
	MSGQ_LocateAttrs syncLocateAttrs        ;
	SCALE_ScaleMsg * msg                    ;

	unsigned int i,j;
	short  tab[1024];
	short  tab2[1024];
	printf ("Entered SCALE_Execute version Check\n") ;


	/*
	*  Locate the DSP's message queue
	*/
	syncLocateAttrs.timeout = WAIT_FOREVER;
	while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
		status = MSGQ_locate (SampleDspMsgqName,
							&SampleDspMsgq,
							&syncLocateAttrs) ;
		if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
			usleep (100000) ;
		}
		else if (DSP_FAILED (status)) {
			printf ("MSGQ_locate () failed. Status = [0x%x]\n", status) ;
		}
	}

	/*
	*  Fill the IO Request structure
	*  It gives Information for adding or reclaiming an input request.
	*/
	if (DSP_SUCCEEDED (status)) {
		SCALE_IOReq.buffer = SCALE_Buffers [0] ;
		SCALE_IOReq.size   = SCALE_BufferSize   ;
	}

	/* 
	*  Send the scaling factor 
	*/
	status = MSGQ_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
							APP_MSG_SIZE,
							(MSGQ_Msg *) &msg) ;
	if (DSP_SUCCEEDED (status)) {
		msg->scalingFactor = SCALE_XferValue ;
		/* Send the allocated message */
		MSGQ_setMsgId ((MSGQ_Msg) msg, SCALE_REQUEST_MSGID) ;
		status = MSGQ_put (SampleDspMsgq, (MSGQ_Msg) msg) ;
		if (DSP_SUCCEEDED (status)) {
			printf ("Changed the scale factor to: %f\n",
							SCALE_XferValue) ;
		}
		else {
			printf ("MSGQ_put () failed. Status = [0x%x]\n",
							status) ;
		}
	}
	else {
		printf ("MSGQ_alloc () failed. Status = [0x%x]\n",
						status) ;
	}

	printf("SCALE_BufferSize : %d\n", SCALE_BufferSize);

	for(i=0;i<SCALE_BufferSize/2;i++)
	{
		tab[i]=i;
	}
	/*
	for(i=0;i<128;i++)
	{
		printf("envoyée: %d\n",tab[i]);
	}*/

	for (i = 1 ;
			(  (SCALE_NumIterations == 0) || (i <= SCALE_NumIterations))
			&&(DSP_SUCCEEDED (status)) ;
				i++) {
	//while(DSP_SUCCEEDED (status)){
				

		sfl_dsp_process(tab,tab2, SCALE_BufferSize/2);



#if defined (VERIFY_DATA)
		/*
		*  Verify correctness of data received.
		*/
//		if (DSP_SUCCEEDED (status)) {
//			status = SCALE_VerifyData (SCALE_IOReq.buffer) ;
//		}

#endif
	//	if (DSP_SUCCEEDED (status) /*&& (i % 100)*/ == 0) {
	//		printf ("Transferred %d buffers\n", i) ;
	//	}
		for(j=0;j<SCALE_BufferSize/2;j++)
				{
					printf("envoyée %d | recu: %d\n",tab[j],tab2[j]);
				}
	}


	/*
	*  Release the remote message queue
	*/
	status = MSGQ_release (SampleDspMsgq) ;
	if (DSP_FAILED (status)) {
		printf ("MSGQ_release () failed. Status = [0x%x]\n", status) ;
	}


	printf ("Leaving SCALE_Execute ()\n") ;

	return status ;
}


/** ============================================================================
*  @func   SCALE_Delete
*
*  @desc   This function releases resources allocated earlier by call to
*          SCALE_Create ().
*          During cleanup, the allocated resources are being freed
*          unconditionally. Actual applications may require stricter check
*          against return values for robustness.
*
*  @modif  SCALE_Buffers , SCALE_Buffers
*  ============================================================================
*/
NORMAL_API
Void
SCALE_Delete (Uint8 processorId)
{
	DSP_STATUS status    = DSP_SOK ;
	DSP_STATUS tmpStatus = DSP_SOK ;

	printf ("Entered SCALE_Delete ()\n") ;

	/*
	*  Close the remote transport
	*/
	status = MSGQ_transportClose (processorId) ;
	if (DSP_FAILED (status)) {
		printf ("MSGQ_transportClose () failed. Status = [0x%x]\n",
						status) ;
	}

	/*
	*  Stop execution on DSP.
	*/
	tmpStatus = PROC_stop (processorId) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("PROC_stop () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	/*
	*  Free the buffer(s) allocated for channel to DSP
	*/
	tmpStatus = CHNL_freeBuffer (processorId,
								CHNL_ID_OUTPUT,
								SCALE_Buffers,
								1) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("CHNL_freeBuffer () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	/*
	*  Delete the input channel
	*/
	tmpStatus = CHNL_delete  (processorId, CHNL_ID_INPUT) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("CHNL_delete () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	/*
	*  Delete the output channel
	*/
	tmpStatus = CHNL_delete  (processorId, CHNL_ID_OUTPUT) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("CHNL_delete () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	/*
	*  Close the pool
	*/
	tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("POOL_close () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	/*
	*  Detach from the processor
	*/
	tmpStatus = PROC_detach (processorId) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("PROC_detach () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	/*
	*  Destroy the PROC object.
	*/
	tmpStatus = PROC_destroy () ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		printf ("PROC_destroy () failed. Status = [0x%x]\n",
						tmpStatus) ;
	}

	printf ("Leaving SCALE_Delete ()\n") ;
}


/** ============================================================================
*  @func   SCALE_Main
*
*  @desc   Entry point for the application
*
*  @modif  None
*  ============================================================================
*/
NORMAL_API
Void
SCALE_Main (IN Char8 * dspExecutable,
			IN Char8 * strBufferSize,
			IN Char8 * strNumIterations,
			IN Char8 * strProcessorId,
			IN Char8 * strScaleFactor)
{
	DSP_STATUS status = DSP_SOK ;
	Uint8 processorId = 0 ;

	printf ("=============== Sample Application : SCALE ==========\n") ;

	if (   (dspExecutable != NULL)
		&& (strBufferSize != NULL)
		&& (strScaleFactor != NULL)
		&& (strNumIterations != NULL)) {

		/*
		*  Validate the buffer size and number of iterations specified.
		*/
		SCALE_BufferSize = DSPLINK_ALIGN (SCALE_Atoi (strBufferSize),
										DSPLINK_BUF_ALIGN) ;

		SCALE_NumIterations = SCALE_Atoi (strNumIterations) ;
		processorId         = SCALE_Atoi (strProcessorId) ;
		SCALE_XferValue		= atof       (strScaleFactor);

		if (   (SCALE_NumIterations >  0xFFFF)
			|| (SCALE_BufferSize == 0)
			|| (processorId >= MAX_DSPS)) {
			status = DSP_EINVALIDARG ;
			printf ("ERROR! Invalid arguments specified for  "
						"scale application.\n"
						"     Max iterations = %d\n",
						0xFFFF) ;
			printf ("     Buffer size    = %d\n",
						SCALE_BufferSize) ;
			printf ("     DSP processor Id    = %d\n",
						processorId) ;
		}
		else {
			/*
			*  Specify the dsp executable file name and the buffer size for
			*  loop creation phase.
			*/
			if (DSP_SUCCEEDED (status)) {
				status = SCALE_Create (dspExecutable,
									strBufferSize,
									strNumIterations,
									processorId) ;
			}

			/*
			*  Execute the data transfer loop.
			*/
			if (DSP_SUCCEEDED (status)) {
				status = SCALE_Execute (SCALE_NumIterations, processorId) ;
			}

			/*
			*  Perform cleanup operation.
			*/
			SCALE_Delete (processorId) ;
		}
	}
	else {
		status = DSP_EINVALIDARG ;
		printf ("ERROR! Invalid arguments specified for "
					"scale application\n") ;
	}

	printf ("====================================================\n") ;
}

/** ============================================================================
*  @func   sfl_dsp_init
*
*  @desc   This function allocates and initializes resources used by
*          this application.
*
*  @modif  None
*  ============================================================================
*/
void
sfl_dsp_init (IN char * dspExecutable,
			IN char * strBufferSize,
			IN char * strNumIterations,
			IN char * strProcessorId,
			IN char * strScaleFactor)
{
	DSP_STATUS status = DSP_SOK ;
	Uint8 processorId = 0 ;
	MSGQ_LocateAttrs syncLocateAttrs        ;
	SCALE_ScaleMsg * msg                    ;

	/*
	*  Validate the buffer size and number of iterations specified.
	*/
	SCALE_BufferSize = DSPLINK_ALIGN (SCALE_Atoi (strBufferSize),
					DSPLINK_BUF_ALIGN) ;

	SCALE_NumIterations = SCALE_Atoi (strNumIterations) ;
	processorId         = SCALE_Atoi (strProcessorId) ;
	SCALE_XferValue		= atof       (strScaleFactor);

	/*
	*  Specify the dsp executable file name and the buffer size for
	*  loop creation phase.
	*/
	if (DSP_SUCCEEDED (status)) {
		status = SCALE_Create (dspExecutable,
						strBufferSize,
						strNumIterations,
						processorId) ;
		printf("scale_create OK :D\n");
	}

	if (DSP_SUCCEEDED (status)) {
		status = DSP_ENOTFOUND ;
		/*
		*  Locate the DSP's message queue
		*/
		syncLocateAttrs.timeout = WAIT_FOREVER;
		while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
			status = MSGQ_locate (SampleDspMsgqName,
								&SampleDspMsgq,
								&syncLocateAttrs) ;
			if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
				usleep (100000) ;
			}
			else if (DSP_FAILED (status)) {
				printf ("MSGQ_locate () failed. Status = [0x%x]\n", status) ;
			}
		}


		/*
		*  Send the scaling factor
		*/
		status = MSGQ_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
								APP_MSG_SIZE,
								(MSGQ_Msg *) &msg) ;
		if (DSP_SUCCEEDED (status)) {
			msg->scalingFactor = SCALE_XferValue ;
			/* Send the allocated message */
			MSGQ_setMsgId ((MSGQ_Msg) msg, SCALE_REQUEST_MSGID) ;
			status = MSGQ_put (SampleDspMsgq, (MSGQ_Msg) msg) ;
			if (DSP_SUCCEEDED (status)) {
				printf ("Changed the scale factor to: %f\n",
								SCALE_XferValue) ;
			}
			else {
				printf ("MSGQ_put () failed. Status = [0x%x]\n",
								status) ;
			}
		}
		else {
			printf ("MSGQ_alloc () failed. Status = [0x%x]\n",
							status) ;
		}
	}
}

/** ============================================================================
*  @func   sfl_dsp_deinit
*
*  @desc   This function releases resources allocated earlier by call to
*          sfl_dsp_init ().
*
*  @modif  None
*  ============================================================================
*/
void
sfl_dsp_deinit(Uint8 processorId) {

	DSP_STATUS       status = DSP_ENOTFOUND ;
	/*
	*  Release the remote message queue
	*/
	status = MSGQ_release (SampleDspMsgq) ;
	if (DSP_FAILED (status)) {
		printf ("MSGQ_release () failed. Status = [0x%x]\n", status) ;
	}

	//  Perform cleanup operation.
	SCALE_Delete (processorId) ;
}



/** ============================================================================
*  @func   sfl_dsp_process
*
*  @desc   Process buffer.
*
*  @modif  None
*  ============================================================================
*/
void
sfl_dsp_process(short *buffer_in, short *buffer_out, unsigned int nbelem) {

	Uint8 processorId = 0 ;
	DSP_STATUS       status = DSP_ENOTFOUND ;
	//unsigned int j;
	//short * ptr16 = NULL;


	/*
	*  Fill the IO Request structure
	*  It gives Information for adding or reclaiming an input request.
	*/

	SCALE_IOReq.buffer = SCALE_Buffers [0] ;
	SCALE_IOReq.size   = DSPLINK_ALIGN (nbelem*sizeof(short), DSPLINK_BUF_ALIGN) ; // because DSP size are in byte

	//printf( "SCALE_BufferSize: %d ",SCALE_BufferSize);
	//printf( "SCALE_IOReq.size: %d ",SCALE_IOReq.size);

	//printf( "buffer @ 0x%04x\n",SCALE_Buffers [0]);


	memcpy(SCALE_IOReq.buffer, buffer_in, SCALE_IOReq.size);
	//printf( "end memcpy\n");
/*
	ptr16 = (short *) (SCALE_IOReq.buffer) ;
	for (   j = 0 ; j < taille ;j++ ) {
		*ptr16=buffer_in[j];

		ptr16++;
	}
*/

	//Send data to DSP.  Issue 'filled' buffer to the channel.
	status = CHNL_issue (processorId, CHNL_ID_OUTPUT, &SCALE_IOReq) ;


	//Reclaim 'empty' buffer from the channel

	if (DSP_SUCCEEDED (status)) {
		status = CHNL_reclaim (processorId,
							CHNL_ID_OUTPUT,
							WAIT_FOREVER,
							&SCALE_IOReq) ;
		if (DSP_FAILED (status)) {
			printf ("CHNL_reclaim () failed. Status = [0x%x]\n",
							status) ;
		}
	}
	else {
		printf ("CHNL_issue1 () failed. Status = [0x%x]\n", status) ;
	}

	// Receive data from DSP Issue 'empty' buffer to the channel.
	if (DSP_SUCCEEDED (status)) {
		status = CHNL_issue (processorId, CHNL_ID_INPUT, &SCALE_IOReq) ;
		if (DSP_FAILED (status)) {
			printf ("CHNL_issue2 () failed. Status = [0x%x]\n",
							status) ;
		}
	}

	//Reclaim 'filled' buffer from the channel

	if (DSP_SUCCEEDED (status)) {
		status = CHNL_reclaim (processorId,
							CHNL_ID_INPUT,
							WAIT_FOREVER,
							&SCALE_IOReq) ;
		if (DSP_FAILED (status)) {
			printf ("CHNL_reclaim () failed. Status = [0x%x]\n",
							status) ;
		}
	}

	memcpy( buffer_out, SCALE_IOReq.buffer,SCALE_IOReq.size);
/*
	ptr16 = (short *) (SCALE_IOReq.buffer) ;
	for (   j = 0 ;  j < taille ;j++ ) {
		buffer_out[j]=*ptr16;
		ptr16++;
	}*/
}




#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
*  @func   SCALE_VerifyData
*
*  @desc   This function verifies the data-integrity of given buffer.
*
*  @modif  None
*  ----------------------------------------------------------------------------
*/
STATIC
NORMAL_API
DSP_STATUS
SCALE_VerifyData (IN Char8 * buf)
{
	DSP_STATUS status = DSP_SOK ;
	Uint16 *   ptr16  = NULL    ;
	double *   ptr32  = NULL    ;
	Int16      i                ;

	/*
	*  Verify the data
	*/
	
	
	printf ("---> Verify the data\n") ;
	
	
	ptr16 = (Uint16 *) (buf) ;
	ptr32 = (double *) (buf) ;
	for (i = 0 ;
		DSP_SUCCEEDED (status) && (i < SCALE_BufferSize / sizeof(double)) ;
		i++) {
			/*if (DSP_MAUSIZE == 1) {
			//printf ("DSP_MAUSIZE == 1\n") ;
			printf ("    Expected [0x%x]",
							(i * 42)) ;
			printf ("    Received [0x%x]\n", *buf) ;
		
			if (*buf != (Char8) (XFER_CHAR * SCALE_XferValue)) {
				printf ("ERROR! Data integrity check failed\n") ;
				printf ("    Expected [0x%x]\n",
							(XFER_CHAR * SCALE_XferValue)) ;
				printf ("    Received [0x%x]\n", *buf) ;
				status = DSP_EFAIL ;
			}
			buf++ ;
		}
		else if (DSP_MAUSIZE == 2) {
			//printf ("DSP_MAUSIZE == 2\n") ;
			printf ("    Expected [%d]",
							(i * 42)) ;   
			printf ("    Received [%d]\n", *ptr16) ;
	
	
			if (*ptr16 != (Uint16) (XFER_CHAR * SCALE_XferValue)) {
				printf ("ERROR! Data integrity check failed\n") ;
				printf ("    Expected [0x%x]\n",
							(XFER_CHAR * SCALE_XferValue)) ;
				printf ("    Received [0x%x]\n", *buf) ;
				status = DSP_EFAIL ;
			}
			ptr16++ ;
		}*/
		
		//printf ("    Received [%f]\n", *ptr32) ;
		ptr32++;
		
	}

	return status ;

}
#endif /* if defined (VERIFY_DATA) */






#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

