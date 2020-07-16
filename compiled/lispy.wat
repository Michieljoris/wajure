(module
 (type $none_=>_i32 (func (result i32)))
 (type $i32_i32_=>_i32 (func (param i32 i32) (result i32)))
 (import "env" "__data_end" (global $__data_end i32))
 (import "env" "printf" (func $printf (param i32 i32) (result i32)))
 (memory $0 2 32767)
 (data (global.get $__data_end) "hello, world")
 (export "test" (func $test))
 (export "mem" (memory $0))
 (func $test (result i32)
  (local $0 i32)
  (local $1 i32)
  (block $my-block
   (drop
    (call $printf
     (global.get $__data_end)
     (i32.const 0)
    )
   )
  )
 )
)
