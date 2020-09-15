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
 (data (global.get $data_offset) "\0f\0f\0f\0ftest\00from test2\00\01\00\00\00\00\00\00\00\00\00\00\00$\00\00\00\02\t\00\00\00\00\00\00\t\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00H\00\00\00\02\08\00\00\05\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00l\00\00\00\02\08\00\00\02\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\90\00\00\00\02\08\00\00\03\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b4\00\00\00\02\08\00\00\0b\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ffFAIL\00\01\00\00\00\00\00\00\00\00\00\00\00\dd\00\00\00\02\t\00\00\00\00\00\00\c8\00\00\00\00\00\00\00\ff\ff\ff\ff+\00\01\00\00\00\00\00\00\00\00\00\00\00\03\01\00\00\00\ff\ff\ff\00\00\00\00\f1\00\00\00\00\00\00\00\ff\ff\ff\ff*\00\01\00\00\00\00\00\00\00\00\00\00\00)\01\00\00\00\ff\ff\ff\00\00\00\00\17\01\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00M\01\00\00\1e\00\00\00\90\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00M\01\00\00\a1\03\00\00l\00\00\00M\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00M\01\00\00~p\00\00)\01\00\00i\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a1\01\00\00\01\10\00\00\00\00\00\00\00\00\00\00\85\01\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c5\01\00\00\9dp\00\00\a1\01\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\'\01\00\00\02\a3\0d\00H\00\00\00\c5\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\'\01\00\00=\bd\a6\01\03\01\00\00\e1\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\19\02\00\00\01\10\00\00\00\00\00\00\00\00\00\00\fd\01\00\00=\bd\a6\01Expected:\00\01\00\00\00\00\00\00\00\00\00\00\00G\02\00\00\02\t\00\00\00\00\00\00-\02\00\00\00\00\00\00\ff\ff\ff\ffGot:\00\01\00\00\00\00\00\00\00\00\00\00\00p\02\00\00\02\t\00\00\00\00\00\00[\02\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\94\02\00\00\02\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b8\02\00\00\02\08\00\00\08\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff-\00\01\00\00\00\00\00\00\00\00\00\00\00\de\02\00\00\00\ff\ff\ff\00\00\00\00\cc\02\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\02\03\00\00\1e\00\00\00\90\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\03\00\00\a1\03\00\00l\00\00\00\02\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\03\00\00~p\00\00)\01\00\00\1e\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00V\03\00\00\01\10\00\00\00\00\00\00\00\00\00\00:\03\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00z\03\00\00\9dp\00\00V\03\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\03\00\00\02\a3\0d\00H\00\00\00z\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\03\00\00=\bd\a6\01\03\01\00\00\96\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ce\03\00\00\01\10\00\00\00\00\00\00\00\00\00\00\b2\03\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\f2\03\00\00\1e\00\00\00\90\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f2\03\00\00\df\c0\a6\01\ce\03\00\00\f2\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\03\00\00\00[13\de\02\00\00\0e\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00F\04\00\00\01\10\00\00\00\00\00\00\00\00\00\00*\04\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00j\04\00\00\02\08\00\00\04\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff/\00\01\00\00\00\00\00\00\00\00\00\00\00\90\04\00\00\00\ff\ff\ff\00\00\00\00~\04\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00\1e\00\00\00\90\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00\a1\03\00\00l\00\00\00\b4\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00~p\00\00)\01\00\00\d0\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00\08\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00\ec\04\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00,\05\00\00\9dp\00\00\08\05\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00\02\a3\0d\00H\00\00\00,\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00=\bd\a6\01\03\01\00\00H\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\80\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00d\05\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\a4\05\00\00\1e\00\00\00\90\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a4\05\00\00\df\c0\a6\01\80\05\00\00\a4\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00\00[13\de\02\00\00\c0\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f8\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00\dc\05\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00\1c\06\00\00\1e\00\00\00j\04\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1c\06\00\00\a2^13\f8\05\00\00\1c\06\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b4\04\00\00\9du\fa2\90\04\00\008\06\00\00\01\00\00\00\00\00\00\00\00\00\00\00p\06\00\00\01\10\00\00\00\00\00\00\00\00\00\00T\06\00\00\9du\fa2\01\00\00\00\00\00\00\00\00\00\00\00\94\06\00\00\02\08\00\00\03\02\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b8\06\00\00\02\08\00\00W\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\dc\06\00\00\02\08\00\007\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\00\07\00\00\02\08\00\00.\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00$\07\00\00\02\08\00\00\1b\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00H\07\00\00\02\08\00\00\f2\03\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00\1e\00\00\00\dc\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00\a1\03\00\00\b8\06\00\00l\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00~p\00\00)\01\00\00\88\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c0\07\00\00\01\10\00\00\00\00\00\00\00\00\00\00\a4\07\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\07\00\00\9dp\00\00\c0\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00\02\a3\0d\00\94\06\00\00\e4\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00=\bd\a6\01\03\01\00\00\00\08\00\00\01\00\00\00\00\00\00\00\00\00\00\008\08\00\00\01\10\00\00\00\00\00\00\00\00\00\00\1c\08\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\\\08\00\00\1e\00\00\00\00\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\\\08\00\00\df\c0\a6\018\08\00\00\\\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00\00[13\de\02\00\00x\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b0\08\00\00\01\10\00\00\00\00\00\00\00\00\00\00\94\08\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00\d4\08\00\00\1e\00\00\00$\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\d4\08\00\00\a2^13\b0\08\00\00\d4\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00l\07\00\00\9du\fa2\90\04\00\00\f0\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00(\t\00\00\01\10\00\00\00\00\00\00\00\00\00\00\0c\t\00\00\9du\fa2\01\00\00\00\00\00\00\00\00\00\00\00L\t\00\00\02\08\00\00\fd\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00p\t\00\00\02\08\00\00\06\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\94\t\00\00\02\08\00\00\ee\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b8\t\00\00\1e\00\00\00p\t\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b8\t\00\00\a1\03\00\00L\t\00\00\b8\t\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b8\t\00\00~p\00\00)\01\00\00\d4\t\00\00\01\00\00\00\00\00\00\00\00\00\00\00\0c\n\00\00\01\10\00\00\00\00\00\00\00\00\00\00\f0\t\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\000\n\00\00\02\08\00\00\03\02\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00T\n\00\00\02\08\00\00\a9\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00x\n\00\00\02\08\00\007\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\9c\n\00\00\02\08\00\00(\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\c0\n\00\00\02\08\00\00\1e\fc\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00\1e\00\00\00x\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00\a1\03\00\00T\n\00\00\e4\n\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00~p\00\00)\01\00\00\00\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\008\0b\00\00\01\10\00\00\00\00\00\00\00\00\00\00\1c\0b\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\\\0b\00\00\9dp\00\008\0b\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00\02\a3\0d\000\n\00\00\\\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00=\bd\a6\01\03\01\00\00x\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b0\0b\00\00\01\10\00\00\00\00\00\00\00\00\00\00\94\0b\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\d4\0b\00\00\1e\00\00\00\9c\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\d4\0b\00\00\df\c0\a6\01\b0\0b\00\00\d4\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00\00[13\de\02\00\00\f0\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00(\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\0c\0c\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00L\0c\00\00\1e\00\00\00$\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00L\0c\00\00\a2^13(\0c\00\00L\0c\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e4\n\00\00\9du\fa2\90\04\00\00h\0c\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a0\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\84\0c\00\00\9du\fa2\01\00\00\00\00\00\00\00\00\00\00\00\c4\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e8\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\0c\0d\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\000\0d\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00T\0d\00\00\1e\00\00\00l\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00T\0d\00\00\a1\03\00\000\0d\00\00T\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00T\0d\00\00~p\00\00\03\01\00\00p\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a8\0d\00\00\01\10\00\00\00\00\00\00\00\00\00\00\8c\0d\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\cc\0d\00\00\9dp\00\00\a8\0d\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00T\0d\00\00\02\a3\0d\00l\00\00\00\cc\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00T\0d\00\00=\bd\a6\010\0d\00\00\e8\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00 \0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\04\0e\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00D\0e\00\00\1e\00\00\00\90\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00D\0e\00\00\a1\03\00\00l\00\00\00D\0e\00\00\01\00\00\00\00\00\00\00\00\00\00\00D\0e\00\00~p\00\000\0d\00\00`\0e\00\00\01\00\00\00\00\00\00\00\00\00\00\00\98\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00|\0e\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\bc\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e0\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\04\0f\00\00\19\ff\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\14\00\00\008\00\00\00\\\00\00\00\80\00\00\00\a4\00\00\00\cd\00\00\00\f3\00\00\00\19\01\00\00\91\01\00\00\t\02\00\007\02\00\00`\02\00\00\84\02\00\00\a8\02\00\00\ce\02\00\00F\03\00\00\be\03\00\006\04\00\00Z\04\00\00\80\04\00\00\f8\04\00\00p\05\00\00\e8\05\00\00`\06\00\00\84\06\00\00\a8\06\00\00\cc\06\00\00\f0\06\00\00\14\07\00\008\07\00\00\b0\07\00\00(\08\00\00\a0\08\00\00\18\t\00\00<\t\00\00`\t\00\00\84\t\00\00\fc\t\00\00 \n\00\00D\n\00\00h\n\00\00\8c\n\00\00\b0\n\00\00(\0b\00\00\a0\0b\00\00\18\0c\00\00\90\0c\00\00\b4\0c\00\00\d8\0c\00\00\fc\0c\00\00 \0d\00\00\98\0d\00\00\10\0e\00\00\88\0e\00\00\ac\0e\00\00\d0\0e\00\00=\01\00\00Y\01\00\00u\01\00\00\b5\01\00\00\d1\01\00\00\ed\01\00\00\f2\02\00\00\0e\03\00\00*\03\00\00j\03\00\00\86\03\00\00\a2\03\00\00\e2\03\00\00\fe\03\00\00\1a\04\00\00\a4\04\00\00\c0\04\00\00\dc\04\00\00\1c\05\00\008\05\00\00T\05\00\00\94\05\00\00\b0\05\00\00\cc\05\00\00\0c\06\00\00(\06\00\00D\06\00\00\\\07\00\00x\07\00\00\94\07\00\00\d4\07\00\00\f0\07\00\00\0c\08\00\00L\08\00\00h\08\00\00\84\08\00\00\c4\08\00\00\e0\08\00\00\fc\08\00\00\a8\t\00\00\c4\t\00\00\e0\t\00\00\d4\n\00\00\f0\n\00\00\0c\0b\00\00L\0b\00\00h\0b\00\00\84\0b\00\00\c4\0b\00\00\e0\0b\00\00\fc\0b\00\00<\0c\00\00X\0c\00\00t\0c\00\00D\0d\00\00`\0d\00\00|\0d\00\00\bc\0d\00\00\d8\0d\00\00\f4\0d\00\004\0e\00\00P\0e\00\00l\0e\00\00\f4\0e\00\00\18\0f\00\008\00\00\00\f8\0f\00\00?\00\00\00\f4\10\00\00\01\00\00\00\18\00\00\00")
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
 (export "test" (func $test))
 (export "stack_pointer" (global $stack_pointer))
 (export "mem" (memory $0))
 (func $test (param $0 i32) (param $1 i32) (result i32)
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
  (block $do_70 (result i32)
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
     (i32.const 4)
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
      (i32.const 4)
     )
     (nop)
    )
   )
   (block $store_args_in_locals_1
    (local.set $2
     (i32.sub
      (global.get $stack_pointer)
      (i32.const 0)
     )
    )
   )
   (local.set $3
    (call $print_fn
     (i32.const 0)
     (call $new_lval_list
      (call $list_cons
       (i32.add
        (global.get $data_offset)
        (i32.const 36)
       )
       (i32.const 0)
      )
     )
    )
   )
   (local.set $12
    (block $let_9 (result i32)
     (local.set $4
      (call $add_fn
       (i32.const 0)
       (call $new_lval_list
        (call $list_cons
         (i32.add
          (global.get $data_offset)
          (i32.const 72)
         )
         (call $list_cons
          (call $mul_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 108)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 144)
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
     (block $let_body_result_8 (result i32)
      (local.set $11
       (block $if_block_6 (result i32)
        (local.set $10
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $5
               (call $not_eq_fn
                (i32.const 0)
                (call $new_lval_list
                 (call $list_cons
                  (local.get $4)
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 180)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_4 (result i32)
           (local.set $6
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $7
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 537)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $8
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 583)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 180)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $do_body_result_3 (result i32)
            (local.set $9
             (call $pr_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 624)
                )
                (call $list_cons
                 (call $add_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 72)
                    )
                    (call $list_cons
                     (call $mul_fn
                      (i32.const 0)
                      (call $new_lval_list
                       (call $list_cons
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 108)
                        )
                        (call $list_cons
                         (i32.add
                          (global.get $data_offset)
                          (i32.const 144)
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
                 (i32.const 0)
                )
               )
              )
             )
            )
            (block $release_locals_for_do_2
             (call $release
              (local.get $6)
             )
             (call $release
              (local.get $7)
             )
             (call $release
              (local.get $8)
             )
            )
            (local.get $9)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 660)
           )
          )
         )
        )
        (call $release
         (local.get $5)
        )
        (local.get $10)
       )
      )
      (block $release_locals_for_let_7
       (call $release
        (local.get $4)
       )
      )
      (local.get $11)
     )
    )
   )
   (local.set $21
    (block $let_17 (result i32)
     (local.set $13
      (call $sub_fn
       (i32.const 0)
       (call $new_lval_list
        (call $list_cons
         (call $add_fn
          (i32.const 0)
          (call $new_lval_list
           (call $list_cons
            (i32.add
             (global.get $data_offset)
             (i32.const 72)
            )
            (call $list_cons
             (call $mul_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 108)
                )
                (call $list_cons
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 144)
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
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 144)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (block $let_body_result_16 (result i32)
      (local.set $20
       (block $if_block_14 (result i32)
        (local.set $19
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $14
               (call $not_eq_fn
                (i32.const 0)
                (call $new_lval_list
                 (call $list_cons
                  (local.get $13)
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 696)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_12 (result i32)
           (local.set $15
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $16
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 1094)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $17
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 583)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 696)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $do_body_result_11 (result i32)
            (local.set $18
             (call $pr_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 624)
                )
                (call $list_cons
                 (call $sub_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (call $add_fn
                     (i32.const 0)
                     (call $new_lval_list
                      (call $list_cons
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 72)
                       )
                       (call $list_cons
                        (call $mul_fn
                         (i32.const 0)
                         (call $new_lval_list
                          (call $list_cons
                           (i32.add
                            (global.get $data_offset)
                            (i32.const 108)
                           )
                           (call $list_cons
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 144)
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
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 144)
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
            (block $release_locals_for_do_10
             (call $release
              (local.get $15)
             )
             (call $release
              (local.get $16)
             )
             (call $release
              (local.get $17)
             )
            )
            (local.get $18)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 660)
           )
          )
         )
        )
        (call $release
         (local.get $14)
        )
        (local.get $19)
       )
      )
      (block $release_locals_for_let_15
       (call $release
        (local.get $13)
       )
      )
      (local.get $20)
     )
    )
   )
   (local.set $30
    (block $let_25 (result i32)
     (local.set $22
      (call $div_fn
       (i32.const 0)
       (call $new_lval_list
        (call $list_cons
         (call $sub_fn
          (i32.const 0)
          (call $new_lval_list
           (call $list_cons
            (call $add_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 72)
               )
               (call $list_cons
                (call $mul_fn
                 (i32.const 0)
                 (call $new_lval_list
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 108)
                   )
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 144)
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
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 144)
             )
             (i32.const 0)
            )
           )
          )
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 1130)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (block $let_body_result_24 (result i32)
      (local.set $29
       (block $if_block_22 (result i32)
        (local.set $28
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $23
               (call $not_eq_fn
                (i32.const 0)
                (call $new_lval_list
                 (call $list_cons
                  (local.get $22)
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 108)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_20 (result i32)
           (local.set $24
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $25
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 1648)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $26
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 583)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 108)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $do_body_result_19 (result i32)
            (local.set $27
             (call $pr_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 624)
                )
                (call $list_cons
                 (call $div_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (call $sub_fn
                     (i32.const 0)
                     (call $new_lval_list
                      (call $list_cons
                       (call $add_fn
                        (i32.const 0)
                        (call $new_lval_list
                         (call $list_cons
                          (i32.add
                           (global.get $data_offset)
                           (i32.const 72)
                          )
                          (call $list_cons
                           (call $mul_fn
                            (i32.const 0)
                            (call $new_lval_list
                             (call $list_cons
                              (i32.add
                               (global.get $data_offset)
                               (i32.const 108)
                              )
                              (call $list_cons
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 144)
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
                       (call $list_cons
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 144)
                        )
                        (i32.const 0)
                       )
                      )
                     )
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 1130)
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
            (block $release_locals_for_do_18
             (call $release
              (local.get $24)
             )
             (call $release
              (local.get $25)
             )
             (call $release
              (local.get $26)
             )
            )
            (local.get $27)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 660)
           )
          )
         )
        )
        (call $release
         (local.get $23)
        )
        (local.get $28)
       )
      )
      (block $release_locals_for_let_23
       (call $release
        (local.get $22)
       )
      )
      (local.get $29)
     )
    )
   )
   (local.set $39
    (block $let_33 (result i32)
     (local.set $31
      (call $div_fn
       (i32.const 0)
       (call $new_lval_list
        (call $list_cons
         (call $sub_fn
          (i32.const 0)
          (call $new_lval_list
           (call $list_cons
            (call $add_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 1684)
               )
               (call $list_cons
                (call $mul_fn
                 (i32.const 0)
                 (call $new_lval_list
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 1720)
                   )
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 1756)
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
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 1792)
             )
             (i32.const 0)
            )
           )
          )
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 1828)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (block $let_body_result_32 (result i32)
      (local.set $38
       (block $if_block_30 (result i32)
        (local.set $37
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $32
               (call $not_eq_fn
                (i32.const 0)
                (call $new_lval_list
                 (call $list_cons
                  (local.get $31)
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 1864)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_28 (result i32)
           (local.set $33
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $34
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 2344)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $35
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 583)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 1864)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $do_body_result_27 (result i32)
            (local.set $36
             (call $pr_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 624)
                )
                (call $list_cons
                 (call $div_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (call $sub_fn
                     (i32.const 0)
                     (call $new_lval_list
                      (call $list_cons
                       (call $add_fn
                        (i32.const 0)
                        (call $new_lval_list
                         (call $list_cons
                          (i32.add
                           (global.get $data_offset)
                           (i32.const 1684)
                          )
                          (call $list_cons
                           (call $mul_fn
                            (i32.const 0)
                            (call $new_lval_list
                             (call $list_cons
                              (i32.add
                               (global.get $data_offset)
                               (i32.const 1720)
                              )
                              (call $list_cons
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 1756)
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
                       (call $list_cons
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 1792)
                        )
                        (i32.const 0)
                       )
                      )
                     )
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 1828)
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
            (block $release_locals_for_do_26
             (call $release
              (local.get $33)
             )
             (call $release
              (local.get $34)
             )
             (call $release
              (local.get $35)
             )
            )
            (local.get $36)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 660)
           )
          )
         )
        )
        (call $release
         (local.get $32)
        )
        (local.get $37)
       )
      )
      (block $release_locals_for_let_31
       (call $release
        (local.get $31)
       )
      )
      (local.get $38)
     )
    )
   )
   (local.set $48
    (block $let_41 (result i32)
     (local.set $40
      (call $mul_fn
       (i32.const 0)
       (call $new_lval_list
        (call $list_cons
         (i32.add
          (global.get $data_offset)
          (i32.const 2380)
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 2416)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (block $let_body_result_40 (result i32)
      (local.set $47
       (block $if_block_38 (result i32)
        (local.set $46
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $41
               (call $not_eq_fn
                (i32.const 0)
                (call $new_lval_list
                 (call $list_cons
                  (local.get $40)
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 2452)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_36 (result i32)
           (local.set $42
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $43
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 2572)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $44
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 583)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 2452)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $do_body_result_35 (result i32)
            (local.set $45
             (call $pr_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 624)
                )
                (call $list_cons
                 (call $mul_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2380)
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 2416)
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
            (block $release_locals_for_do_34
             (call $release
              (local.get $42)
             )
             (call $release
              (local.get $43)
             )
             (call $release
              (local.get $44)
             )
            )
            (local.get $45)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 660)
           )
          )
         )
        )
        (call $release
         (local.get $41)
        )
        (local.get $46)
       )
      )
      (block $release_locals_for_let_39
       (call $release
        (local.get $40)
       )
      )
      (local.get $47)
     )
    )
   )
   (local.set $57
    (block $let_49 (result i32)
     (local.set $49
      (call $div_fn
       (i32.const 0)
       (call $new_lval_list
        (call $list_cons
         (call $sub_fn
          (i32.const 0)
          (call $new_lval_list
           (call $list_cons
            (call $add_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 2608)
               )
               (call $list_cons
                (call $mul_fn
                 (i32.const 0)
                 (call $new_lval_list
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 2644)
                   )
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2680)
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
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 2716)
             )
             (i32.const 0)
            )
           )
          )
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 1828)
          )
          (i32.const 0)
         )
        )
       )
      )
     )
     (block $let_body_result_48 (result i32)
      (local.set $56
       (block $if_block_46 (result i32)
        (local.set $55
         (if (result i32)
          (i32.xor
           (i32.eq
            (i32.and
             (i32.load8_u offset=1
              (local.tee $50
               (call $not_eq_fn
                (i32.const 0)
                (call $new_lval_list
                 (call $list_cons
                  (local.get $49)
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 2752)
                   )
                   (i32.const 0)
                  )
                 )
                )
               )
              )
             )
             (i32.const 253)
            )
            (i32.const 5)
           )
           (i32.const 1)
          )
          (block $do_44 (result i32)
           (local.set $51
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 221)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $52
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 3232)
               )
               (i32.const 0)
              )
             )
            )
           )
           (local.set $53
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 583)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 2752)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $do_body_result_43 (result i32)
            (local.set $54
             (call $pr_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 624)
                )
                (call $list_cons
                 (call $div_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (call $sub_fn
                     (i32.const 0)
                     (call $new_lval_list
                      (call $list_cons
                       (call $add_fn
                        (i32.const 0)
                        (call $new_lval_list
                         (call $list_cons
                          (i32.add
                           (global.get $data_offset)
                           (i32.const 2608)
                          )
                          (call $list_cons
                           (call $mul_fn
                            (i32.const 0)
                            (call $new_lval_list
                             (call $list_cons
                              (i32.add
                               (global.get $data_offset)
                               (i32.const 2644)
                              )
                              (call $list_cons
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 2680)
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
                       (call $list_cons
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 2716)
                        )
                        (i32.const 0)
                       )
                      )
                     )
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 1828)
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
            (block $release_locals_for_do_42
             (call $release
              (local.get $51)
             )
             (call $release
              (local.get $52)
             )
             (call $release
              (local.get $53)
             )
            )
            (local.get $54)
           )
          )
          (call $retain
           (i32.add
            (global.get $data_offset)
            (i32.const 660)
           )
          )
         )
        )
        (call $release
         (local.get $50)
        )
        (local.get $55)
       )
      )
      (block $release_locals_for_let_47
       (call $release
        (local.get $49)
       )
      )
      (local.get $56)
     )
    )
   )
   (local.set $65
    (block $let_55 (result i32)
     (local.set $58
      (i32.add
       (global.get $data_offset)
       (i32.const 3268)
      )
     )
     (block $if_block_54 (result i32)
      (local.set $64
       (if (result i32)
        (i32.xor
         (i32.eq
          (i32.and
           (i32.load8_u offset=1
            (local.tee $59
             (call $not_eq_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (local.get $58)
                (call $list_cons
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 3304)
                 )
                 (i32.const 0)
                )
               )
              )
             )
            )
           )
           (i32.const 253)
          )
          (i32.const 5)
         )
         (i32.const 1)
        )
        (block $do_52 (result i32)
         (local.set $60
          (call $pr_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 221)
             )
             (i32.const 0)
            )
           )
          )
         )
         (local.set $61
          (call $pr_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 3340)
             )
             (i32.const 0)
            )
           )
          )
         )
         (local.set $62
          (call $pr_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 583)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 3304)
              )
              (i32.const 0)
             )
            )
           )
          )
         )
         (block $do_body_result_51 (result i32)
          (local.set $63
           (call $pr_fn
            (i32.const 0)
            (call $new_lval_list
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 624)
              )
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 3268)
               )
               (i32.const 0)
              )
             )
            )
           )
          )
          (block $release_locals_for_do_50
           (call $release
            (local.get $60)
           )
           (call $release
            (local.get $61)
           )
           (call $release
            (local.get $62)
           )
          )
          (local.get $63)
         )
        )
        (call $retain
         (i32.add
          (global.get $data_offset)
          (i32.const 660)
         )
        )
       )
      )
      (call $release
       (local.get $59)
      )
      (local.get $64)
     )
    )
   )
   (local.set $73
    (block $let_61 (result i32)
     (local.set $66
      (i32.add
       (global.get $data_offset)
       (i32.const 3616)
      )
     )
     (block $if_block_60 (result i32)
      (local.set $72
       (if (result i32)
        (i32.xor
         (i32.eq
          (i32.and
           (i32.load8_u offset=1
            (local.tee $67
             (call $not_eq_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (local.get $66)
                (call $list_cons
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 3736)
                 )
                 (i32.const 0)
                )
               )
              )
             )
            )
           )
           (i32.const 253)
          )
          (i32.const 5)
         )
         (i32.const 1)
        )
        (block $do_58 (result i32)
         (local.set $68
          (call $pr_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 221)
             )
             (i32.const 0)
            )
           )
          )
         )
         (local.set $69
          (call $pr_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 3616)
             )
             (i32.const 0)
            )
           )
          )
         )
         (local.set $70
          (call $pr_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 583)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 3736)
              )
              (i32.const 0)
             )
            )
           )
          )
         )
         (block $do_body_result_57 (result i32)
          (local.set $71
           (call $pr_fn
            (i32.const 0)
            (call $new_lval_list
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 624)
              )
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 3616)
               )
               (i32.const 0)
              )
             )
            )
           )
          )
          (block $release_locals_for_do_56
           (call $release
            (local.get $68)
           )
           (call $release
            (local.get $69)
           )
           (call $release
            (local.get $70)
           )
          )
          (local.get $71)
         )
        )
        (call $retain
         (i32.add
          (global.get $data_offset)
          (i32.const 660)
         )
        )
       )
      )
      (call $release
       (local.get $67)
      )
      (local.get $72)
     )
    )
   )
   (block $do_body_result_69 (result i32)
    (local.set $81
     (block $let_67 (result i32)
      (local.set $74
       (i32.add
        (global.get $data_offset)
        (i32.const 3772)
       )
      )
      (block $if_block_66 (result i32)
       (local.set $80
        (if (result i32)
         (i32.xor
          (i32.eq
           (i32.and
            (i32.load8_u offset=1
             (local.tee $75
              (call $not_eq_fn
               (i32.const 0)
               (call $new_lval_list
                (call $list_cons
                 (local.get $74)
                 (call $list_cons
                  (i32.add
                   (global.get $data_offset)
                   (i32.const 3808)
                  )
                  (i32.const 0)
                 )
                )
               )
              )
             )
            )
            (i32.const 253)
           )
           (i32.const 5)
          )
          (i32.const 1)
         )
         (block $do_64 (result i32)
          (local.set $76
           (call $pr_fn
            (i32.const 0)
            (call $new_lval_list
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 221)
              )
              (i32.const 0)
             )
            )
           )
          )
          (local.set $77
           (call $pr_fn
            (i32.const 0)
            (call $new_lval_list
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 3772)
              )
              (i32.const 0)
             )
            )
           )
          )
          (local.set $78
           (call $pr_fn
            (i32.const 0)
            (call $new_lval_list
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 583)
              )
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 3808)
               )
               (i32.const 0)
              )
             )
            )
           )
          )
          (block $do_body_result_63 (result i32)
           (local.set $79
            (call $pr_fn
             (i32.const 0)
             (call $new_lval_list
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 624)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 3772)
                )
                (i32.const 0)
               )
              )
             )
            )
           )
           (block $release_locals_for_do_62
            (call $release
             (local.get $76)
            )
            (call $release
             (local.get $77)
            )
            (call $release
             (local.get $78)
            )
           )
           (local.get $79)
          )
         )
         (call $retain
          (i32.add
           (global.get $data_offset)
           (i32.const 660)
          )
         )
        )
       )
       (call $release
        (local.get $75)
       )
       (local.get $80)
      )
     )
    )
    (block $release_locals_for_do_68
     (call $release
      (local.get $3)
     )
     (call $release
      (local.get $12)
     )
     (call $release
      (local.get $21)
     )
     (call $release
      (local.get $30)
     )
     (call $release
      (local.get $39)
     )
     (call $release
      (local.get $48)
     )
     (call $release
      (local.get $57)
     )
     (call $release
      (local.get $65)
     )
     (call $release
      (local.get $73)
     )
    )
    (local.get $81)
   )
  )
 )
 ;; custom section "symbol_table", size 25
 ;; custom section "deps", size 0, contents: ""
 ;; custom section "data_size", size 4, contents: "4372"
 ;; custom section "fn_table_size", size 1, contents: "1"
)
