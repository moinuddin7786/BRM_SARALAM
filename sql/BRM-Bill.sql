select * from account_T WHERE account_no = 'TestPS7'; --717590
select * from bal_grp_t where account_obj_id0=717590; --719510
select rec_id2, valid_from, valid_to, current_bal from bal_grp_sub_bals_t where obj_id0=719510;

select sub.rec_id2, sub.valid_from, sub.valid_to, sub.current_bal
from
bal_grp_t bal, bal_grp_sub_bals_t sub
where bal.poid_id0 = sub.obj_id0
and bal.account_obj_id0=717590;

select item_total, poid_id0, poid_type , status, due, transfered, recvd from item_t where account_obj_id0=717590
and bill_obj_id0 <> 0 and item_total <> 0;


select eb.amount, eb.GL_ID, eb.Resource_id, eb.impact_category, eb.quantity , eb.product_obj_id0
from
event_t e, event_bal_impacts_t eb
where e.poid_id0 = eb.obj_id0
and e.poid_id0 = 335799647216280496
and e.account_obj_id0 = 717590;

select poid_id0, to_date('01-JAN-1970','dd-mon-yyyy')+(end_t /60/60/24) Bill_date,current_total, subords_total, due, total_due , adjusted, recvd , writeoff
from bill_t where account_obj_id0=716950 and bill_no is not null;

select invoice_obj_id0,to_date('01-JAN-1970','dd-mon-yyyy')+(end_t /60/60/24) Bill_date,
current_total, subords_total, due, total_due , adjusted, recvd , writeoff
from bill_t where account_obj_id0=719510 and bill_no is not null;



select * from invoice_t where poid_id0 = 719510;
select * from invoice_formats_buf_t where obj_id0 = 717590;
