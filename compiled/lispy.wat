(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (i32.const 4280) "Too many args passed to test\00Too few args passed to test\00Too many args passed to foo\00Too few args passed to foo\00from foo\00\01\00\00\00\00\00\00\00\00\00\00\00A\11\00\00\02\t\00\00\00\00\00\00(\11\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00e\11\00\00\02\08\00\00{\00\00\00\b8\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\89\11\00\00\02\08\00\00\01\00\00\00\b8\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\ad\11\00\00\02\08\00\00\02\00\00\00\b8\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\d1\11\00\00\02\08\00\00\03\00\00\00\b8\10\00\00\00\00\00\00\ff\ff\ff\ff")
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "stack_pointer" (global $stack_pointer (mut i32)))
 (import "env" "log_int" (func $log_int (param i32)))
 (import "env" "log_string" (func $log_string (param i32)))
 (import "env" "log_string_n" (func $log_string_n (param i32 i32)))
 (import "env" "runtime_error" (func $runtime_error (param i32)))
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
 (import "env" "list_cons" (func $list_cons (param i32 i32) (result i32)))
 (import "env" "_strcpy" (func $_strcpy (param i32 i32) (result i32)))
 (import "env" "print_slot_size" (func $print_slot_size))
 (import "env" "wval_print" (func $wval_print (param i32)))
 (import "env" "make_lval_wasm_lambda" (func $make_lval_wasm_lambda (param i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "init_rest_args" (func $init_rest_args (param i32 i32)))
 (import "env" "check_args_count" (func $check_args_count (param i32 i32 i32) (result i32)))
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
 (import "env" "print_fn" (func $print_fn (param i32 i32) (result i32)))
 (import "env" "pr_fn" (func $pr_fn (param i32 i32) (result i32)))
 (import "env" "debug_fn" (func $debug_fn (param i32 i32) (result i32)))
 (import "env" "boolean_fn" (func $boolean_fn (param i32 i32) (result i32)))
 (import "env" "hash_fn" (func $hash_fn (param i32 i32) (result i32)))
 (table $0 4 4 funcref)
 (elem (i32.const 0) $foo $test $log_int $printf_)
 (export "test" (func $test))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $foo (param $0 i32) (param $1 i32) (result i32)
  (block $body (result i32)
   (block $process_args
    (if
     (i32.eq
      (call $check_args_count
       (i32.const 2)
       (local.get $1)
       (i32.const 2)
      )
      (i32.const 0)
     )
     (call $runtime_error
      (i32.const 4337)
     )
     (if
      (i32.eq
       (call $check_args_count
        (i32.const 2)
        (local.get $1)
        (i32.const 2)
       )
       (i32.const 1)
      )
      (call $runtime_error
       (i32.const 4365)
      )
      (nop)
     )
    )
    (call $init_rest_args
     (i32.sub
      (global.get $stack_pointer)
      (i32.mul
       (i32.const 4)
       (local.get $1)
      )
     )
     (i32.sub
      (local.get $1)
      (i32.const 1)
     )
    )
   )
   (call $print_fn
    (i32.const 0)
    (call $new_lval_list
     (call $list_cons
      (i32.const 4417)
      (call $list_cons
       (i32.load offset=4
        (i32.sub
         (global.get $stack_pointer)
         (i32.const 8)
        )
       )
       (call $list_cons
        (i32.load
         (i32.sub
          (global.get $stack_pointer)
          (i32.const 8)
         )
        )
        (i32.const 0)
       )
      )
     )
    )
   )
  )
 )
 (func $test (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (local $3 i32)
  (block $body (result i32)
   (if
    (i32.eq
     (call $check_args_count
      (i32.const 2)
      (local.get $1)
      (i32.const 0)
     )
     (i32.const 0)
    )
    (call $runtime_error
     (i32.const 4280)
    )
    (if
     (i32.eq
      (call $check_args_count
       (i32.const 2)
       (local.get $1)
       (i32.const 0)
      )
      (i32.const 1)
     )
     (call $runtime_error
      (i32.const 4309)
     )
     (nop)
    )
   )
   (block $let (result i32)
    (local.set $2
     (call $make_lval_wasm_lambda
      (i32.const 0)
      (i32.const 2)
      (i32.const 2)
      (i32.const 0)
      (i32.const 0)
      (i32.const 0)
     )
    )
    (block $lambda_call_1 (result i32)
     (i32.store offset=12
      (global.get $stack_pointer)
      (i32.const 4453)
     )
     (i32.store offset=8
      (global.get $stack_pointer)
      (i32.const 4489)
     )
     (i32.store offset=4
      (global.get $stack_pointer)
      (i32.const 4525)
     )
     (i32.store
      (global.get $stack_pointer)
      (i32.const 4561)
     )
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 16)
      )
     )
     (local.set $3
      (call_indirect (type $i32_i32_=>_i32)
       (i32.load offset=12
        (local.get $2)
       )
       (i32.const 4)
       (i32.load16_u offset=2
        (local.get $2)
       )
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 16)
      )
     )
     (local.get $3)
    )
   )
  )
 )
)
