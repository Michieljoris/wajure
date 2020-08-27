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
 (data (i32.const 4308) "Too few args passed to test\00Too many args passed to test\00Too few args passed to anon_2\00Too many args passed to anon_2\00\01\00\00\00\00\00\00\00\00\00\00\00Z\11\00\00\02\08\00\00\01\00\00\00\d4\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00~\11\00\00\02\08\00\00\02\00\00\00\d4\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\a2\11\00\00\02\08\00\00\03\00\00\00\d4\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\c6\11\00\00\02\08\00\00\05\00\00\00\d4\10\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\ea\11\00\00\1e\00\00\00\c6\11\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c6\11\00\00\a1\03\00\00\a2\11\00\00\ea\11\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a2\11\00\00~p\00\00~\11\00\00\06\12\00\00\01\00\00\00\00\00\00\00\00\00\00\00~\11\00\00A\9f\0d\00Z\11\00\00\"\12\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\12\00\00\01\10\00\00\00\00\00\00\d4\10\00\00>\12\00\00A\9f\0d\00\01\00\00\00\00\00\00\00\00\00\00\00~\12\00\00\18\ff\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a2\12\00\00\00\00\00\00~\12\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\be\12\00\00\01\10\00\00\00\00\00\00\d4\10\00\00\a2\12\00\00\03\00\00\00")
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
 (import "env" "retain" (func $retain (param i32) (result i32)))
 (import "env" "release" (func $release (param i32)))
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
 (elem (i32.const 0) $do_4 $test $log_int $printf_)
 (export "test" (func $test))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $do_4 (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (block $do_4 (result i32)
   (if
    (i32.eq
     (call $check_args_count
      (i32.const 0)
      (local.get $1)
      (i32.const 0)
     )
     (i32.const 0)
    )
    (call $runtime_error
     (i32.const 4395)
    )
    (if
     (i32.eq
      (call $check_args_count
       (i32.const 0)
       (local.get $1)
       (i32.const 0)
      )
      (i32.const 1)
     )
     (call $runtime_error
      (i32.const 4365)
     )
     (nop)
    )
   )
   (block $store_args_in_locals_3
    (local.set $2
     (i32.sub
      (global.get $stack_pointer)
      (i32.const 0)
     )
    )
   )
   (i32.const 4698)
  )
 )
 (func $test (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  (block $do_6 (result i32)
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
     (i32.const 4336)
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
      (i32.const 4308)
     )
     (nop)
    )
   )
   (block $store_args_in_locals_1
    (local.set $2
     (i32.sub
      (global.get $stack_pointer)
      (i32.const 8)
     )
    )
    (local.set $3
     (i32.load offset=4
      (local.get $2)
     )
    )
    (local.set $4
     (i32.load
      (local.get $2)
     )
    )
   )
   (call $print_fn
    (i32.const 0)
    (call $new_lval_list
     (call $list_cons
      (block $lambda_call_5 (result i32)
       (global.set $stack_pointer
        (i32.add
         (global.get $stack_pointer)
         (i32.const 0)
        )
       )
       (local.set $5
        (call_indirect (type $i32_i32_=>_i32)
         (i32.load offset=12
          (call $first_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.const 4798)
             (i32.const 0)
            )
           )
          )
         )
         (i32.const 0)
         (i32.load16_u offset=2
          (call $first_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.const 4798)
             (i32.const 0)
            )
           )
          )
         )
        )
       )
       (global.set $stack_pointer
        (i32.sub
         (global.get $stack_pointer)
         (i32.const 0)
        )
       )
       (local.get $5)
      )
      (i32.const 0)
     )
    )
   )
  )
 )
)
