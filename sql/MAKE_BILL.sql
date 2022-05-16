select * from account_t where account_no='TestPS7'; --693873

select * from billinfo_t where account_obj_id0=717590; 

select * from bill_t where bill_no = 'B1-1014'; --718998

select item_total, poid_type,bill_obj_id0,bill_obj_type , due , status , recvd
from item_t where account_obj_id0=717590 and bill_obj_id0=687851
and item_total <> 0;

select item_total, poid_type,bill_obj_id0,bill_obj_type , due , status , recvd
from item_t where account_obj_id0=693873 and bill_obj_id0=0
and item_total <> 0;

select * from account_t;

select poid_id0, to_date('01-JAN-1970','dd-mon-yyyy')+(end_t /60/60/24) Bill_date,current_total, subords_total, due, total_due , adjusted, recvd , writeoff
from bill_t where account_obj_id0=716950 and bill_no = 'B1-1014';



select * from bill_t where bill_no = 'B1-1014';