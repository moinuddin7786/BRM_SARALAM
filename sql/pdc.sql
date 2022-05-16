select * from dd_objects_fields_t where sm_item_name like '%profile%';

select name, poid_id0 from product_t where name like 'Usage T%';
select name from product_t where name like 'Rec%';
select * from product_t where name like 'Ins%';

select * from product_t where created_t > 1666089750;

select name from discount_t where name like 'Test%';

select name from deal_t where name like 'Test%';

select name from plan_t where name like 'Wire%';

select poid_type from service_t;