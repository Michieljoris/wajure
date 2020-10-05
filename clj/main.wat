(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32) (result i32)))
 (type $none_=>_none (func))
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
 (data (global.get $data_offset) "\0f\0f\0f\0fnot a fn\00fn_name???\00\01\00\00\00\00\00\00\00\00\00\00\00(\00\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00L\00\00\00\02\08\00\00\02\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00p\00\00\00\02\08\00\00\03\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\94\00\00\00\19\ff\19\00\02\00\00\00\00\00\00\00\00\00\00\00\84\00\00\00\18\00\00\00<\00\00\00`\00\00\00\84\00\00\00\a8\00\00\00\03\00\00\00\b4\00\00\00\00\00\00\00\b4\00\00\00\01\00\00\00\18\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $call#0 $call#1 $call#2 $call#3 $call#4 $call#5 $call#6 $call#7 $call#8 $call#9 $call#10 $call#11 $call#12 $call#13 $call#14 $call#15 $call#16 $call#17 $call#18 $call#19 $call#20 $validate_fn $partial_fn $apply_fn $main_0#24 $main_0)
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "stack_pointer" (global $stack_pointer (mut i32)))
 (import "env" "data_offset" (global $data_offset i32))
 (import "env" "fn_table_offset" (global $fn_table_offset i32))
 (import "env" "fn:test.test-partial/test-partial_10" (global $fn:test.test-partial/test-partial_10 i32))
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
 (import "env" "new_lval_vector" (func $new_lval_vector (param i32) (result i32)))
 (import "env" "lalloc_size" (func $lalloc_size (param i32) (result i32)))
 (import "env" "lalloc_type" (func $lalloc_type (param i32) (result i32)))
 (import "env" "retain" (func $retain (param i32) (result i32)))
 (import "env" "release" (func $release (param i32)))
 (import "env" "prefix_list" (func $prefix_list (param i32 i32) (result i32)))
 (import "env" "_strcpy" (func $_strcpy (param i32 i32) (result i32)))
 (import "env" "print_slot_size" (func $print_slot_size))
 (import "env" "wval_print" (func $wval_print (param i32)))
 (import "env" "make_lval_wasm_lambda" (func $make_lval_wasm_lambda (param i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "get_wval_type" (func $get_wval_type (param i32) (result i32)))
 (import "env" "get_wval_subtype" (func $get_wval_subtype (param i32) (result i32)))
 (import "env" "get_wval_fn_table_index" (func $get_wval_fn_table_index (param i32) (result i32)))
 (import "env" "get_wval_param_count" (func $get_wval_param_count (param i32) (result i32)))
 (import "env" "get_wval_min_param_count" (func $get_wval_min_param_count (param i32) (result i32)))
 (import "env" "get_wval_has_rest_arg" (func $get_wval_has_rest_arg (param i32) (result i32)))
 (import "env" "get_wval_rest_arg_index" (func $get_wval_rest_arg_index (param i32) (result i32)))
 (import "env" "get_wval_closure" (func $get_wval_closure (param i32) (result i32)))
 (import "env" "get_wval_partials" (func $get_wval_partials (param i32) (result i32)))
 (import "env" "get_wval_partial_count" (func $get_wval_partial_count (param i32) (result i32)))
 (import "env" "bundle_rest_args" (func $bundle_rest_args (param i32 i32)))
 (import "env" "check_args_count" (func $check_args_count (param i32 i32 i32) (result i32)))
 (import "env" "rewrite_pointers" (func $rewrite_pointers (param i32 i32 i32)))
 (import "env" "new_cell" (func $new_cell (param i32 i32) (result i32)))
 (import "env" "dbg" (func $dbg (param i32)))
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
 (import "env" "is_nil_fn" (func $is_nil_fn (param i32 i32) (result i32)))
 (import "env" "foo_fn" (func $foo_fn (param i32 i32) (result i32)))
 (import "env" "print_fn" (func $print_fn (param i32 i32) (result i32)))
 (import "env" "pr_fn" (func $pr_fn (param i32 i32) (result i32)))
 (import "env" "debug_fn" (func $debug_fn (param i32 i32) (result i32)))
 (import "env" "boolean_fn" (func $boolean_fn (param i32 i32) (result i32)))
 (import "env" "hash_fn" (func $hash_fn (param i32 i32) (result i32)))
 (import "env" "str_fn" (func $str_fn (param i32 i32) (result i32)))
 (import "env" "read_string_fn" (func $read_string_fn (param i32 i32) (result i32)))
 (export "main" (func $main_0))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $call#0 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_=>_i32)
   (local.get $0)
   (local.get $1)
  )
 )
 (func $call#1 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#2 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#3 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#4 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#5 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#6 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#7 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#8 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#9 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#10 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#11 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#12 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#13 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#14 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#15 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (i32.load offset=56 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#16 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (i32.load offset=56 align=2
    (local.get $2)
   )
   (i32.load offset=60 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#17 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (i32.load offset=56 align=2
    (local.get $2)
   )
   (i32.load offset=60 align=2
    (local.get $2)
   )
   (i32.load offset=64 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#18 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (i32.load offset=56 align=2
    (local.get $2)
   )
   (i32.load offset=60 align=2
    (local.get $2)
   )
   (i32.load offset=64 align=2
    (local.get $2)
   )
   (i32.load offset=68 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#19 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (i32.load offset=56 align=2
    (local.get $2)
   )
   (i32.load offset=60 align=2
    (local.get $2)
   )
   (i32.load offset=64 align=2
    (local.get $2)
   )
   (i32.load offset=68 align=2
    (local.get $2)
   )
   (i32.load offset=72 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $call#20 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (call_indirect (type $i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_i32_=>_i32)
   (local.get $0)
   (i32.load align=2
    (local.get $2)
   )
   (i32.load offset=4 align=2
    (local.get $2)
   )
   (i32.load offset=8 align=2
    (local.get $2)
   )
   (i32.load offset=12 align=2
    (local.get $2)
   )
   (i32.load offset=16 align=2
    (local.get $2)
   )
   (i32.load offset=20 align=2
    (local.get $2)
   )
   (i32.load offset=24 align=2
    (local.get $2)
   )
   (i32.load offset=28 align=2
    (local.get $2)
   )
   (i32.load offset=32 align=2
    (local.get $2)
   )
   (i32.load offset=36 align=2
    (local.get $2)
   )
   (i32.load offset=40 align=2
    (local.get $2)
   )
   (i32.load offset=44 align=2
    (local.get $2)
   )
   (i32.load offset=48 align=2
    (local.get $2)
   )
   (i32.load offset=52 align=2
    (local.get $2)
   )
   (i32.load offset=56 align=2
    (local.get $2)
   )
   (i32.load offset=60 align=2
    (local.get $2)
   )
   (i32.load offset=64 align=2
    (local.get $2)
   )
   (i32.load offset=68 align=2
    (local.get $2)
   )
   (i32.load offset=72 align=2
    (local.get $2)
   )
   (i32.load offset=76 align=2
    (local.get $2)
   )
   (local.get $1)
  )
 )
 (func $validate_fn (param $0 i32) (param $1 i32)
  (local $2 i32)
  (if
   (i32.ne
    (call $get_wval_type
     (local.get $0)
    )
    (i32.const 25)
   )
   (call $runtime_error
    (i32.const 2)
    (i32.add
     (global.get $data_offset)
     (i32.const 4)
    )
   )
   (nop)
  )
  (if
   (call $get_wval_has_rest_arg
    (local.get $0)
   )
   (if
    (i32.lt_u
     (local.get $1)
     (call $get_wval_min_param_count
      (local.get $0)
     )
    )
    (call $runtime_error
     (i32.const 0)
     (i32.add
      (global.get $data_offset)
      (i32.const 13)
     )
    )
    (nop)
   )
   (if
    (i32.gt_u
     (local.get $1)
     (local.tee $2
      (call $get_wval_param_count
       (local.get $0)
      )
     )
    )
    (call $runtime_error
     (i32.const 1)
     (i32.add
      (global.get $data_offset)
      (i32.const 13)
     )
    )
    (if
     (i32.lt_u
      (local.get $1)
      (local.get $2)
     )
     (call $runtime_error
      (i32.const 0)
      (i32.add
       (global.get $data_offset)
       (i32.const 13)
      )
     )
     (nop)
    )
   )
  )
 )
 (func $partial_fn (param $0 i32) (param $1 i32) (param $2 i32)
  (nop)
 )
 (func $apply_fn (param $0 i32) (param $1 i32) (param $2 i32)
  (nop)
 )
 (func $main_0#24 (param $0 i32) (param $1 i32) (param $2 i32) (param $3 i32) (result i32)
  (block $do_491 (result i32)
   (call $new_lval_vector
    (call $prefix_list
     (call $retain
      (local.get $1)
     )
     (call $prefix_list
      (call $retain
       (local.get $2)
      )
      (call $prefix_list
       (call $retain
        (local.get $3)
       )
       (i32.const 0)
      )
     )
    )
   )
  )
 )
 (func $main_0 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  (local $6 i32)
  (local $7 i32)
  (local $8 i32)
  (local $9 i32)
  (local $10 i32)
  (local $11 i32)
  (local $12 i32)
  (local $13 i32)
  (local $14 i32)
  (local $15 i32)
  (local $16 i32)
  (local $17 i32)
  (local $18 i32)
  (local $19 i32)
  (local $20 i32)
  (local $21 i32)
  (local $22 i32)
  (local $23 i32)
  (local $24 i32)
  (local $25 i32)
  (local $26 i32)
  (local $27 i32)
  (local $28 i32)
  (local $29 i32)
  (local $30 i32)
  (local $31 i32)
  (local $32 i32)
  (local $33 i32)
  (local $34 i32)
  (local $35 i32)
  (local $36 i32)
  (local $37 i32)
  (local $38 i32)
  (local $39 i32)
  (local $40 i32)
  (local $41 i32)
  (local $42 i32)
  (local $43 i32)
  (local $44 i32)
  (local $45 i32)
  (local $46 i32)
  (block $do_512 (result i32)
   (local.set $44
    (block $let_508 (result i32)
     (local.set $4
      (block $main_0#24_492 (result i32)
       (local.set $3
        (call $lalloc_size
         (i32.const 0)
        )
       )
       (call $make_lval_wasm_lambda
        (i32.add
         (global.get $fn_table_offset)
         (i32.const 24)
        )
        (i32.const 3)
        (i32.const 0)
        (local.get $3)
        (i32.const 0)
        (i32.const 0)
       )
      )
     )
     (local.set $10
      (block $partial_call_494 (result i32)
       (local.set $7
        (call $lalloc_size
         (i32.mul
          (local.tee $6
           (i32.add
            (local.tee $5
             (call $get_wval_partial_count
              (local.get $4)
             )
            )
            (i32.const 1)
           )
          )
          (i32.const 4)
         )
        )
       )
       (local.set $8
        (call $get_wval_partials
         (local.get $4)
        )
       )
       (local.set $9
        (local.get $7)
       )
       (loop $copy_partials_493
        (if
         (local.get $5)
         (block
          (i32.store align=2
           (local.get $9)
           (call $retain
            (i32.load align=2
             (local.get $8)
            )
           )
          )
          (local.set $5
           (i32.sub
            (local.get $5)
            (i32.const 1)
           )
          )
          (local.set $8
           (i32.add
            (local.get $8)
            (i32.const 4)
           )
          )
          (local.set $9
           (i32.add
            (local.get $9)
            (i32.const 4)
           )
          )
          (br $copy_partials_493)
         )
         (nop)
        )
       )
       (i32.store align=2
        (local.get $9)
        (i32.add
         (global.get $data_offset)
         (i32.const 40)
        )
       )
       (call $make_lval_wasm_lambda
        (call $get_wval_fn_table_index
         (local.get $4)
        )
        (call $get_wval_param_count
         (local.get $4)
        )
        (call $get_wval_has_rest_arg
         (local.get $4)
        )
        (call $retain
         (call $get_wval_closure
          (local.get $4)
         )
        )
        (local.get $7)
        (local.get $6)
       )
      )
     )
     (local.set $16
      (block $partial_call_496 (result i32)
       (local.set $13
        (call $lalloc_size
         (i32.mul
          (local.tee $12
           (i32.add
            (local.tee $11
             (call $get_wval_partial_count
              (local.get $10)
             )
            )
            (i32.const 1)
           )
          )
          (i32.const 4)
         )
        )
       )
       (local.set $14
        (call $get_wval_partials
         (local.get $10)
        )
       )
       (local.set $15
        (local.get $13)
       )
       (loop $copy_partials_495
        (if
         (local.get $11)
         (block
          (i32.store align=2
           (local.get $15)
           (call $retain
            (i32.load align=2
             (local.get $14)
            )
           )
          )
          (local.set $11
           (i32.sub
            (local.get $11)
            (i32.const 1)
           )
          )
          (local.set $14
           (i32.add
            (local.get $14)
            (i32.const 4)
           )
          )
          (local.set $15
           (i32.add
            (local.get $15)
            (i32.const 4)
           )
          )
          (br $copy_partials_495)
         )
         (nop)
        )
       )
       (i32.store align=2
        (local.get $15)
        (i32.add
         (global.get $data_offset)
         (i32.const 76)
        )
       )
       (call $make_lval_wasm_lambda
        (call $get_wval_fn_table_index
         (local.get $10)
        )
        (call $get_wval_param_count
         (local.get $10)
        )
        (call $get_wval_has_rest_arg
         (local.get $10)
        )
        (call $retain
         (call $get_wval_closure
          (local.get $10)
         )
        )
        (local.get $13)
        (local.get $12)
       )
      )
     )
     (local.set $25
      (block $sys_call_499 (result i32)
       (local.set $24
        (call $print_fn
         (i32.const 0)
         (local.tee $23
          (call $new_lval_list
           (call $prefix_list
            (block $lambda_call_498 (result i32)
             (local.set $17
              (call $lalloc_size
               (i32.const 12)
              )
             )
             (local.set $18
              (i32.add
               (i32.const 3)
               (call $get_wval_partial_count
                (local.get $4)
               )
              )
             )
             (call_indirect (type $i32_i32_=>_none)
              (local.get $4)
              (local.get $18)
              (i32.const 21)
             )
             (block $args_497
              (local.set $19
               (i32.mul
                (call $get_wval_partial_count
                 (local.get $4)
                )
                (i32.const 4)
               )
              )
              (if
               (call $get_wval_partial_count
                (local.get $4)
               )
               (memory.copy
                (local.get $17)
                (call $get_wval_partials
                 (local.get $4)
                )
                (local.get $19)
               )
               (nop)
              )
              (local.set $20
               (i32.add
                (local.get $17)
                (local.get $19)
               )
              )
              (i32.store align=2
               (local.get $20)
               (i32.add
                (global.get $data_offset)
                (i32.const 40)
               )
              )
              (i32.store offset=4 align=2
               (local.get $20)
               (i32.add
                (global.get $data_offset)
                (i32.const 76)
               )
              )
              (i32.store offset=8 align=2
               (local.get $20)
               (i32.add
                (global.get $data_offset)
                (i32.const 112)
               )
              )
             )
             (if
              (call $get_wval_has_rest_arg
               (local.get $4)
              )
              (call $bundle_rest_args
               (i32.add
                (local.get $17)
                (i32.mul
                 (i32.const 4)
                 (local.tee $21
                  (call $get_wval_rest_arg_index
                   (local.get $4)
                  )
                 )
                )
               )
               (i32.sub
                (local.get $18)
                (local.get $21)
               )
              )
              (nop)
             )
             (local.set $22
              (call_indirect (type $i32_i32_i32_i32_=>_i32)
               (call $get_wval_closure
                (local.get $4)
               )
               (call $get_wval_fn_table_index
                (local.get $4)
               )
               (local.get $17)
               (local.get $18)
               (call $get_wval_param_count
                (local.get $4)
               )
              )
             )
             (call $release
              (local.get $17)
             )
             (local.get $22)
            )
            (i32.const 0)
           )
          )
         )
        )
       )
       (call $release
        (local.get $23)
       )
       (local.get $24)
      )
     )
     (local.set $34
      (block $sys_call_502 (result i32)
       (local.set $33
        (call $print_fn
         (i32.const 0)
         (local.tee $32
          (call $new_lval_list
           (call $prefix_list
            (block $lambda_call_501 (result i32)
             (local.set $26
              (call $lalloc_size
               (i32.const 8)
              )
             )
             (local.set $27
              (i32.add
               (i32.const 2)
               (call $get_wval_partial_count
                (local.get $10)
               )
              )
             )
             (call_indirect (type $i32_i32_=>_none)
              (local.get $10)
              (local.get $27)
              (i32.const 21)
             )
             (block $args_500
              (local.set $28
               (i32.mul
                (call $get_wval_partial_count
                 (local.get $10)
                )
                (i32.const 4)
               )
              )
              (if
               (call $get_wval_partial_count
                (local.get $10)
               )
               (memory.copy
                (local.get $26)
                (call $get_wval_partials
                 (local.get $10)
                )
                (local.get $28)
               )
               (nop)
              )
              (local.set $29
               (i32.add
                (local.get $26)
                (local.get $28)
               )
              )
              (i32.store align=2
               (local.get $29)
               (i32.add
                (global.get $data_offset)
                (i32.const 76)
               )
              )
              (i32.store offset=4 align=2
               (local.get $29)
               (i32.add
                (global.get $data_offset)
                (i32.const 112)
               )
              )
             )
             (if
              (call $get_wval_has_rest_arg
               (local.get $10)
              )
              (call $bundle_rest_args
               (i32.add
                (local.get $26)
                (i32.mul
                 (i32.const 4)
                 (local.tee $30
                  (call $get_wval_rest_arg_index
                   (local.get $10)
                  )
                 )
                )
               )
               (i32.sub
                (local.get $27)
                (local.get $30)
               )
              )
              (nop)
             )
             (local.set $31
              (call_indirect (type $i32_i32_i32_i32_=>_i32)
               (call $get_wval_closure
                (local.get $10)
               )
               (call $get_wval_fn_table_index
                (local.get $10)
               )
               (local.get $26)
               (local.get $27)
               (call $get_wval_param_count
                (local.get $10)
               )
              )
             )
             (call $release
              (local.get $26)
             )
             (local.get $31)
            )
            (i32.const 0)
           )
          )
         )
        )
       )
       (call $release
        (local.get $32)
       )
       (local.get $33)
      )
     )
     (block $let_body_result_507 (result i32)
      (local.set $43
       (block $sys_call_505 (result i32)
        (local.set $42
         (call $print_fn
          (i32.const 0)
          (local.tee $41
           (call $new_lval_list
            (call $prefix_list
             (block $lambda_call_504 (result i32)
              (local.set $35
               (call $lalloc_size
                (i32.const 4)
               )
              )
              (local.set $36
               (i32.add
                (i32.const 1)
                (call $get_wval_partial_count
                 (local.get $16)
                )
               )
              )
              (call_indirect (type $i32_i32_=>_none)
               (local.get $16)
               (local.get $36)
               (i32.const 21)
              )
              (block $args_503
               (local.set $37
                (i32.mul
                 (call $get_wval_partial_count
                  (local.get $16)
                 )
                 (i32.const 4)
                )
               )
               (if
                (call $get_wval_partial_count
                 (local.get $16)
                )
                (memory.copy
                 (local.get $35)
                 (call $get_wval_partials
                  (local.get $16)
                 )
                 (local.get $37)
                )
                (nop)
               )
               (local.set $38
                (i32.add
                 (local.get $35)
                 (local.get $37)
                )
               )
               (i32.store align=2
                (local.get $38)
                (i32.add
                 (global.get $data_offset)
                 (i32.const 112)
                )
               )
              )
              (if
               (call $get_wval_has_rest_arg
                (local.get $16)
               )
               (call $bundle_rest_args
                (i32.add
                 (local.get $35)
                 (i32.mul
                  (i32.const 4)
                  (local.tee $39
                   (call $get_wval_rest_arg_index
                    (local.get $16)
                   )
                  )
                 )
                )
                (i32.sub
                 (local.get $36)
                 (local.get $39)
                )
               )
               (nop)
              )
              (local.set $40
               (call_indirect (type $i32_i32_i32_i32_=>_i32)
                (call $get_wval_closure
                 (local.get $16)
                )
                (call $get_wval_fn_table_index
                 (local.get $16)
                )
                (local.get $35)
                (local.get $36)
                (call $get_wval_param_count
                 (local.get $16)
                )
               )
              )
              (call $release
               (local.get $35)
              )
              (local.get $40)
             )
             (i32.const 0)
            )
           )
          )
         )
        )
        (call $release
         (local.get $41)
        )
        (local.get $42)
       )
      )
      (block $release_locals_for_let_506
       (call $release
        (local.get $4)
       )
       (call $release
        (local.get $10)
       )
       (call $release
        (local.get $16)
       )
       (call $release
        (local.get $25)
       )
       (call $release
        (local.get $34)
       )
      )
      (local.get $43)
     )
    )
   )
   (block $do_body_result_511 (result i32)
    (local.set $46
     (block $lambda_call_509 (result i32)
      (local.set $45
       (call_indirect (type $i32_=>_i32)
        (i32.const 0)
        (global.get $fn:test.test-partial/test-partial_10)
       )
      )
      (local.get $45)
     )
    )
    (block $release_locals_for_do_510
     (call $release
      (local.get $44)
     )
    )
    (local.get $46)
   )
  )
 )
 ;; custom section "symbol_table", size 27
 ;; custom section "deps", size 37
 ;; custom section "data_size", size 3, contents: "212"
 ;; custom section "fn_table_size", size 2, contents: "26"
)
