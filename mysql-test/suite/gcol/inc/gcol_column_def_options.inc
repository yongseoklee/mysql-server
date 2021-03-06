################################################################################
# inc/gcol_column_def_options.inc                                              #
#                                                                              #
# Purpose:                                                                     #
#  Testing different optional parameters specified when defining               #
#  a generated column.                                                         #
#                                                                              #
#                                                                              #
#------------------------------------------------------------------------------#
# Original Author: Andrey Zhakov                                               #
# Original Date: 2008-09-02                                                    #
# Change Author:                                                               #
# Change Date:                                                                 #
# Change:                                                                      #
################################################################################

--echo #
--echo # Section 1. Wrong column definition options
--echo #            - NULL
--echo #            - DEFAULT <value>
--echo #            - AUTO_INCREMENT

--echo # NOT NULL
create table t1 (a int, b int generated always as (a+1) virtual not null);
--error 1048
insert into t1(a) values(null);
insert into t1(a) values(1);
select * from t1;
drop table t1;
create table t1 (a int, b int generated always as (a+1) stored not null);
--error 1048
insert into t1(a) values(null);
insert into t1(a) values(1);
select * from t1;
drop table t1;
create table t1 (a int);
alter table t1 add column b int generated always as (a+1) virtual not null;
drop table t1;

--echo # NULL  
--error 1064
create table t1 (a int, b int generated always as (a+1) virtual null);
create table t1 (a int);
--error 1064
alter table t1 add column b int generated always as (a+1) virtual null;
drop table t1;

--echo # Added columns mixed with virtual GC and other columns  
create table t1 (a int);
alter table t1 add column (b int generated always as (a+1) virtual, c int);
alter table t1 add column (d int, e int generated always as (a+1) virtual);
alter table t1 add column (f int generated always as (a+1) virtual, g int as(5) stored);
alter table t1 add column (h int generated always as (a+1) virtual, i int as(5) virtual);
drop table t1;

--echo # DEFAULT
--error 1064
create table t1 (a int, b int generated always as (a+1) virtual default 0);
create table t1 (a int);
--error 1064
alter table t1 add column b int generated always as (a+1) virtual default 0;
drop table t1;

--echo # AUTO_INCREMENT
--error 1064
create table t1 (a int, b int generated always as (a+1) virtual AUTO_INCREMENT);
create table t1 (a int);
--error 1064
alter table t1 add column b int generated always as (a+1) virtual AUTO_INCREMENT;
drop table t1;

--echo # [PRIMARY] KEY
--error ER_KEY_BASED_ON_GENERATED_COLUMN
create table t1 (a int, b int generated always as (a+1) virtual key);
create table t1 (a int, b int generated always as (a+1) stored key);
insert into t1 (a) values (3),(1),(2);
select * from t1;
drop table t1;
--error ER_KEY_BASED_ON_GENERATED_COLUMN
create table t1 (a int, b int generated always as (a+1) virtual primary key);
create table t1 (a int, b int generated always as (a+1) stored primary key);
insert into t1 (a) values (3),(1),(2);
select * from t1;
drop table t1;
create table t1 (a int);
--error ER_KEY_BASED_ON_GENERATED_COLUMN
alter table t1 add column b int generated always as (a+1) virtual key;
alter table t1 add column b int generated always as (a+1) stored key;
--error ER_KEY_BASED_ON_GENERATED_COLUMN
alter table t1 add column c int generated always as (a+2) virtual primary key;
show create table t1;
--error 1068
alter table t1 add column c int generated always as (a+2) stored primary key;
drop table t1;

--echo # Section 2. Other column definition options
--echo #            - COMMENT
--echo #            - REFERENCES (only syntax testing here)
--echo #            - STORED (only systax testing here)
create table t1 (a int, b int generated always as (a % 2) virtual comment 'my comment');
show create table t1;
describe t1;
drop table t1;
create table t1 (a int, b int generated always as (a % 2) virtual);
alter table t1 modify b int generated always as (a % 2) virtual comment 'my comment';
show create table t1;
describe t1;
insert into t1 (a) values (1);
select * from t1;
insert into t1 values (2,default);
select a,b from t1;
create table t2 like t1;
show create table t2;
describe t2;
insert into t2 (a) values (1);
select * from t2;
insert into t2 values (2,default);
select a,b from t2;
drop table t2;
drop table t1;

create table t1 (a int, b int generated always as (a % 2) stored);
show create table t1;
describe t1;
insert into t1 (a) values (1);
select * from t1;
insert into t1 values (2,default);
select a,b from t1;
drop table t1;


create table t2 (a int);
create table t1 (a int, b int generated always as (a % 2) stored references t2(a));
show create table t1;
drop table t1;
create table t1 (a int, b int generated always as (a % 2) virtual);
--error 1064
alter table t1 modify b int generated always as (a % 2) stored references t2(a);
show create table t1;
drop table t1;
drop table t2;
--echo FK options
create table t1(a int,  b int as (a % 2), c int as (a) stored);
create table t2 (a int);
--error ER_KEY_COLUMN_DOES_NOT_EXITS
alter table t1 add constraint foreign key fk(d) references t2(a);
--error ER_KEY_BASED_ON_GENERATED_COLUMN
alter table t1 add constraint foreign key fk(b) references t2(a);
--error ER_WRONG_FK_OPTION_FOR_GENERATED_COLUMN
alter table t1 add constraint foreign key fk(c) references t2(a) on delete set null;
--error ER_WRONG_FK_OPTION_FOR_GENERATED_COLUMN
alter table t1 add constraint foreign key fk(c) references t2(a) on update set null;
--error ER_WRONG_FK_OPTION_FOR_GENERATED_COLUMN
alter table t1 add constraint foreign key fk(c) references t2(a) on update cascade;
drop table t1;
drop table t2;
--echo Generated alwasy is optional
create table t1 (a int, b int as (a % 2) virtual);
show create table t1;
describe t1;
drop table t1;
create table t1 (a int, b int as (a % 2) stored);
show create table t1;
describe t1;
drop table t1;
--echo Default should be non-stored column
create table t1 (a int, b int as (a % 2));
show create table t1;
describe t1;
drop table t1;
--echo Expression can be constant 
create table t1 (a int, b int as (5 * 2));
show create table t1;
describe t1;
drop table t1;
--echo Test generated columns referencing other generated columns
create table t1 (a int unique, b int generated always as(-a) virtual, c int generated always as (b + 1) virtual);
insert into t1 (a) values (1), (2);
select * from t1;
insert into t1(a) values (1) on duplicate key update a=3;
select * from t1;
update t1 set a=4 where a=2;
select * from t1;
drop table t1;

--error ER_GENERATED_COLUMN_NON_PRIOR
create table t1 (a int, b int generated always as(-b) virtual, c int generated always as (b + 1) virtual);
--error ER_GENERATED_COLUMN_NON_PRIOR
create table t1 (a int, b int generated always as(-c) virtual, c int generated always as (b + 1) virtual);
--error ER_GENERATED_COLUMN_REF_AUTO_INC
CREATE TABLE t1 (pk INTEGER AUTO_INCREMENT, col_int_nokey INTEGER GENERATED ALWAYS AS (pk + col_int_key) STORED, col_int_key INTEGER, PRIMARY KEY (pk));

--echo # Bug#20339347: FAIL TO USE CREATE ....SELECT STATEMENT TO CREATE A NEW TABLE
create table t1 (a int, b int generated always as(-a) virtual, c int generated always as (b + 1) stored);
insert into t1(a) values(1),(2);
create table tt as select * from t1;
select * from t1;
select * from tt;
drop table t1,tt;
