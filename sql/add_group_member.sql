select * from account_t where account_no='ADDMBR01';
select * from group_t where parent_id0 =855638;

select * from group_billing_members_t where obj_id0 = 868803;

select poid_id0, ar_billinfo_obj_id0, account_obj_id0
from billinfo_t where account_obj_id0 in(868803, 855638);

select * from account_t where account_no='TestPS5';

select * from account_t;

select * from invoice_t where poid_id0 = 535877;
select * from invoice_formats_buf_t where obj_id0 = 535877;