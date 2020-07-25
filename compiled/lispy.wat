(module
 (type $none_=>_i32 (func (result i32)))
 (type $i32_=>_i32 (func (param i32) (result i32)))
 (type $i32_=>_none (func (param i32)))
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (type $i32_i32_=>_none (func (param i32 i32)))
 (import "env" "memory" (memory $0 2 65536))
 (data (global.get $__data_end) "ok\00then\00foo3\00bar\00")
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "printf_" (func $printf_ (param i32 i32) (result i32)))
 (import "env" "log_int" (func $log_int (param i32)))
 (import "env" "log_string" (func $log_string (param i32)))
 (import "env" "log_string_n" (func $log_string_n (param i32 i32)))
 (import "env" "make_lval_num" (func $make_lval_num (param i32) (result i32)))
 (import "env" "make_lval_nil" (func $make_lval_nil (result i32)))
 (import "env" "make_lval_true" (func $make_lval_true (result i32)))
 (import "env" "make_lval_false" (func $make_lval_false (result i32)))
 (import "env" "make_lval_str" (func $make_lval_str (param i32) (result i32)))
 (import "env" "lalloc_size" (func $lalloc_size (param i32) (result i32)))
 (import "env" "_strcpy" (func $_strcpy (param i32 i32) (result i32)))
 (table $0 3 3 funcref)
 (elem (i32.const 0) $test $log_int $printf_)
 (export "test" (func $test))
 (export "mem" (memory $0))
 (func $test (result i32)
  (block $body (result i32)
   (drop
    (call $make_lval_str
     (call $_strcpy
      (call $lalloc_size
       (i32.const 2)
      )
      (i32.add
       (global.get $__data_end)
       (i32.const 0)
      )
     )
    )
   )
   (drop
    (call $make_lval_str
     (call $_strcpy
      (call $lalloc_size
       (i32.const 4)
      )
      (i32.add
       (global.get $__data_end)
       (i32.const 3)
      )
     )
    )
   )
   (call $make_lval_nil)
  )
 )
)
