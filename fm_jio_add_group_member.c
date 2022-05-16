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
op_jio_add_group_member(
	cm_nap_connection_t	*connp,
	u_int			opcode,
	u_int			flags,
	pin_flist_t		*in_flistp,
	pin_flist_t		**ret_flistpp,
	pin_errbuf_t	*ebufp);
	
static void
fm_jio_add_group_member(
	pcm_context_t	*ctxp,
	pin_flist_t	*i_flistp,
	pin_flist_t	**out_flistpp,
	pin_errbuf_t	*ebufp);
	
/***********************************************************
* Implementation - op_jio_add_group_member.
***********************************************************/
	
void
op_jio_add_group_member (
	cm_nap_connection_t	*connp,
	u_int			opcode,
	u_int			flags,
	pin_flist_t		*in_flistp,
	pin_flist_t		**ret_flistpp,
	pin_errbuf_t		*ebufp)
{
	pcm_context_t		*ctxp = connp->dm_ctx;
	pin_flist_t			*r_flistp = NULL;

	if( PIN_ERR_IS_ERR(ebufp)){
		return;
	}
	PIN_ERR_CLEAR_ERR(ebufp);

	/***********************************************************
	* Null out results until we have some.
	***********************************************************/
	*ret_flistpp = NULL;

	/***********************************************************
	* Error out in case opcode is not RAX_PYMT_CUSTOMER_SEARCH
	***********************************************************/
	if (opcode != JIO_OP_ADD_GROUP_MEMBER) {
		pin_set_err(ebufp, PIN_ERRLOC_FM,
			PIN_ERRCLASS_SYSTEM_DETERMINATE,
			PIN_ERR_BAD_OPCODE, 0, 0, opcode);
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"bad opcode in op_rax_item_refund", ebufp);
		return;
	}

	/***********************************************************
	* Debug what we got.
	***********************************************************/
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"op_jio_add_group_member input flist", in_flistp);


	/***********************************************************
	 * Call main function to do it
	 ***********************************************************/
	fm_jio_add_group_member(ctxp, in_flistp, &r_flistp, ebufp);

	/***********************************************************
	 * Results.
	 ***********************************************************/
	if (PIN_ERR_IS_ERR(ebufp)) {
		*ret_flistpp = (pin_flist_t *)NULL;
		PIN_FLIST_DESTROY(r_flistp, NULL);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
				"op_jio_add_group_member error", ebufp);
		PIN_ERR_LOG_FLIST( PIN_ERR_LEVEL_ERROR,
				"op_jio_add_group_member input flist", in_flistp );
	} else {
		*ret_flistpp = r_flistp;
		PIN_ERR_CLEAR_ERR(ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
				"op_jio_add_group_member return flist", r_flistp);
	}

	return;
}

/***********************************************************
* Implementation - fm_jio_add_group_member.
***********************************************************/

static void
fm_jio_add_group_member(
	pcm_context_t		*ctxp,
	pin_flist_t			*i_flistp,
	pin_flist_t			**out_flistpp,
	pin_errbuf_t		*ebufp)
{
	pin_flist_t        *search_flistp  = NULL;
	pin_flist_t        *search_rflistp = NULL;
	pin_flist_t        *args_flist     = NULL;
	pin_flist_t        *out_flistp     = NULL;
	pin_flist_t        *results_flist  = NULL;
	char               *acct_no_parent = NULL;
	char               *acct_no_child = NULL;
	poid_t             *search_pdp;
	poid_t             *acc_pdp_parent = NULL;
	poid_t             *acc_pdp_child = NULL;
	int32              elemid = 0;
	pin_cookie_t       cookie = NULL;
	int64				database = 0 ;
	char				*template = "select X from /group where F1 = V1 ";	
	int32				flags = 256;

	pin_flist_t		*add_memflistp = NULL;
	pin_flist_t		*add_rmemflistp = NULL;
	pin_flist_t		*mem_flistp = NULL;
	pin_flist_t		*temp_flistp = NULL;
	poid_t     	        *group_pdp;	
	
	pin_flist_t *create_grpflistp = NULL;
	pin_flist_t *create_rgrpflistp = NULL;
	pin_flist_t *grp_flistp = NULL;
	pin_flist_t *addmem_flistp = NULL;
	pin_flist_t *addmem_rflistp = NULL;
	poid_t *creategroup_pdp;
    
	database = cm_fm_get_current_db_no(ctxp);
    
	if (PIN_ERR_IS_ERR(ebufp))
                    return ;
	PIN_ERR_CLEAR_ERR(ebufp);
    
	acct_no_parent = (char *)PIN_FLIST_FLD_GET(i_flistp, JIO_FLD_PARENT_ACCOUNT, 0, ebufp);
	acct_no_child = (char *)PIN_FLIST_FLD_GET(i_flistp, JIO_FLD_CHILD_ACCOUNT, 0, ebufp);

	fm_jio_add_group_member_get_acct_pdp(ctxp, acct_no_parent, &acc_pdp_parent, ebufp);
	fm_jio_add_group_member_get_acct_pdp(ctxp, acct_no_child, &acc_pdp_child, ebufp);
    
	search_flistp = PIN_FLIST_CREATE(ebufp);
	search_pdp = PIN_POID_CREATE(database, "/search", -1, ebufp);
	PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)search_pdp, ebufp);
    
	/******************* SET TEMPLATE ********************/ 
	PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, template, ebufp);
	
	/******************* SET THE FLAGS *******************/
	PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&flags, ebufp);
    
	args_flist = PIN_FLIST_ELEM_ADD(search_flistp, PIN_FLD_ARGS, 1, ebufp);
	PIN_FLIST_FLD_SET(args_flist, PIN_FLD_PARENT, (void *)acc_pdp_parent, ebufp);
    
	results_flist = PIN_FLIST_ELEM_ADD(search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
    
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                    "(fm_jio_add_group_member):search acct_pdp result flist", search_flistp);
	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"fm_jio_add_group_member Search Input Flist Prepared Is", search_flistp);
	
	/******************** CALLING SEARCH OPCODE ********************/
	PCM_OP(ctxp, PCM_OP_SEARCH, 0, search_flistp, &search_rflistp, ebufp);
	
	if (PIN_ERR_IS_ERR(ebufp)) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			 "fm_jio_add_group_member: Error while calling Opcode  PCM_OP_SEARCH", ebufp);
		goto cleanup;
	}
	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"fm_jio_add_group_member Search Output Flist Got Is", search_rflistp);
	
	temp_flistp = PIN_FLIST_ELEM_GET(search_rflistp, PIN_FLD_RESULTS, 0, 1, ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"fm_jio_add_group_member: search results from search_rflistp", temp_flistp);
	
	if(temp_flistp) 
	{

		/**** CREATING ADD MEMBER FLIST ****/
		add_memflistp = PIN_FLIST_CREATE(ebufp);
	
		group_pdp = PIN_FLIST_FLD_GET(temp_flistp, PIN_FLD_POID, 0, ebufp);
	
		PIN_FLIST_FLD_PUT(add_memflistp, PIN_FLD_POID, (void *)group_pdp, ebufp);
	
		mem_flistp =  PIN_FLIST_ELEM_ADD(add_memflistp, PIN_FLD_MEMBERS, 1, ebufp);
	
		PIN_FLIST_FLD_PUT(mem_flistp, PIN_FLD_OBJECT, (void *)acc_pdp_child, ebufp);
	
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
        	        "fm_jio_add_group_member mem Flist Prepared Is", add_memflistp);

		/************* CALLING PCM_OP_BILL_GROUP_ADD_MEMBER OPCODE ********************/
		PCM_OP(ctxp, PCM_OP_BILL_GROUP_ADD_MEMBER, 0, add_memflistp, &add_rmemflistp, ebufp);
	
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
			"fm_jio_add_group_member Add member output Flist Prepared Is", add_rmemflistp);
	
		*out_flistpp = add_rmemflistp;	
	}
	else {
		
		/*** CREATING GROUP FLIST ***/
		create_grpflistp = PIN_FLIST_CREATE(ebufp);

		creategroup_pdp = PIN_FLIST_FLD_GET(create_grpflistp, PIN_FLD_POID, 0, ebufp);

		PIN_FLIST_FLD_PUT(create_grpflistp, PIN_FLD_POID, (void *)creategroup_pdp, ebufp);



		PIN_FLIST_FLD_PUT(grp_flistp, PIN_FLD_PARENT, (void *)acc_pdp_parent, ebufp);



	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"fm_jio_add_group_member grp input Flist Prepared Is", create_grpflistp);



	/*** CALLING PCM_OP_BILL_GROUP_CREATE OPCODE ***/
	PCM_OP(ctxp, PCM_OP_BILL_GROUP_CREATE, 0, create_grpflistp, &create_rgrpflistp, ebufp);

	PIN_FLIST_FLD_GET(create_rgrpflistp, PIN_FLD_POID, 0, ebufp);



	/**** CREATING ADD MEMBER FLIST ****/
	add_memflistp = PIN_FLIST_CREATE(ebufp);

	PIN_FLIST_FLD_PUT(add_memflistp, PIN_FLD_POID, (void *)creategroup_pdp, ebufp);	

	mem_flistp = PIN_FLIST_ELEM_ADD(add_memflistp, PIN_FLD_MEMBERS, 1, ebufp);

	PIN_FLIST_FLD_PUT(mem_flistp, PIN_FLD_OBJECT, (void *)acc_pdp_child, ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"fm_jio_add_group_member mem Flist Prepared Is", add_memflistp);



	/*** CALLING PCM_OP_BILL_GROUP_ADD_MEMBER OPCODE ***/
	PCM_OP(ctxp, PCM_OP_BILL_GROUP_ADD_MEMBER, 0, add_memflistp, &addmem_rflistp, ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"fm_jio_add_group_member Add member output Flist Prepared Is", addmem_rflistp);

	*out_flistpp = addmem_rflistp;
	}
	cleanup:
								
	/*** DESTROY FLIST ***/
	PIN_FLIST_DESTROY_EX(&search_flistp, ebufp);
	
	return;
}
