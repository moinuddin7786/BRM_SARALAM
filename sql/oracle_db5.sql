select product_obj_id0, p.name from purchased_product_t pp , product_t p
where pp.product_obj_id0 = p.poid_id0
and pp.account_obj_id0=799476;

select poid_id0, bill_no, current_total, due, total_due, adjusted, recvd from bill_t where bill_no = 'B1-1055';

select poid_id0, bill_no,
to_date('01-JAN-1970','dd-mon-yyyy')+(end_t/60/60/24) bill_date,
to_date('01-JAN-1970','dd-mon-yyyy')+(due_t/60/60/24) due_date,
current_total, due, total_due, adjusted, recvd from bill_t
where bill_no='B1-1055';

select distinct poid_DB, poid_ID0, poid_TYPE, poid_REV, scenario_obj_DB, scenario_obj_ID0, scenario_obj_TYPE,
scenario_obj_REV, account_obj_DB, account_obj_ID0, account_obj_TYPE, account_obj_REV
from billinfo_t where billinfo_t.poid_id0 > 1
and billinfo_t.exempt_from_collections != 1
and ( billinfo_t.pay_type in
( 10001, 10002, 10003, 10004, 10005, 10006 ))
and ( billinfo_t.scenario_obj_ID0 != 0
or billinfo_t.pending_recv > 0 or billinfo_t.exempt_from_collections = 4 )
and billinfo_t.exempt_from_collections != 2 order by billinfo_t.poid_id0;

select * from billinfo_t where scenario_obj_id0 <> 0;
select * from account_t where account_no='TestPS18'; --799476

select pending_recv from billinfo_t where account_obj_id0=799476;
select * from billinfo_t where account_obj_id0=799476;

select * from config_t where poid_type like '%coll%';

select discount_obj_id0, d.name from purchased_discount_t pp , discount_t d
where pp.discount_obj_id0 = d.poid_id0
and pp.account_obj_id0=799476;

select * from config_t where poid_type = '/config/collections/scenario';

select * from billinfo_t where account_obj_id0= 799476;