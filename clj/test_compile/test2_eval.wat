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
 (data (global.get $data_offset) "\0f\0f\0f\0ftest2\00\01\00\00\00\00\00\00\00\00\00\00\00\1a\00\00\00\02\t\00\00\00\00\00\00\04\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00>\00\00\00\02\08\00\00\05\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00b\00\00\00\02\08\00\00\02\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\86\00\00\00\02\08\00\00\03\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\aa\00\00\00\02\08\00\00\0b\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff+\00\01\00\00\00\00\00\00\00\00\00\00\00\d0\00\00\00\00\ff\ff\ff\00\00\00\00\be\00\00\00\00\00\00\00\ff\ff\ff\ff*\00\01\00\00\00\00\00\00\00\00\00\00\00\f6\00\00\00\00\ff\ff\ff\00\00\00\00\e4\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\1a\01\00\00\00\00\00\00\86\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1a\01\00\00\00\00\00\00b\00\00\00\1a\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1a\01\00\00\00\00\00\00\f6\00\00\006\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00n\01\00\00\01\10\00\00\00\00\00\00\00\00\00\00R\01\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\92\01\00\00\00\00\00\00n\01\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f4\00\00\00\00\00\00\00>\00\00\00\92\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f4\00\00\00\00\00\00\00\d0\00\00\00\ae\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e6\01\00\00\01\10\00\00\00\00\00\00\00\00\00\00\ca\01\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\n\02\00\00\02\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00.\02\00\00\02\08\00\00\08\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff-\00\01\00\00\00\00\00\00\00\00\00\00\00T\02\00\00\00\ff\ff\ff\00\00\00\00B\02\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00x\02\00\00\00\00\00\00\86\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00x\02\00\00\00\00\00\00b\00\00\00x\02\00\00\01\00\00\00\00\00\00\00\00\00\00\00x\02\00\00\00\00\00\00\f6\00\00\00\94\02\00\00\01\00\00\00\00\00\00\00\00\00\00\00\cc\02\00\00\01\10\00\00\00\00\00\00\00\00\00\00\b0\02\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f0\02\00\00\00\00\00\00\cc\02\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00x\02\00\00\00\00\00\00>\00\00\00\f0\02\00\00\01\00\00\00\00\00\00\00\00\00\00\00x\02\00\00\00\00\00\00\d0\00\00\00\0c\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00D\03\00\00\01\10\00\00\00\00\00\00\00\00\00\00(\03\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00h\03\00\00\00\00\00\00\86\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00h\03\00\00\00\00\00\00D\03\00\00h\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00x\02\00\00\00\00\00\00T\02\00\00\84\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\bc\03\00\00\01\10\00\00\00\00\00\00\00\00\00\00\a0\03\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e0\03\00\00\02\08\00\00\04\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff/\00\01\00\00\00\00\00\00\00\00\00\00\00\06\04\00\00\00\ff\ff\ff\00\00\00\00\f4\03\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00\86\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00b\00\00\00*\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00\f6\00\00\00F\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00~\04\00\00\01\10\00\00\00\00\00\00\00\00\00\00b\04\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a2\04\00\00\00\00\00\00~\04\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00>\00\00\00\a2\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00\d0\00\00\00\be\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f6\04\00\00\01\10\00\00\00\00\00\00\00\00\00\00\da\04\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1a\05\00\00\00\00\00\00\86\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1a\05\00\00\00\00\00\00\f6\04\00\00\1a\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00T\02\00\006\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00n\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00R\05\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\92\05\00\00\00\00\00\00\e0\03\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\92\05\00\00\00\00\00\00n\05\00\00\92\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00*\04\00\00\00\00\00\00\06\04\00\00\ae\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e6\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00\ca\05\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\n\06\00\00\02\08\00\00\03\02\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00.\06\00\00\02\08\00\00W\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00R\06\00\00\02\08\00\007\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00v\06\00\00\02\08\00\00.\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\9a\06\00\00\02\08\00\00\1b\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\be\06\00\00\02\08\00\00\f2\03\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00R\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00.\06\00\00\e2\06\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00\f6\00\00\00\fe\06\00\00\01\00\00\00\00\00\00\00\00\00\00\006\07\00\00\01\10\00\00\00\00\00\00\00\00\00\00\1a\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\07\00\00\00\00\00\006\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00\n\06\00\00Z\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00\d0\00\00\00v\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ae\07\00\00\01\10\00\00\00\00\00\00\00\00\00\00\92\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\d2\07\00\00\00\00\00\00v\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\d2\07\00\00\00\00\00\00\ae\07\00\00\d2\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00T\02\00\00\ee\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00&\08\00\00\01\10\00\00\00\00\00\00\00\00\00\00\n\08\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00J\08\00\00\00\00\00\00\9a\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00J\08\00\00\00\00\00\00&\08\00\00J\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e2\06\00\00\00\00\00\00\06\04\00\00f\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\9e\08\00\00\01\10\00\00\00\00\00\00\00\00\00\00\82\08\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c2\08\00\00\02\08\00\00\fd\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\e6\08\00\00\02\08\00\00\06\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\n\t\00\00\02\08\00\00\ee\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00.\t\00\00\00\00\00\00\e6\08\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00.\t\00\00\00\00\00\00\c2\08\00\00.\t\00\00\01\00\00\00\00\00\00\00\00\00\00\00.\t\00\00\00\00\00\00\f6\00\00\00J\t\00\00\01\00\00\00\00\00\00\00\00\00\00\00\82\t\00\00\01\10\00\00\00\00\00\00\00\00\00\00f\t\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a6\t\00\00\02\08\00\00\03\02\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\ca\t\00\00\02\08\00\00\a9\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\ee\t\00\00\02\08\00\007\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\12\n\00\00\02\08\00\00(\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\006\n\00\00\02\08\00\00\1e\fc\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00\ee\t\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00\ca\t\00\00Z\n\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00\f6\00\00\00v\n\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ae\n\00\00\01\10\00\00\00\00\00\00\00\00\00\00\92\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\d2\n\00\00\00\00\00\00\ae\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00\a6\t\00\00\d2\n\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00\d0\00\00\00\ee\n\00\00\01\00\00\00\00\00\00\00\00\00\00\00&\0b\00\00\01\10\00\00\00\00\00\00\00\00\00\00\n\0b\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00J\0b\00\00\00\00\00\00\12\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00J\0b\00\00\00\00\00\00&\0b\00\00J\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00T\02\00\00f\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\9e\0b\00\00\01\10\00\00\00\00\00\00\00\00\00\00\82\0b\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c2\0b\00\00\00\00\00\00\9a\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c2\0b\00\00\00\00\00\00\9e\0b\00\00\c2\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00Z\n\00\00\00\00\00\00\06\04\00\00\de\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\16\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\fa\0b\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00:\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00^\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\82\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a6\0c\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\ca\0c\00\00\00\00\00\00\86\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ca\0c\00\00\00\00\00\00b\00\00\00\ca\0c\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ca\0c\00\00\00\00\00\00\a6\0c\00\00\e6\0c\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1e\0d\00\00\01\12\00\00\00\00\00\00\00\00\00\00\02\0d\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00B\0d\00\00\00\00\00\00b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00B\0d\00\00\00\00\00\00\a6\0c\00\00B\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00B\0d\00\00\00\00\00\00\d0\00\00\00^\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00\96\0d\00\00\01\10\00\00\00\00\00\00\00\00\00\00z\0d\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ba\0d\00\00\00\00\00\00\96\0d\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00B\0d\00\00\00\00\00\00b\00\00\00\ba\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00B\0d\00\00\00\00\00\00\a6\0c\00\00\d6\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00\0e\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\f2\0d\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\002\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00V\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00z\0e\00\00\19\ff\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\n\00\00\00.\00\00\00R\00\00\00v\00\00\00\9a\00\00\00\c0\00\00\00\e6\00\00\00^\01\00\00\d6\01\00\00\fa\01\00\00\1e\02\00\00D\02\00\00\bc\02\00\004\03\00\00\ac\03\00\00\d0\03\00\00\f6\03\00\00n\04\00\00\e6\04\00\00^\05\00\00\d6\05\00\00\fa\05\00\00\1e\06\00\00B\06\00\00f\06\00\00\8a\06\00\00\ae\06\00\00&\07\00\00\9e\07\00\00\16\08\00\00\8e\08\00\00\b2\08\00\00\d6\08\00\00\fa\08\00\00r\t\00\00\96\t\00\00\ba\t\00\00\de\t\00\00\02\n\00\00&\n\00\00\9e\n\00\00\16\0b\00\00\8e\0b\00\00\06\0c\00\00*\0c\00\00N\0c\00\00r\0c\00\00\96\0c\00\00\0e\0d\00\00\86\0d\00\00\fe\0d\00\00\"\0e\00\00F\0e\00\00\n\01\00\00&\01\00\00B\01\00\00\82\01\00\00\9e\01\00\00\ba\01\00\00h\02\00\00\84\02\00\00\a0\02\00\00\e0\02\00\00\fc\02\00\00\18\03\00\00X\03\00\00t\03\00\00\90\03\00\00\1a\04\00\006\04\00\00R\04\00\00\92\04\00\00\ae\04\00\00\ca\04\00\00\n\05\00\00&\05\00\00B\05\00\00\82\05\00\00\9e\05\00\00\ba\05\00\00\d2\06\00\00\ee\06\00\00\n\07\00\00J\07\00\00f\07\00\00\82\07\00\00\c2\07\00\00\de\07\00\00\fa\07\00\00:\08\00\00V\08\00\00r\08\00\00\1e\t\00\00:\t\00\00V\t\00\00J\n\00\00f\n\00\00\82\n\00\00\c2\n\00\00\de\n\00\00\fa\n\00\00:\0b\00\00V\0b\00\00r\0b\00\00\b2\0b\00\00\ce\0b\00\00\ea\0b\00\00\ba\0c\00\00\d6\0c\00\00\f2\0c\00\002\0d\00\00N\0d\00\00j\0d\00\00\aa\0d\00\00\c6\0d\00\00\e2\0d\00\00j\0e\00\00\8e\0e\00\005\00\00\00b\0f\00\00?\00\00\00^\10\00\00\01\00\00\00\18\00\00\00")
 (import "env" "fn_table" (table $0 100000 1000000 funcref))
 (elem (global.get $fn_table_offset) $test)
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
 (export "test" (func $test))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $test (param $0 i32) (result i32)
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
  (local $46 i32)
  (local $47 i32)
  (local $48 i32)
  (local $49 i32)
  (local $50 i32)
  (local $51 i32)
  (local $52 i32)
  (local $53 i32)
  (local $54 i32)
  (local $55 i32)
  (local $56 i32)
  (local $57 i32)
  (local $58 i32)
  (local $59 i32)
  (local $60 i32)
  (local $61 i32)
  (local $62 i32)
  (local $63 i32)
  (local $64 i32)
  (local $65 i32)
  (local $66 i32)
  (local $67 i32)
  (local $68 i32)
  (local $69 i32)
  (local $70 i32)
  (local $71 i32)
  (local $72 i32)
  (local $73 i32)
  (local $74 i32)
  (local $75 i32)
  (local $76 i32)
  (local $77 i32)
  (local $78 i32)
  (local $79 i32)
  (local $80 i32)
  (local $81 i32)
  (local $82 i32)
  (local $83 i32)
  (local $84 i32)
  (local $85 i32)
  (local $86 i32)
  (local $87 i32)
  (local $88 i32)
  (local $89 i32)
  (local $90 i32)
  (local $91 i32)
  (local $92 i32)
  (local $93 i32)
  (local $94 i32)
  (local $95 i32)
  (local $96 i32)
  (local $97 i32)
  (local $98 i32)
  (local $99 i32)
  (local $100 i32)
  (local $101 i32)
  (local $102 i32)
  (local $103 i32)
  (local $104 i32)
  (local $105 i32)
  (local $106 i32)
  (local $107 i32)
  (local $108 i32)
  (local $109 i32)
  (local $110 i32)
  (local $111 i32)
  (local $112 i32)
  (local $113 i32)
  (local $114 i32)
  (local $115 i32)
  (local $116 i32)
  (local $117 i32)
  (local $118 i32)
  (local $119 i32)
  (local $120 i32)
  (local $121 i32)
  (local $122 i32)
  (local $123 i32)
  (local $124 i32)
  (local $125 i32)
  (local $126 i32)
  (local $127 i32)
  (local $128 i32)
  (local $129 i32)
  (local $130 i32)
  (local $131 i32)
  (local $132 i32)
  (local $133 i32)
  (local $134 i32)
  (local $135 i32)
  (local $136 i32)
  (local $137 i32)
  (local $138 i32)
  (local $139 i32)
  (local $140 i32)
  (local $141 i32)
  (local $142 i32)
  (local $143 i32)
  (local $144 i32)
  (local $145 i32)
  (local $146 i32)
  (local $147 i32)
  (local $148 i32)
  (local $149 i32)
  (local $150 i32)
  (local $151 i32)
  (local $152 i32)
  (local $153 i32)
  (local $154 i32)
  (local $155 i32)
  (local $156 i32)
  (local $157 i32)
  (local $158 i32)
  (local $159 i32)
  (local $160 i32)
  (local $161 i32)
  (local $162 i32)
  (local $163 i32)
  (local $164 i32)
  (local $165 i32)
  (local $166 i32)
  (local $167 i32)
  (local $168 i32)
  (local $169 i32)
  (local $170 i32)
  (local $171 i32)
  (local $172 i32)
  (local $173 i32)
  (local $174 i32)
  (local $175 i32)
  (local $176 i32)
  (local $177 i32)
  (local $178 i32)
  (local $179 i32)
  (local $180 i32)
  (local $181 i32)
  (local $182 i32)
  (local $183 i32)
  (local $184 i32)
  (local $185 i32)
  (local $186 i32)
  (local $187 i32)
  (local $188 i32)
  (local $189 i32)
  (local $190 i32)
  (local $191 i32)
  (local $192 i32)
  (local $193 i32)
  (local $194 i32)
  (local $195 i32)
  (local $196 i32)
  (local $197 i32)
  (local $198 i32)
  (local $199 i32)
  (local $200 i32)
  (local $201 i32)
  (local $202 i32)
  (local $203 i32)
  (local $204 i32)
  (local $205 i32)
  (local $206 i32)
  (local $207 i32)
  (local $208 i32)
  (local $209 i32)
  (local $210 i32)
  (local $211 i32)
  (local $212 i32)
  (local $213 i32)
  (local $214 i32)
  (local $215 i32)
  (local $216 i32)
  (local $217 i32)
  (local $218 i32)
  (local $219 i32)
  (local $220 i32)
  (local $221 i32)
  (local $222 i32)
  (local $223 i32)
  (local $224 i32)
  (local $225 i32)
  (local $226 i32)
  (local $227 i32)
  (local $228 i32)
  (local $229 i32)
  (local $230 i32)
  (local $231 i32)
  (local $232 i32)
  (local $233 i32)
  (local $234 i32)
  (local $235 i32)
  (local $236 i32)
  (local $237 i32)
  (local $238 i32)
  (local $239 i32)
  (local $240 i32)
  (local $241 i32)
  (local $242 i32)
  (local $243 i32)
  (local $244 i32)
  (local $245 i32)
  (local $246 i32)
  (local $247 i32)
  (local $248 i32)
  (local $249 i32)
  (block $do_226 (result i32)
   (local.set $3
    (block $sys_call_70 (result i32)
     (local.set $2
      (call $pr_fn
       (i32.const 0)
       (local.tee $1
        (call $new_lval_list
         (call $prefix_list
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 26)
           )
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (call $release
      (local.get $1)
     )
     (local.get $2)
    )
   )
   (local.set $30
    (block $let_87 (result i32)
     (local.set $8
      (block $sys_call_72 (result i32)
       (local.set $7
        (call $add_fn
         (i32.const 0)
         (local.tee $6
          (call $new_lval_list
           (call $prefix_list
            (call $retain
             (i32.add
              (global.get $data_offset)
              (i32.const 62)
             )
            )
            (call $prefix_list
             (block $sys_call_71 (result i32)
              (local.set $5
               (call $mul_fn
                (i32.const 0)
                (local.tee $4
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 98)
                    )
                   )
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 134)
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
               (local.get $4)
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
       (call $release
        (local.get $6)
       )
       (local.get $7)
      )
     )
     (block $let_body_result_86 (result i32)
      (local.set $29
       (block $if_block_84 (result i32)
        (local.set $28
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $11
               (block $sys_call_73 (result i32)
                (local.set $10
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $9
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $8)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 170)
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
                 (local.get $9)
                )
                (local.get $10)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_82 (result i32)
           (local.set $14
            (block $sys_call_74 (result i32)
             (local.set $13
              (call $pr_fn
               (i32.const 0)
               (local.tee $12
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
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
           (local.set $17
            (block $sys_call_75 (result i32)
             (local.set $16
              (call $pr_fn
               (i32.const 0)
               (local.tee $15
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 486)
                   )
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
           (local.set $20
            (block $sys_call_76 (result i32)
             (local.set $19
              (call $pr_fn
               (i32.const 0)
               (local.tee $18
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 170)
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
              (local.get $18)
             )
             (local.get $19)
            )
           )
           (block $do_body_result_81 (result i32)
            (local.set $27
             (block $sys_call_79 (result i32)
              (local.set $26
               (call $pr_fn
                (i32.const 0)
                (local.tee $25
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (block $sys_call_78 (result i32)
                     (local.set $24
                      (call $add_fn
                       (i32.const 0)
                       (local.tee $23
                        (call $new_lval_list
                         (call $prefix_list
                          (call $retain
                           (i32.add
                            (global.get $data_offset)
                            (i32.const 62)
                           )
                          )
                          (call $prefix_list
                           (block $sys_call_77 (result i32)
                            (local.set $22
                             (call $mul_fn
                              (i32.const 0)
                              (local.tee $21
                               (call $new_lval_list
                                (call $prefix_list
                                 (call $retain
                                  (i32.add
                                   (global.get $data_offset)
                                   (i32.const 98)
                                  )
                                 )
                                 (call $prefix_list
                                  (call $retain
                                   (i32.add
                                    (global.get $data_offset)
                                    (i32.const 134)
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
                             (local.get $21)
                            )
                            (local.get $22)
                           )
                           (i32.const 0)
                          )
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
                    (i32.const 0)
                   )
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $25)
              )
              (local.get $26)
             )
            )
            (block $release_locals_for_do_80
             (call $release
              (local.get $14)
             )
             (call $release
              (local.get $17)
             )
             (call $release
              (local.get $20)
             )
            )
            (local.get $27)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $11)
        )
        (local.get $28)
       )
      )
      (block $release_locals_for_let_85
       (call $release
        (local.get $8)
       )
      )
      (local.get $29)
     )
    )
   )
   (local.set $61
    (block $let_106 (result i32)
     (local.set $37
      (block $sys_call_90 (result i32)
       (local.set $36
        (call $sub_fn
         (i32.const 0)
         (local.tee $35
          (call $new_lval_list
           (call $prefix_list
            (block $sys_call_89 (result i32)
             (local.set $34
              (call $add_fn
               (i32.const 0)
               (local.tee $33
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 62)
                   )
                  )
                  (call $prefix_list
                   (block $sys_call_88 (result i32)
                    (local.set $32
                     (call $mul_fn
                      (i32.const 0)
                      (local.tee $31
                       (call $new_lval_list
                        (call $prefix_list
                         (call $retain
                          (i32.add
                           (global.get $data_offset)
                           (i32.const 98)
                          )
                         )
                         (call $prefix_list
                          (call $retain
                           (i32.add
                            (global.get $data_offset)
                            (i32.const 134)
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
                     (local.get $31)
                    )
                    (local.get $32)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (call $release
              (local.get $33)
             )
             (local.get $34)
            )
            (call $prefix_list
             (call $retain
              (i32.add
               (global.get $data_offset)
               (i32.const 134)
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
        (local.get $35)
       )
       (local.get $36)
      )
     )
     (block $let_body_result_105 (result i32)
      (local.set $60
       (block $if_block_103 (result i32)
        (local.set $59
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $40
               (block $sys_call_91 (result i32)
                (local.set $39
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $38
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $37)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 558)
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
                 (local.get $38)
                )
                (local.get $39)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_101 (result i32)
           (local.set $43
            (block $sys_call_92 (result i32)
             (local.set $42
              (call $pr_fn
               (i32.const 0)
               (local.tee $41
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $41)
             )
             (local.get $42)
            )
           )
           (local.set $46
            (block $sys_call_93 (result i32)
             (local.set $45
              (call $pr_fn
               (i32.const 0)
               (local.tee $44
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 956)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $44)
             )
             (local.get $45)
            )
           )
           (local.set $49
            (block $sys_call_94 (result i32)
             (local.set $48
              (call $pr_fn
               (i32.const 0)
               (local.tee $47
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 558)
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
              (local.get $47)
             )
             (local.get $48)
            )
           )
           (block $do_body_result_100 (result i32)
            (local.set $58
             (block $sys_call_98 (result i32)
              (local.set $57
               (call $pr_fn
                (i32.const 0)
                (local.tee $56
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (block $sys_call_97 (result i32)
                     (local.set $55
                      (call $sub_fn
                       (i32.const 0)
                       (local.tee $54
                        (call $new_lval_list
                         (call $prefix_list
                          (block $sys_call_96 (result i32)
                           (local.set $53
                            (call $add_fn
                             (i32.const 0)
                             (local.tee $52
                              (call $new_lval_list
                               (call $prefix_list
                                (call $retain
                                 (i32.add
                                  (global.get $data_offset)
                                  (i32.const 62)
                                 )
                                )
                                (call $prefix_list
                                 (block $sys_call_95 (result i32)
                                  (local.set $51
                                   (call $mul_fn
                                    (i32.const 0)
                                    (local.tee $50
                                     (call $new_lval_list
                                      (call $prefix_list
                                       (call $retain
                                        (i32.add
                                         (global.get $data_offset)
                                         (i32.const 98)
                                        )
                                       )
                                       (call $prefix_list
                                        (call $retain
                                         (i32.add
                                          (global.get $data_offset)
                                          (i32.const 134)
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
                                   (local.get $50)
                                  )
                                  (local.get $51)
                                 )
                                 (i32.const 0)
                                )
                               )
                              )
                             )
                            )
                           )
                           (call $release
                            (local.get $52)
                           )
                           (local.get $53)
                          )
                          (call $prefix_list
                           (call $retain
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 134)
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
                      (local.get $54)
                     )
                     (local.get $55)
                    )
                    (i32.const 0)
                   )
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $56)
              )
              (local.get $57)
             )
            )
            (block $release_locals_for_do_99
             (call $release
              (local.get $43)
             )
             (call $release
              (local.get $46)
             )
             (call $release
              (local.get $49)
             )
            )
            (local.get $58)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $40)
        )
        (local.get $59)
       )
      )
      (block $release_locals_for_let_104
       (call $release
        (local.get $37)
       )
      )
      (local.get $60)
     )
    )
   )
   (local.set $96
    (block $let_127 (result i32)
     (local.set $70
      (block $sys_call_110 (result i32)
       (local.set $69
        (call $div_fn
         (i32.const 0)
         (local.tee $68
          (call $new_lval_list
           (call $prefix_list
            (block $sys_call_109 (result i32)
             (local.set $67
              (call $sub_fn
               (i32.const 0)
               (local.tee $66
                (call $new_lval_list
                 (call $prefix_list
                  (block $sys_call_108 (result i32)
                   (local.set $65
                    (call $add_fn
                     (i32.const 0)
                     (local.tee $64
                      (call $new_lval_list
                       (call $prefix_list
                        (call $retain
                         (i32.add
                          (global.get $data_offset)
                          (i32.const 62)
                         )
                        )
                        (call $prefix_list
                         (block $sys_call_107 (result i32)
                          (local.set $63
                           (call $mul_fn
                            (i32.const 0)
                            (local.tee $62
                             (call $new_lval_list
                              (call $prefix_list
                               (call $retain
                                (i32.add
                                 (global.get $data_offset)
                                 (i32.const 98)
                                )
                               )
                               (call $prefix_list
                                (call $retain
                                 (i32.add
                                  (global.get $data_offset)
                                  (i32.const 134)
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
                           (local.get $62)
                          )
                          (local.get $63)
                         )
                         (i32.const 0)
                        )
                       )
                      )
                     )
                    )
                   )
                   (call $release
                    (local.get $64)
                   )
                   (local.get $65)
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 134)
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
              (local.get $66)
             )
             (local.get $67)
            )
            (call $prefix_list
             (call $retain
              (i32.add
               (global.get $data_offset)
               (i32.const 992)
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
        (local.get $68)
       )
       (local.get $69)
      )
     )
     (block $let_body_result_126 (result i32)
      (local.set $95
       (block $if_block_124 (result i32)
        (local.set $94
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $73
               (block $sys_call_111 (result i32)
                (local.set $72
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $71
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $70)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 98)
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
                 (local.get $71)
                )
                (local.get $72)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_122 (result i32)
           (local.set $76
            (block $sys_call_112 (result i32)
             (local.set $75
              (call $pr_fn
               (i32.const 0)
               (local.tee $74
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $74)
             )
             (local.get $75)
            )
           )
           (local.set $79
            (block $sys_call_113 (result i32)
             (local.set $78
              (call $pr_fn
               (i32.const 0)
               (local.tee $77
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 1510)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $77)
             )
             (local.get $78)
            )
           )
           (local.set $82
            (block $sys_call_114 (result i32)
             (local.set $81
              (call $pr_fn
               (i32.const 0)
               (local.tee $80
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 98)
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
              (local.get $80)
             )
             (local.get $81)
            )
           )
           (block $do_body_result_121 (result i32)
            (local.set $93
             (block $sys_call_119 (result i32)
              (local.set $92
               (call $pr_fn
                (i32.const 0)
                (local.tee $91
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (block $sys_call_118 (result i32)
                     (local.set $90
                      (call $div_fn
                       (i32.const 0)
                       (local.tee $89
                        (call $new_lval_list
                         (call $prefix_list
                          (block $sys_call_117 (result i32)
                           (local.set $88
                            (call $sub_fn
                             (i32.const 0)
                             (local.tee $87
                              (call $new_lval_list
                               (call $prefix_list
                                (block $sys_call_116 (result i32)
                                 (local.set $86
                                  (call $add_fn
                                   (i32.const 0)
                                   (local.tee $85
                                    (call $new_lval_list
                                     (call $prefix_list
                                      (call $retain
                                       (i32.add
                                        (global.get $data_offset)
                                        (i32.const 62)
                                       )
                                      )
                                      (call $prefix_list
                                       (block $sys_call_115 (result i32)
                                        (local.set $84
                                         (call $mul_fn
                                          (i32.const 0)
                                          (local.tee $83
                                           (call $new_lval_list
                                            (call $prefix_list
                                             (call $retain
                                              (i32.add
                                               (global.get $data_offset)
                                               (i32.const 98)
                                              )
                                             )
                                             (call $prefix_list
                                              (call $retain
                                               (i32.add
                                                (global.get $data_offset)
                                                (i32.const 134)
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
                                         (local.get $83)
                                        )
                                        (local.get $84)
                                       )
                                       (i32.const 0)
                                      )
                                     )
                                    )
                                   )
                                  )
                                 )
                                 (call $release
                                  (local.get $85)
                                 )
                                 (local.get $86)
                                )
                                (call $prefix_list
                                 (call $retain
                                  (i32.add
                                   (global.get $data_offset)
                                   (i32.const 134)
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
                            (local.get $87)
                           )
                           (local.get $88)
                          )
                          (call $prefix_list
                           (call $retain
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 992)
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
                      (local.get $89)
                     )
                     (local.get $90)
                    )
                    (i32.const 0)
                   )
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $91)
              )
              (local.get $92)
             )
            )
            (block $release_locals_for_do_120
             (call $release
              (local.get $76)
             )
             (call $release
              (local.get $79)
             )
             (call $release
              (local.get $82)
             )
            )
            (local.get $93)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $73)
        )
        (local.get $94)
       )
      )
      (block $release_locals_for_let_125
       (call $release
        (local.get $70)
       )
      )
      (local.get $95)
     )
    )
   )
   (local.set $131
    (block $let_148 (result i32)
     (local.set $105
      (block $sys_call_131 (result i32)
       (local.set $104
        (call $div_fn
         (i32.const 0)
         (local.tee $103
          (call $new_lval_list
           (call $prefix_list
            (block $sys_call_130 (result i32)
             (local.set $102
              (call $sub_fn
               (i32.const 0)
               (local.tee $101
                (call $new_lval_list
                 (call $prefix_list
                  (block $sys_call_129 (result i32)
                   (local.set $100
                    (call $add_fn
                     (i32.const 0)
                     (local.tee $99
                      (call $new_lval_list
                       (call $prefix_list
                        (call $retain
                         (i32.add
                          (global.get $data_offset)
                          (i32.const 1546)
                         )
                        )
                        (call $prefix_list
                         (block $sys_call_128 (result i32)
                          (local.set $98
                           (call $mul_fn
                            (i32.const 0)
                            (local.tee $97
                             (call $new_lval_list
                              (call $prefix_list
                               (call $retain
                                (i32.add
                                 (global.get $data_offset)
                                 (i32.const 1582)
                                )
                               )
                               (call $prefix_list
                                (call $retain
                                 (i32.add
                                  (global.get $data_offset)
                                  (i32.const 1618)
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
                           (local.get $97)
                          )
                          (local.get $98)
                         )
                         (i32.const 0)
                        )
                       )
                      )
                     )
                    )
                   )
                   (call $release
                    (local.get $99)
                   )
                   (local.get $100)
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 1654)
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
              (local.get $101)
             )
             (local.get $102)
            )
            (call $prefix_list
             (call $retain
              (i32.add
               (global.get $data_offset)
               (i32.const 1690)
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
        (local.get $103)
       )
       (local.get $104)
      )
     )
     (block $let_body_result_147 (result i32)
      (local.set $130
       (block $if_block_145 (result i32)
        (local.set $129
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $108
               (block $sys_call_132 (result i32)
                (local.set $107
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $106
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $105)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 1726)
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
                 (local.get $106)
                )
                (local.get $107)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_143 (result i32)
           (local.set $111
            (block $sys_call_133 (result i32)
             (local.set $110
              (call $pr_fn
               (i32.const 0)
               (local.tee $109
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $109)
             )
             (local.get $110)
            )
           )
           (local.set $114
            (block $sys_call_134 (result i32)
             (local.set $113
              (call $pr_fn
               (i32.const 0)
               (local.tee $112
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 2206)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $112)
             )
             (local.get $113)
            )
           )
           (local.set $117
            (block $sys_call_135 (result i32)
             (local.set $116
              (call $pr_fn
               (i32.const 0)
               (local.tee $115
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 1726)
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
              (local.get $115)
             )
             (local.get $116)
            )
           )
           (block $do_body_result_142 (result i32)
            (local.set $128
             (block $sys_call_140 (result i32)
              (local.set $127
               (call $pr_fn
                (i32.const 0)
                (local.tee $126
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (block $sys_call_139 (result i32)
                     (local.set $125
                      (call $div_fn
                       (i32.const 0)
                       (local.tee $124
                        (call $new_lval_list
                         (call $prefix_list
                          (block $sys_call_138 (result i32)
                           (local.set $123
                            (call $sub_fn
                             (i32.const 0)
                             (local.tee $122
                              (call $new_lval_list
                               (call $prefix_list
                                (block $sys_call_137 (result i32)
                                 (local.set $121
                                  (call $add_fn
                                   (i32.const 0)
                                   (local.tee $120
                                    (call $new_lval_list
                                     (call $prefix_list
                                      (call $retain
                                       (i32.add
                                        (global.get $data_offset)
                                        (i32.const 1546)
                                       )
                                      )
                                      (call $prefix_list
                                       (block $sys_call_136 (result i32)
                                        (local.set $119
                                         (call $mul_fn
                                          (i32.const 0)
                                          (local.tee $118
                                           (call $new_lval_list
                                            (call $prefix_list
                                             (call $retain
                                              (i32.add
                                               (global.get $data_offset)
                                               (i32.const 1582)
                                              )
                                             )
                                             (call $prefix_list
                                              (call $retain
                                               (i32.add
                                                (global.get $data_offset)
                                                (i32.const 1618)
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
                                         (local.get $118)
                                        )
                                        (local.get $119)
                                       )
                                       (i32.const 0)
                                      )
                                     )
                                    )
                                   )
                                  )
                                 )
                                 (call $release
                                  (local.get $120)
                                 )
                                 (local.get $121)
                                )
                                (call $prefix_list
                                 (call $retain
                                  (i32.add
                                   (global.get $data_offset)
                                   (i32.const 1654)
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
                            (local.get $122)
                           )
                           (local.get $123)
                          )
                          (call $prefix_list
                           (call $retain
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 1690)
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
                      (local.get $124)
                     )
                     (local.get $125)
                    )
                    (i32.const 0)
                   )
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $126)
              )
              (local.get $127)
             )
            )
            (block $release_locals_for_do_141
             (call $release
              (local.get $111)
             )
             (call $release
              (local.get $114)
             )
             (call $release
              (local.get $117)
             )
            )
            (local.get $128)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $108)
        )
        (local.get $129)
       )
      )
      (block $release_locals_for_let_146
       (call $release
        (local.get $105)
       )
      )
      (local.get $130)
     )
    )
   )
   (local.set $154
    (block $let_163 (result i32)
     (local.set $134
      (block $sys_call_149 (result i32)
       (local.set $133
        (call $mul_fn
         (i32.const 0)
         (local.tee $132
          (call $new_lval_list
           (call $prefix_list
            (call $retain
             (i32.add
              (global.get $data_offset)
              (i32.const 2242)
             )
            )
            (call $prefix_list
             (call $retain
              (i32.add
               (global.get $data_offset)
               (i32.const 2278)
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
        (local.get $132)
       )
       (local.get $133)
      )
     )
     (block $let_body_result_162 (result i32)
      (local.set $153
       (block $if_block_160 (result i32)
        (local.set $152
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $137
               (block $sys_call_150 (result i32)
                (local.set $136
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $135
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $134)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 2314)
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
                 (local.get $135)
                )
                (local.get $136)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_158 (result i32)
           (local.set $140
            (block $sys_call_151 (result i32)
             (local.set $139
              (call $pr_fn
               (i32.const 0)
               (local.tee $138
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $138)
             )
             (local.get $139)
            )
           )
           (local.set $143
            (block $sys_call_152 (result i32)
             (local.set $142
              (call $pr_fn
               (i32.const 0)
               (local.tee $141
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 2434)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $141)
             )
             (local.get $142)
            )
           )
           (local.set $146
            (block $sys_call_153 (result i32)
             (local.set $145
              (call $pr_fn
               (i32.const 0)
               (local.tee $144
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2314)
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
              (local.get $144)
             )
             (local.get $145)
            )
           )
           (block $do_body_result_157 (result i32)
            (local.set $151
             (block $sys_call_155 (result i32)
              (local.set $150
               (call $pr_fn
                (i32.const 0)
                (local.tee $149
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (block $sys_call_154 (result i32)
                     (local.set $148
                      (call $mul_fn
                       (i32.const 0)
                       (local.tee $147
                        (call $new_lval_list
                         (call $prefix_list
                          (call $retain
                           (i32.add
                            (global.get $data_offset)
                            (i32.const 2242)
                           )
                          )
                          (call $prefix_list
                           (call $retain
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 2278)
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
                      (local.get $147)
                     )
                     (local.get $148)
                    )
                    (i32.const 0)
                   )
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $149)
              )
              (local.get $150)
             )
            )
            (block $release_locals_for_do_156
             (call $release
              (local.get $140)
             )
             (call $release
              (local.get $143)
             )
             (call $release
              (local.get $146)
             )
            )
            (local.get $151)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $137)
        )
        (local.get $152)
       )
      )
      (block $release_locals_for_let_161
       (call $release
        (local.get $134)
       )
      )
      (local.get $153)
     )
    )
   )
   (local.set $189
    (block $let_184 (result i32)
     (local.set $163
      (block $sys_call_167 (result i32)
       (local.set $162
        (call $div_fn
         (i32.const 0)
         (local.tee $161
          (call $new_lval_list
           (call $prefix_list
            (block $sys_call_166 (result i32)
             (local.set $160
              (call $sub_fn
               (i32.const 0)
               (local.tee $159
                (call $new_lval_list
                 (call $prefix_list
                  (block $sys_call_165 (result i32)
                   (local.set $158
                    (call $add_fn
                     (i32.const 0)
                     (local.tee $157
                      (call $new_lval_list
                       (call $prefix_list
                        (call $retain
                         (i32.add
                          (global.get $data_offset)
                          (i32.const 2470)
                         )
                        )
                        (call $prefix_list
                         (block $sys_call_164 (result i32)
                          (local.set $156
                           (call $mul_fn
                            (i32.const 0)
                            (local.tee $155
                             (call $new_lval_list
                              (call $prefix_list
                               (call $retain
                                (i32.add
                                 (global.get $data_offset)
                                 (i32.const 2506)
                                )
                               )
                               (call $prefix_list
                                (call $retain
                                 (i32.add
                                  (global.get $data_offset)
                                  (i32.const 2542)
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
                           (local.get $155)
                          )
                          (local.get $156)
                         )
                         (i32.const 0)
                        )
                       )
                      )
                     )
                    )
                   )
                   (call $release
                    (local.get $157)
                   )
                   (local.get $158)
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2578)
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
              (local.get $159)
             )
             (local.get $160)
            )
            (call $prefix_list
             (call $retain
              (i32.add
               (global.get $data_offset)
               (i32.const 1690)
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
        (local.get $161)
       )
       (local.get $162)
      )
     )
     (block $let_body_result_183 (result i32)
      (local.set $188
       (block $if_block_181 (result i32)
        (local.set $187
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $166
               (block $sys_call_168 (result i32)
                (local.set $165
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $164
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $163)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 2614)
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
                 (local.get $164)
                )
                (local.get $165)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_179 (result i32)
           (local.set $169
            (block $sys_call_169 (result i32)
             (local.set $168
              (call $pr_fn
               (i32.const 0)
               (local.tee $167
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $167)
             )
             (local.get $168)
            )
           )
           (local.set $172
            (block $sys_call_170 (result i32)
             (local.set $171
              (call $pr_fn
               (i32.const 0)
               (local.tee $170
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 3094)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $170)
             )
             (local.get $171)
            )
           )
           (local.set $175
            (block $sys_call_171 (result i32)
             (local.set $174
              (call $pr_fn
               (i32.const 0)
               (local.tee $173
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2614)
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
              (local.get $173)
             )
             (local.get $174)
            )
           )
           (block $do_body_result_178 (result i32)
            (local.set $186
             (block $sys_call_176 (result i32)
              (local.set $185
               (call $pr_fn
                (i32.const 0)
                (local.tee $184
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (block $sys_call_175 (result i32)
                     (local.set $183
                      (call $div_fn
                       (i32.const 0)
                       (local.tee $182
                        (call $new_lval_list
                         (call $prefix_list
                          (block $sys_call_174 (result i32)
                           (local.set $181
                            (call $sub_fn
                             (i32.const 0)
                             (local.tee $180
                              (call $new_lval_list
                               (call $prefix_list
                                (block $sys_call_173 (result i32)
                                 (local.set $179
                                  (call $add_fn
                                   (i32.const 0)
                                   (local.tee $178
                                    (call $new_lval_list
                                     (call $prefix_list
                                      (call $retain
                                       (i32.add
                                        (global.get $data_offset)
                                        (i32.const 2470)
                                       )
                                      )
                                      (call $prefix_list
                                       (block $sys_call_172 (result i32)
                                        (local.set $177
                                         (call $mul_fn
                                          (i32.const 0)
                                          (local.tee $176
                                           (call $new_lval_list
                                            (call $prefix_list
                                             (call $retain
                                              (i32.add
                                               (global.get $data_offset)
                                               (i32.const 2506)
                                              )
                                             )
                                             (call $prefix_list
                                              (call $retain
                                               (i32.add
                                                (global.get $data_offset)
                                                (i32.const 2542)
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
                                         (local.get $176)
                                        )
                                        (local.get $177)
                                       )
                                       (i32.const 0)
                                      )
                                     )
                                    )
                                   )
                                  )
                                 )
                                 (call $release
                                  (local.get $178)
                                 )
                                 (local.get $179)
                                )
                                (call $prefix_list
                                 (call $retain
                                  (i32.add
                                   (global.get $data_offset)
                                   (i32.const 2578)
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
                            (local.get $180)
                           )
                           (local.get $181)
                          )
                          (call $prefix_list
                           (call $retain
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 1690)
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
                      (local.get $182)
                     )
                     (local.get $183)
                    )
                    (i32.const 0)
                   )
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $184)
              )
              (local.get $185)
             )
            )
            (block $release_locals_for_do_177
             (call $release
              (local.get $169)
             )
             (call $release
              (local.get $172)
             )
             (call $release
              (local.get $175)
             )
            )
            (local.get $186)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $166)
        )
        (local.get $187)
       )
      )
      (block $release_locals_for_let_182
       (call $release
        (local.get $163)
       )
      )
      (local.get $188)
     )
    )
   )
   (local.set $207
    (block $let_195 (result i32)
     (local.set $190
      (i32.add
       (global.get $data_offset)
       (i32.const 3130)
      )
     )
     (block $if_block_194 (result i32)
      (local.set $206
       (if (result i32)
        (i32.xor
         (i32.eq
          (i32.and
           (i32.load8_u offset=1
            (local.tee $193
             (block $sys_call_185 (result i32)
              (local.set $192
               (call $not_eq_fn
                (i32.const 0)
                (local.tee $191
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (local.get $190)
                   )
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 3166)
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
               (local.get $191)
              )
              (local.get $192)
             )
            )
           )
           (i32.const 253)
          )
          (i32.const 5)
         )
         (i32.const 1)
        )
        (block $do_192 (result i32)
         (local.set $196
          (block $sys_call_186 (result i32)
           (local.set $195
            (call $pr_fn
             (i32.const 0)
             (local.tee $194
              (call $new_lval_list
               (call $prefix_list
                (call $retain
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 103)
                 )
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (call $release
            (local.get $194)
           )
           (local.get $195)
          )
         )
         (local.set $199
          (block $sys_call_187 (result i32)
           (local.set $198
            (call $pr_fn
             (i32.const 0)
             (local.tee $197
              (call $new_lval_list
               (call $prefix_list
                (call $retain
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 3202)
                 )
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (call $release
            (local.get $197)
           )
           (local.get $198)
          )
         )
         (local.set $202
          (block $sys_call_188 (result i32)
           (local.set $201
            (call $pr_fn
             (i32.const 0)
             (local.tee $200
              (call $new_lval_list
               (call $prefix_list
                (call $retain
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 210)
                 )
                )
                (call $prefix_list
                 (call $retain
                  (i32.add
                   (global.get $data_offset)
                   (i32.const 3166)
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
            (local.get $200)
           )
           (local.get $201)
          )
         )
         (block $do_body_result_191 (result i32)
          (local.set $205
           (block $sys_call_189 (result i32)
            (local.set $204
             (call $pr_fn
              (i32.const 0)
              (local.tee $203
               (call $new_lval_list
                (call $prefix_list
                 (call $retain
                  (i32.add
                   (global.get $data_offset)
                   (i32.const 251)
                  )
                 )
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 3130)
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
             (local.get $203)
            )
            (local.get $204)
           )
          )
          (block $release_locals_for_do_190
           (call $release
            (local.get $196)
           )
           (call $release
            (local.get $199)
           )
           (call $release
            (local.get $202)
           )
          )
          (local.get $205)
         )
        )
        (call $retain
         (i32.add
          (global.get $data_offset)
          (i32.const 522)
         )
        )
       )
      )
      (call $release
       (local.get $193)
      )
      (local.get $206)
     )
    )
   )
   (local.set $230
    (block $let_210 (result i32)
     (local.set $210
      (call $new_lval_vector
       (call $prefix_list
        (call $retain
         (i32.add
          (global.get $data_offset)
          (i32.const 3238)
         )
        )
        (call $prefix_list
         (call $retain
          (i32.add
           (global.get $data_offset)
           (i32.const 98)
          )
         )
         (call $prefix_list
          (block $sys_call_196 (result i32)
           (local.set $209
            (call $add_fn
             (i32.const 0)
             (local.tee $208
              (call $new_lval_list
               (call $prefix_list
                (call $retain
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 3238)
                 )
                )
                (call $prefix_list
                 (call $retain
                  (i32.add
                   (global.get $data_offset)
                   (i32.const 98)
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
            (local.get $208)
           )
           (local.get $209)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (block $let_body_result_209 (result i32)
      (local.set $229
       (block $if_block_207 (result i32)
        (local.set $228
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $213
               (block $sys_call_197 (result i32)
                (local.set $212
                 (call $not_eq_fn
                  (i32.const 0)
                  (local.tee $211
                   (call $new_lval_list
                    (call $prefix_list
                     (call $retain
                      (local.get $210)
                     )
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 3358)
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
                 (local.get $211)
                )
                (local.get $212)
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_205 (result i32)
           (local.set $216
            (block $sys_call_198 (result i32)
             (local.set $215
              (call $pr_fn
               (i32.const 0)
               (local.tee $214
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $214)
             )
             (local.get $215)
            )
           )
           (local.set $219
            (block $sys_call_199 (result i32)
             (local.set $218
              (call $pr_fn
               (i32.const 0)
               (local.tee $217
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 3598)
                   )
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
             (call $release
              (local.get $217)
             )
             (local.get $218)
            )
           )
           (local.set $222
            (block $sys_call_200 (result i32)
             (local.set $221
              (call $pr_fn
               (i32.const 0)
               (local.tee $220
                (call $new_lval_list
                 (call $prefix_list
                  (call $retain
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 210)
                   )
                  )
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 3358)
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
              (local.get $220)
             )
             (local.get $221)
            )
           )
           (block $do_body_result_204 (result i32)
            (local.set $227
             (block $sys_call_202 (result i32)
              (local.set $226
               (call $pr_fn
                (i32.const 0)
                (local.tee $225
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 251)
                    )
                   )
                   (call $prefix_list
                    (call $new_lval_vector
                     (call $prefix_list
                      (call $retain
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 3238)
                       )
                      )
                      (call $prefix_list
                       (call $retain
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 98)
                        )
                       )
                       (call $prefix_list
                        (block $sys_call_201 (result i32)
                         (local.set $224
                          (call $add_fn
                           (i32.const 0)
                           (local.tee $223
                            (call $new_lval_list
                             (call $prefix_list
                              (call $retain
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 3238)
                               )
                              )
                              (call $prefix_list
                               (call $retain
                                (i32.add
                                 (global.get $data_offset)
                                 (i32.const 98)
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
                          (local.get $223)
                         )
                         (local.get $224)
                        )
                        (i32.const 0)
                       )
                      )
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
               (local.get $225)
              )
              (local.get $226)
             )
            )
            (block $release_locals_for_do_203
             (call $release
              (local.get $216)
             )
             (call $release
              (local.get $219)
             )
             (call $release
              (local.get $222)
             )
            )
            (local.get $227)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 522)
           )
          )
         )
        )
        (call $release
         (local.get $213)
        )
        (local.get $228)
       )
      )
      (block $release_locals_for_let_208
       (call $release
        (local.get $210)
       )
      )
      (local.get $229)
     )
    )
   )
   (block $do_body_result_225 (result i32)
    (local.set $249
     (block $let_223 (result i32)
      (local.set $231
       (call $new_lval_vector
        (i32.const 0)
       )
      )
      (block $let_body_result_222 (result i32)
       (local.set $248
        (block $if_block_220 (result i32)
         (local.set $247
          (if (result i32)
           (i32.xor
            (i32.eq
             (i32.and
              (i32.load8_u offset=1
               (local.tee $234
                (block $sys_call_211 (result i32)
                 (local.set $233
                  (call $not_eq_fn
                   (i32.const 0)
                   (local.tee $232
                    (call $new_lval_list
                     (call $prefix_list
                      (call $retain
                       (local.get $231)
                      )
                      (call $prefix_list
                       (call $retain
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 3634)
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
                  (local.get $232)
                 )
                 (local.get $233)
                )
               )
              )
              (i32.const 253)
             )
             (i32.const 5)
            )
            (i32.const 1)
           )
           (block $do_218 (result i32)
            (local.set $237
             (block $sys_call_212 (result i32)
              (local.set $236
               (call $pr_fn
                (i32.const 0)
                (local.tee $235
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 103)
                    )
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $235)
              )
              (local.get $236)
             )
            )
            (local.set $240
             (block $sys_call_213 (result i32)
              (local.set $239
               (call $pr_fn
                (i32.const 0)
                (local.tee $238
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 3670)
                    )
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
              (call $release
               (local.get $238)
              )
              (local.get $239)
             )
            )
            (local.set $243
             (block $sys_call_214 (result i32)
              (local.set $242
               (call $pr_fn
                (i32.const 0)
                (local.tee $241
                 (call $new_lval_list
                  (call $prefix_list
                   (call $retain
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 210)
                    )
                   )
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 3634)
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
               (local.get $241)
              )
              (local.get $242)
             )
            )
            (block $do_body_result_217 (result i32)
             (local.set $246
              (block $sys_call_215 (result i32)
               (local.set $245
                (call $pr_fn
                 (i32.const 0)
                 (local.tee $244
                  (call $new_lval_list
                   (call $prefix_list
                    (call $retain
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 251)
                     )
                    )
                    (call $prefix_list
                     (call $new_lval_vector
                      (i32.const 0)
                     )
                     (i32.const 0)
                    )
                   )
                  )
                 )
                )
               )
               (call $release
                (local.get $244)
               )
               (local.get $245)
              )
             )
             (block $release_locals_for_do_216
              (call $release
               (local.get $237)
              )
              (call $release
               (local.get $240)
              )
              (call $release
               (local.get $243)
              )
             )
             (local.get $246)
            )
           )
           (call $retain
            (i32.add
             (global.get $data_offset)
             (i32.const 522)
            )
           )
          )
         )
         (call $release
          (local.get $234)
         )
         (local.get $247)
        )
       )
       (block $release_locals_for_let_221
        (call $release
         (local.get $231)
        )
       )
       (local.get $248)
      )
     )
    )
    (block $release_locals_for_do_224
     (call $release
      (local.get $3)
     )
     (call $release
      (local.get $30)
     )
     (call $release
      (local.get $61)
     )
     (call $release
      (local.get $96)
     )
     (call $release
      (local.get $131)
     )
     (call $release
      (local.get $154)
     )
     (call $release
      (local.get $189)
     )
     (call $release
      (local.get $207)
     )
     (call $release
      (local.get $230)
     )
    )
    (local.get $249)
   )
  )
 )
 ;; custom section "symbol_table", size 25
 ;; custom section "deps", size 0, contents: ""
 ;; custom section "data_size", size 4, contents: "4222"
 ;; custom section "fn_table_size", size 1, contents: "1"
)
