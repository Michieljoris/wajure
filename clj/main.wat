(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $data_offset) "\0f\0f\0f\0f\01\00\00\00\00\00\00\00\00\00\00\00\14\00\00\00\03\0f\00\00\00\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00@\00\00\00\02\08\00\00\01\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00@\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00p\00\00\00\03\0f\00\00\00\00\00\00\ff\ff\ff\ff\02\00\01\00\00\00\00\00\\\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\9c\00\00\00\02\08\00\00\02\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c8\00\00\00\03\0f\00\00\00\00\00\00\ff\ff\ff\ff\04\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\04\00\00\000\00\00\00`\00\00\00\8c\00\00\00\b8\00\00\00\e4\00\00\00\05\00\00\00\f8\00\00\00\00\00\00\00\10\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $f_0_a1 $f_0_a2 $f_0 $main_2_a1 $main_2)
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "data_offset" (global $data_offset i32))
 (import "env" "fn_table_offset" (global $fn_table_offset i32))
 (import "env" "fn:foo.core/foo_0_a2" (global $fn:foo.core/foo_0_a2 i32))
 (import "env" "fn:foo.core/foo_0_a1" (global $fn:foo.core/foo_0_a1 i32))
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
 (import "env" "make_lval_wasm_lambda" (func $make_lval_wasm_lambda (param i32 i32 i32 i32) (result i32)))
 (import "env" "get_wval_type" (func $get_wval_type (param i32) (result i32)))
 (import "env" "get_wval_subtype" (func $get_wval_subtype (param i32) (result i32)))
 (import "env" "get_wval_fn_table_index" (func $get_wval_fn_table_index (param i32) (result i32)))
 (import "env" "get_wval_closure" (func $get_wval_closure (param i32) (result i32)))
 (import "env" "get_wval_partials" (func $get_wval_partials (param i32) (result i32)))
 (import "env" "get_wval_partial_count" (func $get_wval_partial_count (param i32) (result i32)))
 (import "env" "listify_args" (func $listify_args (param i32 i32) (result i32)))
 (import "env" "bundle_rest_args" (func $bundle_rest_args (param i32 i32 i32)))
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
 (import "env" "print_fn" (func $print_fn (param i32 i32) (result i32)))
 (import "env" "pr_fn" (func $pr_fn (param i32 i32) (result i32)))
 (import "env" "debug_fn" (func $debug_fn (param i32 i32) (result i32)))
 (import "env" "boolean_fn" (func $boolean_fn (param i32 i32) (result i32)))
 (import "env" "hash_fn" (func $hash_fn (param i32 i32) (result i32)))
 (import "env" "str_fn" (func $str_fn (param i32 i32) (result i32)))
 (import "env" "read_string_fn" (func $read_string_fn (param i32 i32) (result i32)))
 (export "main" (func $main_2))
 (export "mem" (memory $0))
 (func $f_0_a1 (param $0 i32) (param $1 i32) (result i32)
  (block $do_1 (result i32)
   (call $new_lval_vector
    (call $prefix_list
     (call $retain
      (local.get $1)
     )
     (i32.const 0)
    )
   )
  )
 )
 (func $f_0_a2 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (block $do_2 (result i32)
   (call $new_lval_vector
    (call $prefix_list
     (call $retain
      (local.get $1)
     )
     (call $prefix_list
      (call $retain
       (local.get $2)
      )
      (i32.const 0)
     )
    )
   )
  )
 )
 (func $f_0 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (block $rt_error
   (block $case2
    (block $case1
     (br_table $rt_error $case1 $case2 $rt_error
      (local.get $2)
     )
     (nop)
    )
    (return
     (call $f_0_a1
      (i32.const 0)
      (i32.load align=2
       (local.get $1)
      )
     )
    )
   )
   (return
    (call $f_0_a2
     (i32.const 0)
     (i32.load align=2
      (local.get $1)
     )
     (i32.load offset=4 align=2
      (local.get $1)
     )
    )
   )
  )
  (return
   (block (result i32)
    (call $log_int
     (local.get $2)
    )
    (call $runtime_error
     (i32.const 0)
     (i32.const 0)
    )
    (i32.const 0)
   )
  )
 )
 (func $main_2_a1 (param $0 i32) (param $1 i32) (result i32)
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
  (block $do_17 (result i32)
   (local.set $5
    (block $sys_call_4 (result i32)
     (local.set $4
      (call $print_fn
       (i32.const 0)
       (local.tee $3
        (call $new_lval_list
         (call $prefix_list
          (block $lambda_call_3 (result i32)
           (local.set $2
            (call $f_0_a1
             (i32.const 0)
             (i32.add
              (global.get $data_offset)
              (i32.const 64)
             )
            )
           )
           (local.get $2)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (call $release
      (local.get $3)
     )
     (local.get $4)
    )
   )
   (local.set $9
    (block $sys_call_6 (result i32)
     (local.set $8
      (call $print_fn
       (i32.const 0)
       (local.tee $7
        (call $new_lval_list
         (call $prefix_list
          (block $lambda_call_5 (result i32)
           (local.set $6
            (call $f_0_a2
             (i32.const 0)
             (i32.add
              (global.get $data_offset)
              (i32.const 64)
             )
             (i32.add
              (global.get $data_offset)
              (i32.const 156)
             )
            )
           )
           (local.get $6)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (call $release
      (local.get $7)
     )
     (local.get $8)
    )
   )
   (local.set $13
    (block $sys_call_8 (result i32)
     (local.set $12
      (call $print_fn
       (i32.const 0)
       (local.tee $11
        (call $new_lval_list
         (call $prefix_list
          (block $lambda_call_7 (result i32)
           (local.set $10
            (call $f_0_a2
             (i32.const 0)
             (i32.add
              (global.get $data_offset)
              (i32.const 64)
             )
             (i32.add
              (global.get $data_offset)
              (i32.const 156)
             )
            )
           )
           (local.get $10)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (call $release
      (local.get $11)
     )
     (local.get $12)
    )
   )
   (local.set $17
    (block $sys_call_10 (result i32)
     (local.set $16
      (call $print_fn
       (i32.const 0)
       (local.tee $15
        (call $new_lval_list
         (call $prefix_list
          (block $lambda_call_9 (result i32)
           (local.set $14
            (call_indirect (type $i32_i32_=>_i32)
             (i32.const 0)
             (i32.add
              (global.get $data_offset)
              (i32.const 64)
             )
             (global.get $fn:foo.core/foo_0_a1)
            )
           )
           (local.get $14)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (call $release
      (local.get $15)
     )
     (local.get $16)
    )
   )
   (local.set $21
    (block $sys_call_12 (result i32)
     (local.set $20
      (call $print_fn
       (i32.const 0)
       (local.tee $19
        (call $new_lval_list
         (call $prefix_list
          (block $lambda_call_11 (result i32)
           (local.set $18
            (call_indirect (type $i32_i32_i32_=>_i32)
             (i32.const 0)
             (i32.add
              (global.get $data_offset)
              (i32.const 64)
             )
             (i32.add
              (global.get $data_offset)
              (i32.const 156)
             )
             (global.get $fn:foo.core/foo_0_a2)
            )
           )
           (local.get $18)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (call $release
      (local.get $19)
     )
     (local.get $20)
    )
   )
   (block $do_body_result_16 (result i32)
    (local.set $25
     (block $sys_call_14 (result i32)
      (local.set $24
       (call $print_fn
        (i32.const 0)
        (local.tee $23
         (call $new_lval_list
          (call $prefix_list
           (block $lambda_call_13 (result i32)
            (local.set $22
             (call_indirect (type $i32_i32_i32_=>_i32)
              (i32.const 0)
              (i32.add
               (global.get $data_offset)
               (i32.const 64)
              )
              (i32.add
               (global.get $data_offset)
               (i32.const 156)
              )
              (global.get $fn:foo.core/foo_0_a2)
             )
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
    (block $release_locals_for_do_15
     (call $release
      (local.get $5)
     )
     (call $release
      (local.get $9)
     )
     (call $release
      (local.get $13)
     )
     (call $release
      (local.get $17)
     )
     (call $release
      (local.get $21)
     )
    )
    (local.get $25)
   )
  )
 )
 (func $main_2 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (block $rest_arg
   (br_table $rest_arg
    (local.get $2)
   )
   (nop)
  )
  (return
   (call $main_2_a1
    (i32.const 0)
    (call $listify_args
     (i32.add
      (local.get $1)
      (i32.const 0)
     )
     (i32.sub
      (local.get $2)
      (i32.const 0)
     )
    )
   )
  )
 )
 ;; custom section "symbol_table", size 130
 ;; custom section "deps", size 42
 ;; custom section "data_size", size 3, contents: "268"
 ;; custom section "fn_table_size", size 1, contents: "5"
)
