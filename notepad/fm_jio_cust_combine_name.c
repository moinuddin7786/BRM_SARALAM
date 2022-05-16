#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "pcm.h"
#include "cm_fm.h"
#include "pin_errs.h"
#include "pinlog.h"
#include "fm_bill_utils.h"
#include "pin_pymt.h"
#include "pin_bill.h"
#include "my_custom_fields.h"

/*******************************************************************
* Routines contained herein.
*******************************************************************/
EXPORT_OP void
op_jio_cust_combine_name(
        cm_nap_connection_t     *connp,
        u_int                   opcode,
        u_int                   flags,
        pin_flist_t             *in_flistp,
        pin_flist_t             **ret_flistpp,
        pin_errbuf_t    	*ebufp);
				
static void
fm_jio_cust_combine_name(
        pcm_context_t   *ctxp,
        pin_flist_t     *in_flistp,
        pin_flist_t     **ret_flistpp,
        pin_errbuf_t    *ebufp);

static void
fm_jio_combine_first_last_name(
        pcm_context_t   *ctxp,
        pin_flist_t     *in_flistp,
        pin_flist_t     **ret_flistpp,
        pin_errbuf_t    *ebufp);
		
/***********************************************************
* Implementation - op_jio_cust_combine_name.
***********************************************************/

void
op_jio_cust_combine_name (
        cm_nap_connection_t     *connp,
        u_int32                   opcode,
        u_int32                 flags,
        pin_flist_t             *in_flistp,
        pin_flist_t             **ret_flistpp,
        pin_errbuf_t            *ebufp)
{
        pcm_context_t           *ctxp = connp->dm_ctx;
        pin_flist_t             *r_flistp = NULL;
	poid_t		        *acct_pdp = NULL;
        if( PIN_ERR_IS_ERR(ebufp)){
                return;
        }
		PIN_ERR_CLEAR_ERR(ebufp);


	 /***********************************************************
         * Insanity check.
         ***********************************************************/
        if (opcode != JIO_OP_CUST_COMBINE_NAME) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "op_air_custom_search_acc_services opcode error", ebufp);
                return;
                /*****/
        }

	/***********************************************************
        * Debug what we got.
        ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "op_jio_cust_combine_name input flist", in_flistp);

	acct_pdp = PIN_FLIST_FLD_GET(in_flistp, PIN_FLD_POID, 0, ebufp);
											
		
        /***********************************************************
         * Call main function to do it
         ***********************************************************/
       
	 fm_jio_cust_combine_name(ctxp, in_flistp, ret_flistpp, ebufp);

        /***********************************************************
         * Results.
         ***********************************************************/
       
	
	PIN_FLIST_FLD_SET(*ret_flistpp, PIN_FLD_POID, (void *)acct_pdp, ebufp);
	{
                PIN_ERR_CLEAR_ERR(ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                    "op_jio_cust_combine_name return flist", *ret_flistpp);
        }

        return;
}

/***********************************************************
* Implementation - fm_jio_cust_combine_name.
***********************************************************/

static void
fm_jio_cust_combine_name(
        pcm_context_t   *ctxp,
        pin_flist_t     *in_flistp,
        pin_flist_t     **ret_flistpp,
        pin_errbuf_t    *ebufp)
{
	char		*str_val;
	char 		full_name[25] = {0};
        pin_flist_t     *cmb_flistp = NULL;
									
	if( PIN_ERR_IS_ERR(ebufp))
	{
                return;
	}
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
          	"fm_jio_cust_combine_name input flist", in_flistp);
									
	fm_jio_combine_first_last_name(ctxp, in_flistp, &cmb_flistp, ebufp);
									
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
          	"fm_jio_cust_combine_name names got is", cmb_flistp);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Point-1");					
	str_val = (char *) PIN_FLIST_FLD_GET(cmb_flistp, PIN_FLD_FIRST_NAME, 0, ebufp);
		
	if(str_val)
	{
		strcpy(full_name, str_val);
	}
		
	str_val = (char *) PIN_FLIST_FLD_GET(cmb_flistp, PIN_FLD_LAST_NAME, 0, ebufp);
					
	if(str_val)
	{
		strcat(full_name, " ");
		strcat(full_name, str_val);
	}
		
	PIN_FLIST_FLD_SET(cmb_flistp, JIO_FLD_COMBINED_NAME, (void *) full_name, ebufp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
          	"fm_jio_cust_combine_name after final logic", cmb_flistp);
		
	*ret_flistpp = cmb_flistp;		

        PIN_ERR_CLEAR_ERR(ebufp);
		
	if (PIN_ERR_IS_ERR(ebufp)) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"fm_my_first_custom_search: Error while preparing input Flist", ebufp);
	}

        return;
}

static void
fm_jio_combine_first_last_name(
	pcm_context_t	*ctxp,
	pin_flist_t	*in_flistp,
	pin_flist_t	**o_flistpp,
	pin_errbuf_t	*ebufp)
{
	pin_flist_t	*temp_flistp = NULL;
	pin_flist_t	*temp_flistp2= NULL;
	int64		db = 1 ;
	int32 		id = -1;	 			 
	poid_t		*s_pdp = NULL;
	poid_t		*acct_pdp = NULL;
	pin_flist_t	*args_flistp = NULL;
	char		*template = "select X from /account where F1 = V1 ";	
	pin_flist_t	*s_flistp = NULL;				
	pin_flist_t	*r_flistp = NULL;
	pin_flist_t	*result_flistp = NULL;
	pin_flist_t	*nameinfo_flistp = NULL;
	pin_flist_t	*res_nameinfop = NULL;
	int32		flags = 256;
																	
	/*****create inpit flist for PCM_OP_SEARCH***********/
	s_flistp = PIN_FLIST_CREATE(ebufp);
	
	acct_pdp = PIN_FLIST_FLD_GET(in_flistp, PIN_FLD_POID, 0, ebufp);
	/***********CREATE SEARCH POID*************/
	s_pdp = PIN_POID_CREATE(db, "/search", -1, ebufp);
	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void*)s_pdp, ebufp);
																
	/**********SET THE TEMPLATE**********/
	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_TEMPLATE, (void *)template, ebufp);
	
	/**********PIN_FLD_FLAGS*************/
	PIN_FLIST_FLD_SET(s_flistp, PIN_FLD_FLAGS, (void *)&flags, ebufp);
			
	/* set ARGS[1] -- FIRST NAME*/
	args_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_ARGS, 1, ebufp);
	PIN_FLIST_FLD_SET(args_flistp, PIN_FLD_POID, (void *)acct_pdp, ebufp);
	

	/*******add results*********/
	result_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_RESULTS, 0, ebufp);
	res_nameinfop = PIN_FLIST_ELEM_ADD(result_flistp, PIN_FLD_NAMEINFO, PIN_ELEMID_ANY, ebufp);
	PIN_FLIST_FLD_SET(res_nameinfop, PIN_FLD_FIRST_NAME, (void *)NULL, ebufp);					
	PIN_FLIST_FLD_SET(res_nameinfop, PIN_FLD_MIDDLE_NAME, (void *)NULL, ebufp);
	PIN_FLIST_FLD_SET(res_nameinfop, PIN_FLD_LAST_NAME, (void *)NULL, ebufp);
																																
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR,
		"fm_jio_cust_get_first_last_name: search input flist", s_flistp);
	/* calling the search */
	if (PIN_ERR_IS_ERR(ebufp)) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"fm_jio_cust_get_first_last_name: Error while preparing input Flist", ebufp);
	}
	
	PCM_OP(ctxp, PCM_OP_SEARCH, 0, s_flistp, &r_flistp, ebufp);
	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR,
		"fm_jio_cust_get_first_last_name: Result flist from search", r_flistp);
	
	temp_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS, 0, 0, ebufp);
        
         
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR,
		"fm_jio_cust_get_first_last_name: Result in temp_flistp",temp_flistp);
	temp_flistp2 = PIN_FLIST_ELEM_GET(temp_flistp, PIN_FLD_NAMEINFO, 1, 0, ebufp); 	
	
	*o_flistpp = temp_flistp2;
		
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR,
		"fm_jio_cust_get_first_last_name: Result fm sechtemplate", *o_flistpp);
	
	/* destroying flist */
	PIN_FLIST_DESTROY_EX(&s_flistp, ebufp);
		return;
}
