(module
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_i32_=>_i32 (func (param i32 i32 i32) (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (type $i32_i32_i32_=>_none (func (param i32 i32 i32)))
 (type $none_=>_none (func))
 (type $i32_i32_i32_i32_i32_=>_i32 (func (param i32 i32 i32 i32 i32) (result i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $data_offset) "\0f\0f\0f\0f\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\16\01\00\00\00\00\00\00\00\00\00\00\00)\00\00\00\03\0f\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\04\00\00\00\00++++++++++++++++++++\01\00\00\00\00\00\00\00\00\00\00\00j\00\00\00\02\08\00\00\01\00\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\96\00\00\00\02\08\00\00\03\00\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00FAIL\00\01\00\00\00\00\00\00\00\00\00\00\00\c7\00\00\00\02\t\00\00\b2\00\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00plus\00\01\00\00\00\00\00\00\00\00\00\00\00\f8\00\00\00\00\ff\00\00\e3\00\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00$\01\00\00\00\00\00\00j\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00P\01\00\00\00\00\00\00\f8\00\00\00$\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00|\01\00\00\01\10\00\00P\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00Expected:\00\01\00\00\00\00\00\00\00\00\00\00\00\b2\01\00\00\02\t\00\00\98\01\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00Got:\00\01\00\00\00\00\00\00\00\00\00\00\00\e3\01\00\00\02\t\00\00\ce\01\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\0f\02\00\00\02\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00E\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00;\02\00\00\03\0f\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00E\00\00\00\19\00\00\00Z\00\00\00\86\00\00\00\b7\00\00\00\e8\00\00\00l\01\00\00\a2\01\00\00\d3\01\00\00\ff\01\00\00+\02\00\00\14\01\00\00@\01\00\00W\02\00\00\n\00\00\00\7f\02\00\00\02\00\00\00\10\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $sys_+ $f_0)
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
 (import "env" "make_lval_wasm_lambda" (func $make_lval_wasm_lambda (param i32 i32 i32 i32 i32) (result i32)))
 (import "env" "get_wval_type" (func $get_wval_type (param i32) (result i32)))
 (import "env" "get_wval_subtype" (func $get_wval_subtype (param i32) (result i32)))
 (import "env" "get_wval_fn_table_index" (func $get_wval_fn_table_index (param i32) (result i32)))
 (import "env" "get_wval_closure" (func $get_wval_closure (param i32) (result i32)))
 (import "env" "get_wval_partials" (func $get_wval_partials (param i32) (result i32)))
 (import "env" "get_wval_partial_count" (func $get_wval_partial_count (param i32) (result i32)))
 (import "env" "get_wval_fn_call_relay_array" (func $get_wval_fn_call_relay_array (param i32) (result i32)))
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
 (import "env" "foo_fn" (func $foo_fn (param i32 i32) (result i32)))
 (import "env" "print_fn" (func $print_fn (param i32 i32) (result i32)))
 (import "env" "pr_fn" (func $pr_fn (param i32 i32) (result i32)))
 (import "env" "debug_fn" (func $debug_fn (param i32 i32) (result i32)))
 (import "env" "boolean_fn" (func $boolean_fn (param i32 i32) (result i32)))
 (import "env" "hash_fn" (func $hash_fn (param i32 i32) (result i32)))
 (import "env" "str_fn" (func $str_fn (param i32 i32) (result i32)))
 (import "env" "read_string_fn" (func $read_string_fn (param i32 i32) (result i32)))
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
  (local $43 i32)
  (local $44 i32)
  (local $45 i32)
  (block $do_24 (result i32)
   (block $let_23 (result i32)
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
         (i32.const 106)
        )
       )
      )
      (i32.store offset=4 align=2
       (local.get $1)
       (call $retain
        (i32.add
         (global.get $data_offset)
         (i32.const 106)
        )
       )
      )
      (call $make_lval_wasm_lambda
       (call $get_wval_fn_table_index
        (i32.add
         (global.get $data_offset)
         (i32.const 41)
        )
       )
       (i32.const 0)
       (local.get $1)
       (i32.const 2)
       (call $get_wval_fn_call_relay_array
        (i32.add
         (global.get $data_offset)
         (i32.const 41)
        )
       )
      )
     )
    )
    (local.set $35
     (block $let_17 (result i32)
      (local.set $10
       (block $lambda_call_3 (result i32)
        (local.set $4
         (local.get $2)
        )
        (local.set $3
         (call $lalloc_size
          (i32.mul
           (local.tee $6
            (i32.add
             (i32.const 1)
             (local.tee $5
              (call $get_wval_partial_count
               (local.get $4)
              )
             )
            )
           )
           (i32.const 4)
          )
         )
        )
        (block $args_2
         (local.set $8
          (if (result i32)
           (local.get $5)
           (block (result i32)
            (memory.copy
             (local.get $3)
             (call $get_wval_partials
              (local.get $4)
             )
             (local.tee $7
              (i32.mul
               (local.get $5)
               (i32.const 4)
              )
             )
            )
            (i32.add
             (local.get $3)
             (local.get $7)
            )
           )
           (local.get $3)
          )
         )
         (i32.store align=2
          (local.get $8)
          (i32.add
           (global.get $data_offset)
           (i32.const 106)
          )
         )
        )
        (local.set $9
         (call_indirect (type $i32_i32_i32_=>_i32)
          (local.get $4)
          (local.get $3)
          (if (result i32)
           (i32.gt_u
            (local.get $6)
            (i32.const 20)
           )
           (i32.const 20)
           (local.get $6)
          )
          (i32.load8_u
           (i32.add
            (call $get_wval_fn_call_relay_array
             (local.get $4)
            )
            (local.get $6)
           )
          )
         )
        )
        (call $release
         (local.get $3)
        )
        (local.get $9)
       )
      )
      (block $let_body_result_16 (result i32)
       (local.set $34
        (block $if_block_14 (result i32)
         (local.set $33
          (if (result i32)
           (i32.xor
            (i32.eq
             (i32.and
              (i32.load8_u offset=1
               (local.tee $13
                (block $sys_call_4 (result i32)
                 (local.set $12
                  (call $not_eq_fn
                   (i32.const 0)
                   (local.tee $11
                    (call $new_lval_list
                     (call $prefix_list
                      (call $retain
                       (local.get $10)
                      )
                      (call $prefix_list
                       (call $retain
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 150)
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
                  (local.get $11)
                 )
                 (local.get $12)
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
            (local.set $16
             (block $sys_call_5 (result i32)
              (local.set $15
               (call $pr_fn
                (i32.const 0)
                (local.tee $14
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 199)
                    )
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $14)
              )
              (local.get $15)
             )
            )
            (local.set $19
             (block $sys_call_6 (result i32)
              (local.set $18
               (call $pr_fn
                (i32.const 0)
                (local.tee $17
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 380)
                    )
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $17)
              )
              (local.get $18)
             )
            )
            (local.set $22
             (block $sys_call_7 (result i32)
              (local.set $21
               (call $pr_fn
                (i32.const 0)
                (local.tee $20
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 434)
                    )
                   )
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 150)
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
               (local.get $20)
              )
              (local.get $21)
             )
            )
            (block $do_body_result_12 (result i32)
             (local.set $32
              (block $sys_call_10 (result i32)
               (local.set $31
                (call $pr_fn
                 (i32.const 0)
                 (local.tee $30
                  (call $new_lval_list
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 483)
                     )
                    )
                    (call $prefix_list
                     (block $lambda_call_9 (result i32)
                      (local.set $24
                       (local.get $2)
                      )
                      (local.set $23
                       (call $lalloc_size
                        (i32.mul
                         (local.tee $26
                          (i32.add
                           (i32.const 1)
                           (local.tee $25
                            (call $get_wval_partial_count
                             (local.get $24)
                            )
                           )
                          )
                         )
                         (i32.const 4)
                        )
                       )
                      )
                      (block $args_8
                       (local.set $28
                        (if (result i32)
                         (local.get $25)
                         (block (result i32)
                          (memory.copy
                           (local.get $23)
                           (call $get_wval_partials
                            (local.get $24)
                           )
                           (local.tee $27
                            (i32.mul
                             (local.get $25)
                             (i32.const 4)
                            )
                           )
                          )
                          (i32.add
                           (local.get $23)
                           (local.get $27)
                          )
                         )
                         (local.get $23)
                        )
                       )
                       (i32.store align=2
                        (local.get $28)
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 106)
                        )
                       )
                      )
                      (local.set $29
                       (call_indirect (type $i32_i32_i32_=>_i32)
                        (local.get $24)
                        (local.get $23)
                        (if (result i32)
                         (i32.gt_u
                          (local.get $26)
                          (i32.const 20)
                         )
                         (i32.const 20)
                         (local.get $26)
                        )
                        (i32.load8_u
                         (i32.add
                          (call $get_wval_fn_call_relay_array
                           (local.get $24)
                          )
                          (local.get $26)
                         )
                        )
                       )
                      )
                      (call $release
                       (local.get $23)
                      )
                      (local.get $29)
                     )
                     (i32.const 0)
                    )
                   )
                  )
                 )
                )
               )
               (call $release
                (local.get $30)
               )
               (local.get $31)
              )
             )
             (block $release_locals_for_do_11
              (call $release
               (local.get $16)
              )
              (call $release
               (local.get $19)
              )
              (call $release
               (local.get $22)
              )
             )
             (local.get $32)
            )
           )
           (call $retain
            (i32.add
             (global.get $data_offset)
             (i32.const 527)
            )
           )
          )
         )
         (call $release
          (local.get $13)
         )
         (local.get $33)
        )
       )
       (block $release_locals_for_let_15
        (call $release
         (local.get $10)
        )
       )
       (local.get $34)
      )
     )
    )
    (block $let_body_result_22 (result i32)
     (local.set $45
      (block $sys_call_20 (result i32)
       (local.set $44
        (call $print_fn
         (i32.const 0)
         (local.tee $43
          (call $new_lval_list
           (call $prefix_list
            (block $lambda_call_19 (result i32)
             (local.set $37
              (local.get $2)
             )
             (local.set $36
              (call $lalloc_size
               (i32.mul
                (local.tee $39
                 (i32.add
                  (i32.const 1)
                  (local.tee $38
                   (call $get_wval_partial_count
                    (local.get $37)
                   )
                  )
                 )
                )
                (i32.const 4)
               )
              )
             )
             (block $args_18
              (local.set $41
               (if (result i32)
                (local.get $38)
                (block (result i32)
                 (memory.copy
                  (local.get $36)
                  (call $get_wval_partials
                   (local.get $37)
                  )
                  (local.tee $40
                   (i32.mul
                    (local.get $38)
                    (i32.const 4)
                   )
                  )
                 )
                 (i32.add
                  (local.get $36)
                  (local.get $40)
                 )
                )
                (local.get $36)
               )
              )
              (i32.store align=2
               (local.get $41)
               (i32.add
                (global.get $data_offset)
                (i32.const 106)
               )
              )
             )
             (local.set $42
              (call_indirect (type $i32_i32_i32_=>_i32)
               (local.get $37)
               (local.get $36)
               (if (result i32)
                (i32.gt_u
                 (local.get $39)
                 (i32.const 20)
                )
                (i32.const 20)
                (local.get $39)
               )
               (i32.load8_u
                (i32.add
                 (call $get_wval_fn_call_relay_array
                  (local.get $37)
                 )
                 (local.get $39)
                )
               )
              )
             )
             (call $release
              (local.get $36)
             )
             (local.get $42)
            )
            (i32.const 0)
           )
          )
         )
        )
       )
       (call $release
        (local.get $43)
       )
       (local.get $44)
      )
     )
     (block $release_locals_for_let_21
      (call $release
       (local.get $2)
      )
      (call $release
       (local.get $35)
      )
     )
     (local.get $45)
    )
   )
  )
 )
 ;; custom section "symbol_table", size 23
 ;; custom section "deps", size 0, contents: ""
 ;; custom section "data_size", size 3, contents: "667"
 ;; custom section "fn_table_size", size 1, contents: "2"
)
