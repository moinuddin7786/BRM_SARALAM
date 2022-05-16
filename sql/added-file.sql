select * from account_t where account_no ='TestPS00'; --748509
select * from account_nameinfo_t where obj_id0=748509;

select * from bill_t where account_obj_id0=748509;

select item_total, poid_type, status, recvd, adjusted from item_t where account_obj_id0 = 748509 and bill_obj_id0 = 746557;

robj - $DB /account 748509

select * from account_nameinfo_t where obj_id0 = 748509;

select * from product_t where name like '%500%';

select * from account_nameinfo_t where obj_id0 = 838254; --838254