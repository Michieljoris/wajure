(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32) (result i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $data_offset) "\0f\0f\0f\0f\01\00\00\00\00\00\00\00\00\00\00\00\14\00\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\008\00\00\00\19\ff\15\00\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\04\00\00\00(\00\00\00L\00\00\00\01\00\00\00P\00\00\00\00\00\00\00P\00\00\00\01\00\00\00\18\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $call#0 $call#1 $call#2 $call#3 $call#4 $call#5 $call#6 $call#7 $call#8 $call#9 $call#10 $call#11 $call#12 $call#13 $call#14 $call#15 $call#16 $call#17 $call#18 $call#19 $call#20 $main)
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "stack_pointer" (global $stack_pointer (mut i32)))
 (import "env" "data_offset" (global $data_offset i32))
 (import "env" "fn_table_offset" (global $fn_table_offset i32))
 (import "env" "log_int" (func $log_int (param i32)))
 (import "env" "log_string" (func $log_string (param i32)))
 (import "env" "log_string_n" (func $log_string_n (param i32 i32)))
 (import "env" "runtime_error" (func $runtime_error (param i32 i32)))
 (import "env" "lval_print" (func $lval_print (param i32)))
 (import "env" "lval_println" (func $lval_println (param i32)))
 (import "env" "printf_" (func $printf_ (param i32 i32) (result i32)))
 (import "env" "make_lval_num" (func $make_lval_num (param i32) (result i32)))
 (import "env" "make_lval_nil" (func $make_lval_nil (result i32)))
 (import "env" "make_lval_true" (func $make_lval_true (result i32)))
 (import "env" "make_lval_false" (func $make_lval_false (result i32)))
 (import "env" "make_lval_str" (func $make_lval_str (param i32) (result i32)))
 (import "env" "make_lval_list" (func $make_lval_list (result i32)))
 (import "env" "new_lval_list" (func $new_lval_list (param i32) (result i32)))
 (import "env" "make_lval_sym" (func $make_lval_sym (param i32) (result i32)))
 (import "env" "lalloc_size" (func $lalloc_size (param i32) (result i32)))
 (import "env" "lalloc_type" (func $lalloc_type (param i32) (result i32)))
 (import "env" "retain" (func $retain (param i32) (result i32)))
 (import "env" "release" (func $release (param i32)))
 (import "env" "list_cons" (func $list_cons (param i32 i32) (result i32)))
 (import "env" "_strcpy" (func $_strcpy (param i32 i32) (result i32)))
 (import "env" "print_slot_size" (func $print_slot_size))
 (import "env" "wval_print" (func $wval_print (param i32)))
 (import "env" "make_lval_wasm_lambda" (func $make_lval_wasm_lambda (param i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "get_wval_type" (func $get_wval_type (param i32) (result i32)))
 (import "env" "get_wval_subtype" (func $get_wval_subtype (param i32) (result i32)))
 (import "env" "get_wval_fn_table_index" (func $get_wval_fn_table_index (param i32) (result i32)))
 (import "env" "get_wval_param_count" (func $get_wval_param_count (param i32) (result i32)))
 (import "env" "get_wval_has_rest_arg" (func $get_wval_has_rest_arg (param i32) (result i32)))
 (import "env" "get_wval_closure" (func $get_wval_closure (param i32) (result i32)))
 (import "env" "get_wval_partials" (func $get_wval_partials (param i32) (result i32)))
 (import "env" "get_wval_partial_count" (func $get_wval_partial_count (param i32) (result i32)))
 (import "env" "init_rest_args" (func $init_rest_args (param i32 i32)))
 (import "env" "check_args_count" (func $check_args_count (param i32 i32 i32) (result i32)))
 (import "env" "rewrite_pointers" (func $rewrite_pointers (param i32 i32 i32)))
 (import "env" "add_fn" (func $add_fn (param i32 i32) (result i32)))
 (import "env" "sub_fn" (func $sub_fn (param i32 i32) (result i32)))
 (import "env" "mul_fn" (func $mul_fn (param i32 i32) (result i32)))
 (import "env" "div_fn" (func $div_fn (param i32 i32) (result i32)))
 (import "env" "gt_fn" (func $gt_fn (param i32 i32) (result i32)))
 (import "env" "lt_fn" (func $lt_fn (param i32 i32) (result i32)))
 (import "env" "gte_fn" (func $gte_fn (param i32 i32) (result i32)))
 (import "env" "lte_fn" (func $lte_fn (param i32 i32) (result i32)))
 (import "env" "eq_fn" (func $eq_fn (param i32 i32) (result i32)))
 (import "env" "not_eq_fn" (func $not_eq_fn (param i32 i32) (result i32)))
 (import "env" "cons_fn" (func $cons_fn (param i32 i32) (result i32)))
 (import "env" "first_fn" (func $first_fn (param i32 i32) (result i32)))
 (import "env" "list_fn" (func $list_fn (param i32 i32) (result i32)))
 (import "env" "rest_fn" (func $rest_fn (param i32 i32) (result i32)))
 (import "env" "concat_fn" (func $concat_fn (param i32 i32) (result i32)))
 (import "env" "count_fn" (func $count_fn (param i32 i32) (result i32)))
 (import "env" "nth_fn" (func $nth_fn (param i32 i32) (result i32)))
 (import "env" "is_list_fn" (func $is_list_fn (param i32 i32) (result i32)))
 (import "env" "foo_fn" (func $foo_fn (param i32 i32) (result i32)))
 (import "env" "print_fn" (func $print_fn (param i32 i32) (result i32)))
 (import "env" "pr_fn" (func $pr_fn (param i32 i32) (result i32)))
 (import "env" "debug_fn" (func $debug_fn (param i32 i32) (result i32)))
 (import "env" "boolean_fn" (func $boolean_fn (param i32 i32) (result i32)))
 (import "env" "hash_fn" (func $hash_fn (param i32 i32) (result i32)))
 (import "env" "str_fn" (func $str_fn (param i32 i32) (result i32)))
 (import "env" "read_string_fn" (func $read_string_fn (param i32 i32) (result i32)))
 (export "call#0" (func $call#0))
 (export "call#1" (func $call#1))
 (export "call#2" (func $call#2))
 (export "call#3" (func $call#3))
 (export "call#4" (func $call#4))
 (export "call#5" (func $call#5))
 (export "call#6" (func $call#6))
 (export "call#7" (func $call#7))
 (export "call#8" (func $call#8))
 (export "call#9" (func $call#9))
 (export "call#10" (func $call#10))
 (export "call#11" (func $call#11))
 (export "call#12" (func $call#12))
 (export "call#13" (func $call#13))
 (export "call#14" (func $call#14))
 (export "call#15" (func $call#15))
 (export "call#16" (func $call#16))
 (export "call#17" (func $call#17))
 (export "call#18" (func $call#18))
 (export "call#19" (func $call#19))
 (export "call#20" (func $call#20))
 (export "main" (func $main))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $call#0 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (local.get $1)
  )
 )
 (func $call#1 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#2 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#3 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#4 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#5 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#6 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#7 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#8 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#9 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#10 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#11 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#12 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#13 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#14 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#15 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=56 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#16 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=56 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=60 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#17 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=56 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=60 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=64 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#18 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=56 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=60 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=64 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=68 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#19 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=56 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=60 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=64 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=68 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=72 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $call#20 (param $0 i32) (param $1 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (i32.load align=2
    (local.get $0)
   )
   (i32.load align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=4 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=8 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=12 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=16 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=20 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=24 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=28 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=32 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=36 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=40 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=44 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=48 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=52 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=56 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=60 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=64 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=68 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=72 align=2
    (global.get $stack_pointer)
   )
   (i32.load offset=76 align=2
    (global.get $stack_pointer)
   )
   (local.get $1)
  )
 )
 (func $main (param $0 i32) (param $1 i32) (result i32)
  (block $do_1 (result i32)
   (call $print_fn
    (i32.const 0)
    (call $new_lval_list
     (call $list_cons
      (i32.add
       (global.get $data_offset)
       (i32.const 20)
      )
      (i32.const 0)
     )
    )
   )
  )
 )
 ;; custom section "symbol_table", size 24
 ;; custom section "deps", size 0, contents: ""
 ;; custom section "data_size", size 3, contents: "112"
 ;; custom section "fn_table_size", size 2, contents: "22"
)
