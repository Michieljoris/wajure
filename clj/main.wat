(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $data_offset) "\0f\0f\0f\0fmain\00\01\00\00\00\00\00\00\00\00\00\00\00\19\00\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ffmain#1\00\01\00\00\00\00\00\00\00\00\00\00\00D\00\00\00\02\08\00\00{\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ffhello\00\01\00\00\00\00\00\00\00\00\00\00\00n\00\00\00\02\t\00\00\00\00\00\00X\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\92\00\00\00\02\08\00\00\02\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b6\00\00\00\02\08\00\00\03\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\00f\00\01\00\00\00\00\00\00\00\00\00\00\00\dd\00\00\00\19\ff\01\00\02\00\02\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\01\01\00\00\19\ff\02\00\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\t\00\00\004\00\00\00^\00\00\00\82\00\00\00\a6\00\00\00\cd\00\00\00\f1\00\00\00\15\01\00\00\05\00\00\00)\01\00\00\00\00\00\00)\01\00\00\02\00\00\00\18\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $main#1 $f $main)
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "stack_pointer" (global $stack_pointer (mut i32)))
 (import "env" "data_offset" (global $data_offset i32))
 (import "env" "fn_table_offset" (global $fn_table_offset i32))
 (import "env" "data:foo.core/foo" (global $data:foo.core/foo i32))
 (import "env" "fn:foo.core/foo" (global $fn:foo.core/foo i32))
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
 (export "main#1" (func $main#1))
 (export "f" (func $f))
 (export "main" (func $main))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $main#1 (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (local $3 i32)
  (block $do_4 (result i32)
   (if
    (i32.eq
     (call $check_args_count
      (i32.const 1)
      (local.get $1)
      (i32.const 0)
     )
     (i32.const 0)
    )
    (call $runtime_error
     (i32.const 1)
     (i32.add
      (global.get $data_offset)
      (i32.const 45)
     )
    )
    (if
     (i32.eq
      (call $check_args_count
       (i32.const 1)
       (local.get $1)
       (i32.const 0)
      )
      (i32.const 1)
     )
     (call $runtime_error
      (i32.const 0)
      (i32.add
       (global.get $data_offset)
       (i32.const 45)
      )
     )
     (nop)
    )
   )
   (block $store_args_in_locals_3
    (local.set $2
     (i32.sub
      (global.get $stack_pointer)
      (i32.const 4)
     )
    )
    (local.set $3
     (i32.load
      (local.get $2)
     )
    )
   )
   (call $retain
    (local.get $3)
   )
  )
 )
 (func $f (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (block $do_9 (result i32)
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
      (i32.const 1)
      (i32.add
       (global.get $data_offset)
       (i32.const 203)
      )
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
       (i32.const 0)
       (i32.add
        (global.get $data_offset)
        (i32.const 203)
       )
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
   (block $store_args_in_locals_8
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
   (call $retain
    (local.get $3)
   )
  )
 )
 (func $main (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
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
  (block $do_15 (result i32)
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
     (i32.const 1)
     (i32.add
      (global.get $data_offset)
      (i32.const 4)
     )
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
      (i32.const 0)
      (i32.add
       (global.get $data_offset)
       (i32.const 4)
      )
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
   (local.set $6
    (block $lambda_call_2 (result i32)
     (i32.store
      (global.get $stack_pointer)
      (i32.add
       (global.get $data_offset)
       (i32.const 25)
      )
     )
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.mul
        (i32.const 1)
        (i32.const 4)
       )
      )
     )
     (local.set $5
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 1)
       (global.get $fn:foo.core/foo)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.mul
        (i32.const 1)
        (i32.const 4)
       )
      )
     )
     (local.get $5)
    )
   )
   (block $do_body_result_14 (result i32)
    (local.set $13
     (block $let_12 (result i32)
      (local.set $8
       (block $main#1_5 (result i32)
        (local.set $7
         (call $lalloc_size
          (i32.const 0)
         )
        )
        (call $make_lval_wasm_lambda
         (i32.add
          (global.get $fn_table_offset)
          (i32.const 0)
         )
         (i32.const 1)
         (i32.const 0)
         (local.get $7)
         (i32.const 0)
         (i32.const 0)
        )
       )
      )
      (local.set $9
       (i32.add
        (global.get $data_offset)
        (i32.const 68)
       )
      )
      (block $let_body_result_11 (result i32)
       (local.set $12
        (call $print_fn
         (i32.const 0)
         (call $new_lval_list
          (call $list_cons
           (i32.add
            (global.get $data_offset)
            (i32.const 110)
           )
           (call $list_cons
            (block $lambda_call_6 (result i32)
             (i32.store offset=8
              (global.get $stack_pointer)
              (i32.add
               (global.get $data_offset)
               (i32.const 25)
              )
             )
             (i32.store offset=4
              (global.get $stack_pointer)
              (i32.add
               (global.get $data_offset)
               (i32.const 146)
              )
             )
             (i32.store
              (global.get $stack_pointer)
              (i32.add
               (global.get $data_offset)
               (i32.const 182)
              )
             )
             (global.set $stack_pointer
              (i32.add
               (global.get $stack_pointer)
               (i32.mul
                (i32.const 3)
                (i32.const 4)
               )
              )
             )
             (local.set $10
              (call $f
               (i32.const 0)
               (i32.const 3)
              )
             )
             (global.set $stack_pointer
              (i32.sub
               (global.get $stack_pointer)
               (i32.mul
                (i32.const 3)
                (i32.const 4)
               )
              )
             )
             (local.get $10)
            )
            (call $list_cons
             (block $lambda_call_7 (result i32)
              (i32.store offset=4
               (global.get $stack_pointer)
               (i32.add
                (global.get $data_offset)
                (i32.const 146)
               )
              )
              (i32.store
               (global.get $stack_pointer)
               (i32.add
                (global.get $data_offset)
                (i32.const 182)
               )
              )
              (global.set $stack_pointer
               (i32.add
                (global.get $stack_pointer)
                (i32.mul
                 (i32.const 2)
                 (i32.const 4)
                )
               )
              )
              (call $log_int
               (call $get_wval_type
                (local.get $8)
               )
              )
              (if
               (i32.ne
                (call $get_wval_type
                 (local.get $8)
                )
                (i32.const 25)
               )
               (call $runtime_error
                (i32.const 2)
                (i32.add
                 (global.get $data_offset)
                 (i32.const 202)
                )
               )
               (nop)
              )
              (local.set $11
               (call_indirect (type $i32_i32_=>_i32)
                (i32.load offset=12
                 (local.get $8)
                )
                (i32.const 2)
                (i32.load16_u offset=2
                 (local.get $8)
                )
               )
              )
              (global.set $stack_pointer
               (i32.sub
                (global.get $stack_pointer)
                (i32.mul
                 (i32.const 2)
                 (i32.const 4)
                )
               )
              )
              (local.get $11)
             )
             (call $list_cons
              (local.get $8)
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (call $list_cons
                (global.get $data:foo.core/foo)
                (i32.const 0)
               )
              )
             )
            )
           )
          )
         )
        )
       )
       (block $release_locals_for_let_10
        (call $release
         (local.get $8)
        )
       )
       (local.get $12)
      )
     )
    )
    (block $release_locals_for_do_13
     (call $release
      (local.get $6)
     )
    )
    (local.get $13)
   )
  )
 )
 ;; custom section "symbol_table", size 45
 ;; custom section "deps", size 34
 ;; custom section "data_size", size 3, contents: "333"
 ;; custom section "fn_table_size", size 1, contents: "3"
)
