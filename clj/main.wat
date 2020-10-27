(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $data_offset) "\0f\0f\0f\0f\01\00\00\00\00\00\00\00\00\00\00\00\14\00\00\00\02\08\00\00{\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00@\00\00\00\02\08\00\00\01\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\00\00\00\02\08\00\00\02\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\98\00\00\00\02\08\00\00\03\00\00\00\ff\ff\ff\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c4\00\00\00\03\0f\00\00\00\00\00\00\ff\ff\ff\ff\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\04\00\00\000\00\00\00\\\00\00\00\88\00\00\00\b4\00\00\00\e0\00\00\00\05\00\00\00\f4\00\00\00\00\00\00\00\10\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $main_0_a1#0_a1 $main_0_a1#0_a2 $main_0_a1#0_a3 $main_0_a1#0 $main_0_a1 $main_0)
 (import "env" "__data_end" (global $__data_end i32))
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
 (export "main" (func $main_0))
 (export "mem" (memory $0))
 (func $main_0_a1#0_a1 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (local $3 i32)
  (block $do_2 (result i32)
   (block $args_into_locals_1
    (local.set $3
     (i32.load align=2
      (local.get $1)
     )
    )
   )
   (call $new_lval_vector
    (call $prefix_list
     (call $retain
      (local.get $3)
     )
     (call $prefix_list
      (call $retain
       (i32.load align=2
        (local.get $0)
       )
      )
      (i32.const 0)
     )
    )
   )
  )
 )
 (func $main_0_a1#0_a2 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (local $3 i32)
  (local $4 i32)
  (block $do_4 (result i32)
   (block $args_into_locals_3
    (local.set $3
     (i32.load align=2
      (local.get $1)
     )
    )
    (local.set $4
     (i32.load offset=4 align=2
      (local.get $1)
     )
    )
   )
   (call $new_lval_vector
    (call $prefix_list
     (call $retain
      (local.get $3)
     )
     (call $prefix_list
      (call $retain
       (local.get $4)
      )
      (call $prefix_list
       (call $retain
        (i32.load align=2
         (local.get $0)
        )
       )
       (i32.const 0)
      )
     )
    )
   )
  )
 )
 (func $main_0_a1#0_a3 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  (block $do_6 (result i32)
   (block $args_into_locals_5
    (local.set $3
     (i32.load align=2
      (local.get $1)
     )
    )
    (local.set $4
     (i32.load offset=4 align=2
      (local.get $1)
     )
    )
    (local.set $5
     (i32.load offset=8 align=2
      (local.get $1)
     )
    )
   )
   (call $new_lval_vector
    (call $prefix_list
     (call $retain
      (local.get $3)
     )
     (call $prefix_list
      (call $retain
       (local.get $4)
      )
      (call $prefix_list
       (call $retain
        (local.get $5)
       )
       (call $prefix_list
        (call $retain
         (i32.load align=2
          (local.get $0)
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
 (func $main_0_a1#0 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (block $rt_error
   (block $case3
    (block $case2
     (block $case1
      (br_table $rt_error $case1 $case2 $case3 $rt_error
       (local.get $2)
      )
      (nop)
     )
     (return
      (call $main_0_a1#0_a1
       (local.get $0)
       (local.get $1)
       (local.get $2)
      )
     )
    )
    (return
     (call $main_0_a1#0_a2
      (local.get $0)
      (local.get $1)
      (local.get $2)
     )
    )
   )
   (return
    (call $main_0_a1#0_a3
     (local.get $0)
     (local.get $1)
     (local.get $2)
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
 (func $main_0_a1 (param $0 i32) (param $1 i32) (result i32)
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
  (local $26 i32)
  (local $27 i32)
  (local $28 i32)
  (local $29 i32)
  (local $30 i32)
  (local $31 i32)
  (local $32 i32)
  (local $33 i32)
  (local $34 i32)
  (block $do_20 (result i32)
   (block $let_19 (result i32)
    (local.set $2
     (i32.add
      (global.get $data_offset)
      (i32.const 20)
     )
    )
    (local.set $4
     (block $main_0_a1#0_7 (result i32)
      (local.set $3
       (call $lalloc_size
        (i32.const 4)
       )
      )
      (i32.store
       (local.get $3)
       (local.get $2)
      )
      (call $make_lval_wasm_lambda
       (i32.add
        (global.get $fn_table_offset)
        (i32.const 3)
       )
       (local.get $3)
       (i32.const 0)
       (i32.const 0)
      )
     )
    )
    (local.set $14
     (block $sys_call_10 (result i32)
      (local.set $13
       (call $print_fn
        (i32.const 0)
        (local.tee $12
         (call $new_lval_list
          (call $prefix_list
           (block $lambda_call_9 (result i32)
            (local.set $6
             (local.get $4)
            )
            (local.set $5
             (call $lalloc_size
              (i32.mul
               (local.tee $8
                (i32.add
                 (i32.const 1)
                 (local.tee $7
                  (call $get_wval_partial_count
                   (local.get $6)
                  )
                 )
                )
               )
               (i32.const 4)
              )
             )
            )
            (block $args_8
             (local.set $10
              (if (result i32)
               (local.get $7)
               (block (result i32)
                (memory.copy
                 (local.get $5)
                 (call $get_wval_partials
                  (local.get $6)
                 )
                 (local.tee $9
                  (i32.mul
                   (local.get $7)
                   (i32.const 4)
                  )
                 )
                )
                (i32.add
                 (local.get $5)
                 (local.get $9)
                )
               )
               (local.get $5)
              )
             )
             (i32.store align=2
              (local.get $10)
              (i32.add
               (global.get $data_offset)
               (i32.const 64)
              )
             )
            )
            (local.set $11
             (call_indirect (type $i32_i32_i32_=>_i32)
              (call $get_wval_closure
               (local.get $6)
              )
              (local.get $5)
              (local.get $8)
              (call $get_wval_fn_table_index
               (local.get $6)
              )
             )
            )
            (call $release
             (local.get $5)
            )
            (local.get $11)
           )
           (i32.const 0)
          )
         )
        )
       )
      )
      (call $release
       (local.get $12)
      )
      (local.get $13)
     )
    )
    (local.set $24
     (block $sys_call_13 (result i32)
      (local.set $23
       (call $print_fn
        (i32.const 0)
        (local.tee $22
         (call $new_lval_list
          (call $prefix_list
           (block $lambda_call_12 (result i32)
            (local.set $16
             (local.get $4)
            )
            (local.set $15
             (call $lalloc_size
              (i32.mul
               (local.tee $18
                (i32.add
                 (i32.const 2)
                 (local.tee $17
                  (call $get_wval_partial_count
                   (local.get $16)
                  )
                 )
                )
               )
               (i32.const 4)
              )
             )
            )
            (block $args_11
             (local.set $20
              (if (result i32)
               (local.get $17)
               (block (result i32)
                (memory.copy
                 (local.get $15)
                 (call $get_wval_partials
                  (local.get $16)
                 )
                 (local.tee $19
                  (i32.mul
                   (local.get $17)
                   (i32.const 4)
                  )
                 )
                )
                (i32.add
                 (local.get $15)
                 (local.get $19)
                )
               )
               (local.get $15)
              )
             )
             (i32.store align=2
              (local.get $20)
              (i32.add
               (global.get $data_offset)
               (i32.const 64)
              )
             )
             (i32.store offset=4 align=2
              (local.get $20)
              (i32.add
               (global.get $data_offset)
               (i32.const 108)
              )
             )
            )
            (local.set $21
             (call_indirect (type $i32_i32_i32_=>_i32)
              (call $get_wval_closure
               (local.get $16)
              )
              (local.get $15)
              (local.get $18)
              (call $get_wval_fn_table_index
               (local.get $16)
              )
             )
            )
            (call $release
             (local.get $15)
            )
            (local.get $21)
           )
           (i32.const 0)
          )
         )
        )
       )
      )
      (call $release
       (local.get $22)
      )
      (local.get $23)
     )
    )
    (block $let_body_result_18 (result i32)
     (local.set $34
      (block $sys_call_16 (result i32)
       (local.set $33
        (call $print_fn
         (i32.const 0)
         (local.tee $32
          (call $new_lval_list
           (call $prefix_list
            (block $lambda_call_15 (result i32)
             (local.set $26
              (local.get $4)
             )
             (local.set $25
              (call $lalloc_size
               (i32.mul
                (local.tee $28
                 (i32.add
                  (i32.const 3)
                  (local.tee $27
                   (call $get_wval_partial_count
                    (local.get $26)
                   )
                  )
                 )
                )
                (i32.const 4)
               )
              )
             )
             (block $args_14
              (local.set $30
               (if (result i32)
                (local.get $27)
                (block (result i32)
                 (memory.copy
                  (local.get $25)
                  (call $get_wval_partials
                   (local.get $26)
                  )
                  (local.tee $29
                   (i32.mul
                    (local.get $27)
                    (i32.const 4)
                   )
                  )
                 )
                 (i32.add
                  (local.get $25)
                  (local.get $29)
                 )
                )
                (local.get $25)
               )
              )
              (i32.store align=2
               (local.get $30)
               (i32.add
                (global.get $data_offset)
                (i32.const 64)
               )
              )
              (i32.store offset=4 align=2
               (local.get $30)
               (i32.add
                (global.get $data_offset)
                (i32.const 108)
               )
              )
              (i32.store offset=8 align=2
               (local.get $30)
               (i32.add
                (global.get $data_offset)
                (i32.const 152)
               )
              )
             )
             (local.set $31
              (call_indirect (type $i32_i32_i32_=>_i32)
               (call $get_wval_closure
                (local.get $26)
               )
               (local.get $25)
               (local.get $28)
               (call $get_wval_fn_table_index
                (local.get $26)
               )
              )
             )
             (call $release
              (local.get $25)
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
     (block $release_locals_for_let_17
      (call $release
       (local.get $4)
      )
      (call $release
       (local.get $14)
      )
      (call $release
       (local.get $24)
      )
     )
     (local.get $34)
    )
   )
  )
 )
 (func $main_0 (param $0 i32) (param $1 i32) (param $2 i32) (result i32)
  (block $rest_arg
   (br_table $rest_arg
    (local.get $2)
   )
   (nop)
  )
  (return
   (call $main_0_a1
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
 ;; custom section "symbol_table", size 56
 ;; custom section "deps", size 0, contents: ""
 ;; custom section "data_size", size 3, contents: "264"
 ;; custom section "fn_table_size", size 1, contents: "6"
)
