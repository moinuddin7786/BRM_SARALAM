select * from billinfo_t where scenario_obj_id0 <> 0;

and account_obj_id0=613345;
select * from service_t where login='moin786';
select account_obj_id0 from service_t where login='moin786'; --844118
--creating onother acct
select account_obj_id0, service_t.* from service_t where login='yaser786'; --850757

select * from account_t where account_no = 'TestPS5'; --or-- 
select * from account_t where account_no like 'TestPS5'; --613345
elect * from billinfo_t where scenario_obj_id0 <> 0 and account_obj_id0=613345;

--/service/admin_client
--/service/pcm_clint

select * from plan_t;