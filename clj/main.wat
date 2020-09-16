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
 (data (global.get $data_offset) "\0f\0f\0f\0fmain\00\01\00\00\00\00\00\00\00\00\00\00\00\19\00\00\00\19\ff\00\00\02\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00run-tests\00\01\00\00\00\00\00\00\00\00\00\00\00G\00\00\00\19\ff\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\t\00\00\007\00\00\00[\00\00\00\00\00\00\00[\00\00\00\00\00\00\00[\00\00\00\02\00\00\00\18\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $main $run-tests)
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "stack_pointer" (global $stack_pointer (mut i32)))
 (import "env" "data_offset" (global $data_offset i32))
 (import "env" "fn_table_offset" (global $fn_table_offset i32))
 (import "env" "fn:test-compile.test8/testc" (global $fn:test-compile.test8/testc i32))
 (import "env" "fn:test-compile.test8/testb" (global $fn:test-compile.test8/testb i32))
 (import "env" "fn:test-compile.test8/test" (global $fn:test-compile.test8/test i32))
 (import "env" "fn:test-compile.test7/test7c" (global $fn:test-compile.test7/test7c i32))
 (import "env" "fn:test-compile.test7/test7a" (global $fn:test-compile.test7/test7a i32))
 (import "env" "fn:test-compile.test6-file/test" (global $fn:test-compile.test6-file/test i32))
 (import "env" "fn:test-compile.test4/test4e" (global $fn:test-compile.test4/test4e i32))
 (import "env" "fn:test-compile.test4/test4d" (global $fn:test-compile.test4/test4d i32))
 (import "env" "fn:test-compile.test4/test4c" (global $fn:test-compile.test4/test4c i32))
 (import "env" "fn:test-compile.test4/test4b" (global $fn:test-compile.test4/test4b i32))
 (import "env" "fn:test-compile.test4/test4a" (global $fn:test-compile.test4/test4a i32))
 (import "env" "fn:test-compile.test3-env/test" (global $fn:test-compile.test3-env/test i32))
 (import "env" "fn:test-compile.test2-eval/test" (global $fn:test-compile.test2-eval/test i32))
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
 (import "env" "new_lval_vector" (func $new_lval_vector (param i32) (result i32)))
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
 (export "main" (func $main))
 (export "run-tests" (func $run-tests))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $main (param $0 i32) (param $1 i32) (result i32)
  (local $2 i32)
  (local $3 i32)
  (local $4 i32)
  (local $5 i32)
  (block $do_411 (result i32)
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
     (i32.const 4)
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
      (i32.const 4)
     )
     (nop)
    )
   )
   (block $store_args_in_locals_409
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
   (block $lambda_call_410 (result i32)
    (global.set $stack_pointer
     (i32.add
      (global.get $stack_pointer)
      (i32.const 0)
     )
    )
    (local.set $5
     (call $run-tests
      (i32.const 0)
      (i32.const 0)
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
  )
 )
 (func $run-tests (param $0 i32) (param $1 i32) (result i32)
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
  (block $do_428 (result i32)
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
     (i32.const 1)
     (i32.const 45)
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
      (i32.const 0)
      (i32.const 45)
     )
     (nop)
    )
   )
   (block $store_args_in_locals_412
    (local.set $2
     (i32.sub
      (global.get $stack_pointer)
      (i32.const 0)
     )
    )
   )
   (local.set $4
    (block $lambda_call_413 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $3
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test2-eval/test)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $3)
    )
   )
   (local.set $6
    (block $lambda_call_414 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $5
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test3-env/test)
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
   )
   (local.set $8
    (block $lambda_call_415 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $7
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test4/test4a)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $7)
    )
   )
   (local.set $10
    (block $lambda_call_416 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $9
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test4/test4b)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $9)
    )
   )
   (local.set $12
    (block $lambda_call_417 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $11
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test4/test4c)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $11)
    )
   )
   (local.set $14
    (block $lambda_call_418 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $13
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test4/test4d)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $13)
    )
   )
   (local.set $16
    (block $lambda_call_419 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $15
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test4/test4e)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $15)
    )
   )
   (local.set $18
    (block $lambda_call_420 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $17
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test6-file/test)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $17)
    )
   )
   (local.set $20
    (block $lambda_call_421 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $19
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test7/test7a)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $19)
    )
   )
   (local.set $22
    (block $lambda_call_422 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $21
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test7/test7c)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $21)
    )
   )
   (local.set $24
    (block $lambda_call_423 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $23
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test8/test)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $23)
    )
   )
   (local.set $26
    (block $lambda_call_424 (result i32)
     (global.set $stack_pointer
      (i32.add
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.set $25
      (call_indirect (type $i32_i32_=>_i32)
       (i32.const 0)
       (i32.const 0)
       (global.get $fn:test-compile.test8/testb)
      )
     )
     (global.set $stack_pointer
      (i32.sub
       (global.get $stack_pointer)
       (i32.const 0)
      )
     )
     (local.get $25)
    )
   )
   (block $do_body_result_427 (result i32)
    (local.set $28
     (block $lambda_call_425 (result i32)
      (global.set $stack_pointer
       (i32.add
        (global.get $stack_pointer)
        (i32.const 0)
       )
      )
      (local.set $27
       (call_indirect (type $i32_i32_=>_i32)
        (i32.const 0)
        (i32.const 0)
        (global.get $fn:test-compile.test8/testc)
       )
      )
      (global.set $stack_pointer
       (i32.sub
        (global.get $stack_pointer)
        (i32.const 0)
       )
      )
      (local.get $27)
     )
    )
    (block $release_locals_for_do_426
     (call $release
      (local.get $4)
     )
     (call $release
      (local.get $6)
     )
     (call $release
      (local.get $8)
     )
     (call $release
      (local.get $10)
     )
     (call $release
      (local.get $12)
     )
     (call $release
      (local.get $14)
     )
     (call $release
      (local.get $16)
     )
     (call $release
      (local.get $18)
     )
     (call $release
      (local.get $20)
     )
     (call $release
      (local.get $22)
     )
     (call $release
      (local.get $24)
     )
     (call $release
      (local.get $26)
     )
    )
    (local.get $28)
   )
  )
 )
 ;; custom section "symbol_table", size 51
 ;; custom section "deps", size 381
 ;; custom section "data_size", size 3, contents: "127"
 ;; custom section "fn_table_size", size 1, contents: "2"
)
