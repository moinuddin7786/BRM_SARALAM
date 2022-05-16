/*******************************************************************
 *
 *      Copyright (c) 1996 - 2006 Oracle. All rights reserved.
 *
 *      This material is the confidential property of Oracle Corporation
 *      or its subsidiaries or licensors and may be used, reproduced, stored
 *      or transmitted only in accordance with a valid Oracle license or
 *      sublicense agreement.
 *
 *******************************************************************/

#ifndef lint
static  char    Sccs_id[] = "@(#)%Portal Version: fm_air_custom_search_acc_services.c:ServerIDCVelocityInt:3:2006-Sep-06 16:40:39 %";
#endif

/*******************************************************************
 * Contains the PCM_OP_RATE_POL_TAX_LOC operation. 
 *
 * Used to provide the relevant address information for use
 * in taxing a particular event. The addresses provided are
 * used to determine jusridiction of the tax calculated.
 *
 *******************************************************************/

#include <stdio.h> 
#include <string.h> 
 
#include "pcm.h"
#include "ops/rate.h"
#include "cm_fm.h"
#include "pin_errs.h"
#include "pinlog.h"
#include "my_custom_fields.h"

#define FILE_SOURCE_ID  "fm_air_custom_search_acc_services.c"


/*******************************************************************
 * Routines contained within.
 *******************************************************************/
EXPORT_OP void
op_air_custom_search_acc_services(
        cm_nap_connection_t	*connp,
	u_int32			opcode,
        u_int32			flags,
        pin_flist_t		*i_flistp,
        pin_flist_t		**o_flistpp,
        pin_errbuf_t		*ebufp);

static void
fm_air_custom_search_acc_services(
	pcm_context_t		*ctxp,
	pin_flist_t		*i_flistp,
	pin_flist_t		**o_flistpp,
	pin_errbuf_t		*ebufp);

static void
fm_rax_utils_get_acct_billinfo(
        pcm_context_t           *ctxp,
        poid_t                  *acct_pdp,
        pin_flist_t             **o_flistpp,
        pin_errbuf_t            *ebufp );


/*******************************************************************
 * Main routine for the AIR_OP_SEARCH_ACC_SERVICES operation.
 *******************************************************************/
void
op_air_custom_search_acc_services(
        cm_nap_connection_t	*connp,
	u_int32			opcode,
        u_int32			flags,
        pin_flist_t		*i_flistp,
        pin_flist_t		**o_flistpp,
        pin_errbuf_t		*ebufp)
{
	pcm_context_t		*ctxp = connp->dm_ctx;

	/*check errors in the error buffer before proceeding*/
	if (PIN_ERR_IS_ERR(ebufp))
	{
		return;
		PIN_ERR_CLEAR_ERR(ebufp);
	}

	/***********************************************************
	 * Insanity check.
	 ***********************************************************/
	if (opcode != AIR_OP_SEARCH_ACC_SERVICES) {
		pin_set_err(ebufp, PIN_ERRLOC_FM,
			PIN_ERRCLASS_SYSTEM_DETERMINATE,
			PIN_ERR_BAD_OPCODE, 0, 0, opcode);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"op_air_custom_search_acc_services opcode error", ebufp);
		return;
		/*****/
	}

	/***********************************************************
	 * Debug: What we got.
	 ***********************************************************/
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"op_air_custom_search_acc_services input flist", i_flistp);

	/***********************************************************
	 * Do the actual op in a sub.
	 ***********************************************************/
	fm_air_custom_search_acc_services(ctxp, i_flistp, o_flistpp, ebufp);

	/***********************************************************
	 * Error?
	 ***********************************************************/
	if (PIN_ERR_IS_ERR(ebufp)) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"op_air_custom_search_acc_services error in the Main Function itself", ebufp);
	} else {
		/***************************************************
		 * Debug: What we're sending back.
		 ***************************************************/
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
			"op_air_custom_search_acc_services return flist", *o_flistpp);
	}

	return;
}

/*******************************************************************
 * fm_air_custom_search_acc_services():
 *
 *    This policy could do several things to prepare the input
 *    flist for the tax calculation.  It could locate the locale's
 *    involved. 
 *
 *    The default implementation is just to return the POID only. 
 *
 *******************************************************************/
static void
fm_air_custom_search_acc_services(
	pcm_context_t		*ctxp,
	pin_flist_t		*i_flistp,
	pin_flist_t		**o_flistpp,
	pin_errbuf_t		*ebufp)
{

	pin_flist_t	*cust_rflistp = NULL;
	pin_flist_t	*r_flistp = NULL;
	int		elem_id = 0;
	pin_cookie_t	cookie = NULL;
	poid_t		*a_pdp = NULL;

	/*check error before proceeding*/
	if (PIN_ERR_IS_ERR(ebufp)) 
	{
		return;
	}
	/*clear errorbuf if no error*/
	PIN_ERR_CLEAR_ERR(ebufp);

	/* STEP 1 : Find the poid for the account Passed  START */ 
	PCM_OP(ctxp, PCM_OP_CUST_FIND, 0, i_flistp, &cust_rflistp, ebufp);

	if (PIN_ERR_IS_ERR(ebufp)) {
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "fm_air_custom_search_acc_services error in calling CUST_FIND", ebufp);
        }

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                        "fm_air_custom_search_acc_services return flist", cust_rflistp);

	elem_id = 0;
	cookie = NULL;
	while((r_flistp = PIN_FLIST_ELEM_GET_NEXT(cust_rflistp,
			PIN_FLD_RESULTS, &elem_id, 1, &cookie, ebufp)) != NULL)
	{
		
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
			"fm_air_custom_search_acc_services Each Result from Output", r_flistp);
		/*Break the loop in the first iteration itself as Results contain only one Array*/
		break;

	}
	/*STEP 1 END */

	/*STEP 2 : Get /billinfo for the account in the STEP 1 : STEP 2 START */
	/*Call Function to get the Account Billinfo*/

	a_pdp = PIN_FLIST_FLD_GET(r_flistp, PIN_FLD_POID, 0, ebufp);
	fm_rax_utils_get_acct_billinfo(ctxp, a_pdp, o_flistpp, ebufp); 

	/*STEP 2 END */	
	return;
}

static void
fm_rax_utils_get_acct_billinfo(
	pcm_context_t		*ctxp,
	poid_t			*acct_pdp,
	pin_flist_t		**o_flistpp,
	pin_errbuf_t		*ebufp )
{
	pin_flist_t		*srch_in_flistp = NULL;
	pin_flist_t		*srch_out_flistp = NULL;
	pin_flist_t		*args_flistp = NULL;
	poid_t			*s_pdp = NULL;
	int32			sflags = SRCH_EXACT | SRCH_DISTINCT;
	char			*template = "select X from /billinfo where F1 = V1 ";

	
	if( PIN_ERRBUF_IS_ERR( ebufp ) ) 
		return;
	PIN_ERRBUF_CLEAR( ebufp );

	*o_flistpp = NULL;

	/*******************************************************************
	 * Create the Search flist.
	 *******************************************************************/
	srch_in_flistp = PIN_FLIST_CREATE(ebufp);

	s_pdp = PIN_POID_CREATE(PIN_POID_GET_DB(acct_pdp), "/search", -1, ebufp);
	PIN_FLIST_FLD_PUT(srch_in_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
	

	PIN_FLIST_FLD_SET(srch_in_flistp, PIN_FLD_TEMPLATE, (void *)template, ebufp);
	PIN_FLIST_FLD_SET(srch_in_flistp, PIN_FLD_FLAGS, (void *)&sflags, ebufp);
	
	/*******************************************************************
	 * ARGS[1]: Matching account object.
	 *******************************************************************/
	args_flistp = PIN_FLIST_ELEM_ADD(srch_in_flistp, PIN_FLD_ARGS, 1, ebufp);
	PIN_FLIST_FLD_SET(args_flistp, PIN_FLD_ACCOUNT_OBJ, (void *)acct_pdp, ebufp);

	PIN_FLIST_ELEM_SET(srch_in_flistp, (pin_flist_t *)NULL, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);

	/*******************************************************************
	 * Do the Search.
	 *******************************************************************/
	PCM_OP(ctxp, PCM_OP_SEARCH, 0, srch_in_flistp, &srch_out_flistp, ebufp);


	*o_flistpp = PIN_FLIST_ELEM_TAKE(srch_out_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, 0, ebufp);

	/*******************************************************************
	 * Cleanup and Return.
	 *******************************************************************/
	PIN_FLIST_DESTROY_EX(&srch_in_flistp, NULL);
	PIN_FLIST_DESTROY_EX(&srch_out_flistp, NULL);

	/*******************************************************************
	 * Errors?
	 *******************************************************************/
	if (PIN_ERR_IS_ERR(ebufp)){
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, "fm_rax_utils_get_acct_billinfo Error: ", ebufp);
	}

	return;
}
