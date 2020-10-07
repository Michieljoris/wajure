(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32) (result i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $data_offset) "\0f\0f\0f\0f\01\00\00\00\00\00\00\00\00\00\00\00\14\00\00\00\19\ff\01\00\01\00\01\00\00\00\00\00\00\00\00\00\04\00\00\00\01\00\00\00\00\00\00\00\00\00\00\008\00\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\\\00\00\00\02\08\00\00\03\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ffFAIL\00\01\00\00\00\00\00\00\00\00\00\00\00\85\00\00\00\02\t\00\00\00\00\00\00p\00\00\00\00\00\00\00\ff\ff\ff\ffplus\00\01\00\00\00\00\00\00\00\00\00\00\00\ae\00\00\00\00\ff\ff\ff\00\00\00\00\99\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\d2\00\00\00\00\00\00\008\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ee\00\00\00\00\00\00\00\ae\00\00\00\d2\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\n\01\00\00\01\10\00\00\00\00\00\00\00\00\00\00\ee\00\00\00\00\00\00\00Expected:\00\01\00\00\00\00\00\00\00\00\00\00\008\01\00\00\02\t\00\00\00\00\00\00\1e\01\00\00\00\00\00\00\ff\ff\ff\ffGot:\00\01\00\00\00\00\00\00\00\00\00\00\00a\01\00\00\02\t\00\00\00\00\00\00L\01\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\85\01\00\00\02\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a9\01\00\00\19\ff\02\00\00\00\00\00\00\00\00\00\00\00\00\00\99\01\00\00(\00\00\00L\00\00\00u\00\00\00\9e\00\00\00\fa\00\00\00(\01\00\00Q\01\00\00u\01\00\00\c2\00\00\00\de\00\00\00\04\00\00\00\99\01\00\00\bd\01\00\00\08\00\00\00\dd\01\00\00\02\00\00\00\e5\01\00\00\02\00\00\00\18\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $add_fn $sys_+ $f_0)
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
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $sys_+ (param $0 i32) (param $1 i32) (result i32)
  (call $add_fn
   (i32.const 0)
   (local.get $1)
  )
 )
 (func $f_0 (param $0 i32) (result i32)
  (local $1 i32)
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
  (local $35 i32)
  (local $36 i32)
  (local $37 i32)
  (local $38 i32)
  (local $39 i32)
  (local $40 i32)
  (local $41 i32)
  (local $42 i32)
  (block $do_25 (result i32)
   (block $let_24 (result i32)
    (local.set $2
     (block $make_partial_fn_1 (result i32)
      (local.set $1
       (call $lalloc_size
        (i32.const 8)
       )
      )
      (i32.store align=2
       (local.get $1)
       (call $retain
        (i32.add
         (global.get $data_offset)
         (i32.const 56)
        )
       )
      )
      (i32.store offset=4 align=2
       (local.get $1)
       (call $retain
        (i32.add
         (global.get $data_offset)
         (i32.const 56)
        )
       )
      )
      (call $make_lval_wasm_lambda
       (call $get_wval_fn_table_index
        (i32.add
         (global.get $data_offset)
         (i32.const 20)
        )
       )
       (i32.const 1)
       (i32.const 1)
       (i32.const 0)
       (local.get $1)
       (i32.const 2)
      )
     )
    )
    (local.set $33
     (block $let_18 (result i32)
      (local.set $9
       (block $lambda_call_3 (result i32)
        (local.set $3
         (call $lalloc_size
          (i32.const 4)
         )
        )
        (local.set $4
         (i32.add
          (i32.const 1)
          (call $get_wval_partial_count
           (local.get $2)
          )
         )
        )
        (call_indirect (type $i32_i32_=>_none)
         (local.get $2)
         (local.get $4)
         (i32.const 21)
        )
        (block $args_2
         (local.set $5
          (i32.mul
           (call $get_wval_partial_count
            (local.get $2)
           )
           (i32.const 4)
          )
         )
         (if
          (call $get_wval_partial_count
           (local.get $2)
          )
          (memory.copy
           (local.get $3)
           (call $get_wval_partials
            (local.get $2)
           )
           (local.get $5)
          )
          (nop)
         )
         (local.set $6
          (i32.add
           (local.get $3)
           (local.get $5)
          )
         )
         (i32.store align=2
          (local.get $6)
          (i32.add
           (global.get $data_offset)
           (i32.const 56)
          )
         )
        )
        (if
         (call $get_wval_has_rest_arg
          (local.get $2)
         )
         (call $bundle_rest_args
          (i32.add
           (local.get $3)
           (i32.mul
            (i32.const 4)
            (local.tee $7
             (call $get_wval_rest_arg_index
              (local.get $2)
             )
            )
           )
          )
          (i32.sub
           (local.get $4)
           (local.get $7)
          )
         )
         (nop)
        )
        (local.set $8
         (call_indirect (type $i32_i32_i32_i32_=>_i32)
          (call $get_wval_closure
           (local.get $2)
          )
          (call $get_wval_fn_table_index
           (local.get $2)
          )
          (local.get $3)
          (local.get $4)
          (call $get_wval_param_count
           (local.get $2)
          )
         )
        )
        (call $release
         (local.get $3)
        )
        (local.get $8)
       )
      )
      (block $let_body_result_17 (result i32)
       (local.set $32
        (block $if_block_15 (result i32)
         (local.set $31
          (if (result i32)
           (i32.xor
            (i32.eq
             (i32.and
              (i32.load8_u offset=1
               (local.tee $12
                (block $sys_call_4 (result i32)
                 (local.set $11
                  (call $not_eq_fn
                   (i32.const 0)
                   (local.tee $10
                    (call $new_lval_list
                     (call $prefix_list
                      (call $retain
                       (local.get $9)
                      )
                      (call $prefix_list
                       (call $retain
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 92)
                        )
                       )
                       (i32.const 0)
                      )
                     )
                    )
                   )
                  )
                 )
                 (call $release
                  (local.get $10)
                 )
                 (local.get $11)
                )
               )
              )
              (i32.const 253)
             )
             (i32.const 5)
            )
            (i32.const 1)
           )
           (block $do_13 (result i32)
            (local.set $15
             (block $sys_call_5 (result i32)
              (local.set $14
               (call $pr_fn
                (i32.const 0)
                (local.tee $13
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 133)
                    )
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $13)
              )
              (local.get $14)
             )
            )
            (local.set $18
             (block $sys_call_6 (result i32)
              (local.set $17
               (call $pr_fn
                (i32.const 0)
                (local.tee $16
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 266)
                    )
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $16)
              )
              (local.get $17)
             )
            )
            (local.set $21
             (block $sys_call_7 (result i32)
              (local.set $20
               (call $pr_fn
                (i32.const 0)
                (local.tee $19
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 312)
                    )
                   )
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 92)
                     )
                    )
                    (i32.const 0)
                   )
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
            (block $do_body_result_12 (result i32)
             (local.set $30
              (block $sys_call_10 (result i32)
               (local.set $29
                (call $pr_fn
                 (i32.const 0)
                 (local.tee $28
                  (call $new_lval_list
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 353)
                     )
                    )
                    (call $prefix_list
                     (block $lambda_call_9 (result i32)
                      (local.set $22
                       (call $lalloc_size
                        (i32.const 4)
                       )
                      )
                      (local.set $23
                       (i32.add
                        (i32.const 1)
                        (call $get_wval_partial_count
                         (local.get $2)
                        )
                       )
                      )
                      (call_indirect (type $i32_i32_=>_none)
                       (local.get $2)
                       (local.get $23)
                       (i32.const 21)
                      )
                      (block $args_8
                       (local.set $24
                        (i32.mul
                         (call $get_wval_partial_count
                          (local.get $2)
                         )
                         (i32.const 4)
                        )
                       )
                       (if
                        (call $get_wval_partial_count
                         (local.get $2)
                        )
                        (memory.copy
                         (local.get $22)
                         (call $get_wval_partials
                          (local.get $2)
                         )
                         (local.get $24)
                        )
                        (nop)
                       )
                       (local.set $25
                        (i32.add
                         (local.get $22)
                         (local.get $24)
                        )
                       )
                       (i32.store align=2
                        (local.get $25)
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 56)
                        )
                       )
                      )
                      (if
                       (call $get_wval_has_rest_arg
                        (local.get $2)
                       )
                       (call $bundle_rest_args
                        (i32.add
                         (local.get $22)
                         (i32.mul
                          (i32.const 4)
                          (local.tee $26
                           (call $get_wval_rest_arg_index
                            (local.get $2)
                           )
                          )
                         )
                        )
                        (i32.sub
                         (local.get $23)
                         (local.get $26)
                        )
                       )
                       (nop)
                      )
                      (local.set $27
                       (call_indirect (type $i32_i32_i32_i32_=>_i32)
                        (call $get_wval_closure
                         (local.get $2)
                        )
                        (call $get_wval_fn_table_index
                         (local.get $2)
                        )
                        (local.get $22)
                        (local.get $23)
                        (call $get_wval_param_count
                         (local.get $2)
                        )
                       )
                      )
                      (call $release
                       (local.get $22)
                      )
                      (local.get $27)
                     )
                     (i32.const 0)
                    )
                   )
                  )
                 )
                )
               )
               (call $release
                (local.get $28)
               )
               (local.get $29)
              )
             )
             (block $release_locals_for_do_11
              (call $release
               (local.get $15)
              )
              (call $release
               (local.get $18)
              )
              (call $release
               (local.get $21)
              )
             )
             (local.get $30)
            )
           )
           (call $retain
            (i32.add
             (global.get $data_offset)
             (i32.const 389)
            )
           )
          )
         )
         (call $release
          (local.get $12)
         )
         (local.get $31)
        )
       )
       (block $release_locals_for_let_16
        (call $release
         (local.get $9)
        )
       )
       (local.get $32)
      )
     )
    )
    (block $let_body_result_23 (result i32)
     (local.set $42
      (block $sys_call_21 (result i32)
       (local.set $41
        (call $print_fn
         (i32.const 0)
         (local.tee $40
          (call $new_lval_list
           (call $prefix_list
            (block $lambda_call_20 (result i32)
             (local.set $34
              (call $lalloc_size
               (i32.const 4)
              )
             )
             (local.set $35
              (i32.add
               (i32.const 1)
               (call $get_wval_partial_count
                (local.get $2)
               )
              )
             )
             (call_indirect (type $i32_i32_=>_none)
              (local.get $2)
              (local.get $35)
              (i32.const 21)
             )
             (block $args_19
              (local.set $36
               (i32.mul
                (call $get_wval_partial_count
                 (local.get $2)
                )
                (i32.const 4)
               )
              )
              (if
               (call $get_wval_partial_count
                (local.get $2)
               )
               (memory.copy
                (local.get $34)
                (call $get_wval_partials
                 (local.get $2)
                )
                (local.get $36)
               )
               (nop)
              )
              (local.set $37
               (i32.add
                (local.get $34)
                (local.get $36)
               )
              )
              (i32.store align=2
               (local.get $37)
               (i32.add
                (global.get $data_offset)
                (i32.const 56)
               )
              )
             )
             (if
              (call $get_wval_has_rest_arg
               (local.get $2)
              )
              (call $bundle_rest_args
               (i32.add
                (local.get $34)
                (i32.mul
                 (i32.const 4)
                 (local.tee $38
                  (call $get_wval_rest_arg_index
                   (local.get $2)
                  )
                 )
                )
               )
               (i32.sub
                (local.get $35)
                (local.get $38)
               )
              )
              (nop)
             )
             (local.set $39
              (call_indirect (type $i32_i32_i32_i32_=>_i32)
               (call $get_wval_closure
                (local.get $2)
               )
               (call $get_wval_fn_table_index
                (local.get $2)
               )
               (local.get $34)
               (local.get $35)
               (call $get_wval_param_count
                (local.get $2)
               )
              )
             )
             (call $release
              (local.get $34)
             )
             (local.get $39)
            )
            (i32.const 0)
           )
          )
         )
        )
       )
       (call $release
        (local.get $40)
       )
       (local.get $41)
      )
     )
     (block $release_locals_for_let_22
      (call $release
       (local.get $2)
      )
      (call $release
       (local.get $33)
      )
     )
     (local.get $42)
    )
   )
  )
 )
 ;; custom section "symbol_table", size 23
 ;; custom section "deps", size 0, contents: ""
 ;; custom section "data_size", size 3, contents: "521"
 ;; custom section "fn_table_size", size 1, contents: "3"
)
