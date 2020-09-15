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
 (data (global.get $data_offset) "\0f\0f\0f\0ftest\00test2\00\01\00\00\00\00\00\00\00\00\00\00\00\1f\00\00\00\02\t\00\00\00\00\00\00\t\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00C\00\00\00\02\08\00\00\05\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00g\00\00\00\02\08\00\00\02\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\8b\00\00\00\02\08\00\00\03\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\af\00\00\00\02\08\00\00\0b\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ffFAIL\00\01\00\00\00\00\00\00\00\00\00\00\00\d8\00\00\00\02\t\00\00\00\00\00\00\c3\00\00\00\00\00\00\00\ff\ff\ff\ff+\00\01\00\00\00\00\00\00\00\00\00\00\00\fe\00\00\00\00\ff\ff\ff\00\00\00\00\ec\00\00\00\00\00\00\00\ff\ff\ff\ff*\00\01\00\00\00\00\00\00\00\00\00\00\00$\01\00\00\00\ff\ff\ff\00\00\00\00\12\01\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00H\01\00\00\1e\00\00\00\8b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00H\01\00\00\a1\03\00\00g\00\00\00H\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00H\01\00\00~p\00\00$\01\00\00d\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\9c\01\00\00\01\10\00\00\00\00\00\00\00\00\00\00\80\01\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c0\01\00\00\9dp\00\00\9c\01\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\"\01\00\00\02\a3\0d\00C\00\00\00\c0\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\"\01\00\00=\bd\a6\01\fe\00\00\00\dc\01\00\00\01\00\00\00\00\00\00\00\00\00\00\00\14\02\00\00\01\10\00\00\00\00\00\00\00\00\00\00\f8\01\00\00=\bd\a6\01Expected:\00\01\00\00\00\00\00\00\00\00\00\00\00B\02\00\00\02\t\00\00\00\00\00\00(\02\00\00\00\00\00\00\ff\ff\ff\ffGot:\00\01\00\00\00\00\00\00\00\00\00\00\00k\02\00\00\02\t\00\00\00\00\00\00V\02\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\8f\02\00\00\02\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b3\02\00\00\02\08\00\00\08\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff-\00\01\00\00\00\00\00\00\00\00\00\00\00\d9\02\00\00\00\ff\ff\ff\00\00\00\00\c7\02\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\fd\02\00\00\1e\00\00\00\8b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\fd\02\00\00\a1\03\00\00g\00\00\00\fd\02\00\00\01\00\00\00\00\00\00\00\00\00\00\00\fd\02\00\00~p\00\00$\01\00\00\19\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00Q\03\00\00\01\10\00\00\00\00\00\00\00\00\00\005\03\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00u\03\00\00\9dp\00\00Q\03\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\fd\02\00\00\02\a3\0d\00C\00\00\00u\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\fd\02\00\00=\bd\a6\01\fe\00\00\00\91\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c9\03\00\00\01\10\00\00\00\00\00\00\00\00\00\00\ad\03\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\ed\03\00\00\1e\00\00\00\8b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ed\03\00\00\df\c0\a6\01\c9\03\00\00\ed\03\00\00\01\00\00\00\00\00\00\00\00\00\00\00\fd\02\00\00\00[13\d9\02\00\00\t\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00A\04\00\00\01\10\00\00\00\00\00\00\00\00\00\00%\04\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00e\04\00\00\02\08\00\00\04\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff/\00\01\00\00\00\00\00\00\00\00\00\00\00\8b\04\00\00\00\ff\ff\ff\00\00\00\00y\04\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00\1e\00\00\00\8b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00\a1\03\00\00g\00\00\00\af\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00~p\00\00$\01\00\00\cb\04\00\00\01\00\00\00\00\00\00\00\00\00\00\00\03\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00\e7\04\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\'\05\00\00\9dp\00\00\03\05\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00\02\a3\0d\00C\00\00\00\'\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00=\bd\a6\01\fe\00\00\00C\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00{\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00_\05\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\9f\05\00\00\1e\00\00\00\8b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\9f\05\00\00\df\c0\a6\01{\05\00\00\9f\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00\00[13\d9\02\00\00\bb\05\00\00\01\00\00\00\00\00\00\00\00\00\00\00\f3\05\00\00\01\10\00\00\00\00\00\00\00\00\00\00\d7\05\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00\17\06\00\00\1e\00\00\00e\04\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\17\06\00\00\a2^13\f3\05\00\00\17\06\00\00\01\00\00\00\00\00\00\00\00\00\00\00\af\04\00\00\9du\fa2\8b\04\00\003\06\00\00\01\00\00\00\00\00\00\00\00\00\00\00k\06\00\00\01\10\00\00\00\00\00\00\00\00\00\00O\06\00\00\9du\fa2\01\00\00\00\00\00\00\00\00\00\00\00\8f\06\00\00\02\08\00\00\03\02\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b3\06\00\00\02\08\00\00W\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\d7\06\00\00\02\08\00\007\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\fb\06\00\00\02\08\00\00.\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\1f\07\00\00\02\08\00\00\1b\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00C\07\00\00\02\08\00\00\f2\03\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00\1e\00\00\00\d7\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00\a1\03\00\00\b3\06\00\00g\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00~p\00\00$\01\00\00\83\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00\bb\07\00\00\01\10\00\00\00\00\00\00\00\00\00\00\9f\07\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\07\00\00\9dp\00\00\bb\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00\02\a3\0d\00\8f\06\00\00\df\07\00\00\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00=\bd\a6\01\fe\00\00\00\fb\07\00\00\01\00\00\00\00\00\00\00\00\00\00\003\08\00\00\01\10\00\00\00\00\00\00\00\00\00\00\17\08\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00W\08\00\00\1e\00\00\00\fb\06\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00W\08\00\00\df\c0\a6\013\08\00\00W\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00\00[13\d9\02\00\00s\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ab\08\00\00\01\10\00\00\00\00\00\00\00\00\00\00\8f\08\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00\cf\08\00\00\1e\00\00\00\1f\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\cf\08\00\00\a2^13\ab\08\00\00\cf\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00g\07\00\00\9du\fa2\8b\04\00\00\eb\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00#\t\00\00\01\10\00\00\00\00\00\00\00\00\00\00\07\t\00\00\9du\fa2\01\00\00\00\00\00\00\00\00\00\00\00G\t\00\00\02\08\00\00\fd\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00k\t\00\00\02\08\00\00\06\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\8f\t\00\00\02\08\00\00\ee\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\b3\t\00\00\1e\00\00\00k\t\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b3\t\00\00\a1\03\00\00G\t\00\00\b3\t\00\00\01\00\00\00\00\00\00\00\00\00\00\00\b3\t\00\00~p\00\00$\01\00\00\cf\t\00\00\01\00\00\00\00\00\00\00\00\00\00\00\07\n\00\00\01\10\00\00\00\00\00\00\00\00\00\00\eb\t\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00+\n\00\00\02\08\00\00\03\02\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00O\n\00\00\02\08\00\00\a9\ff\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00s\n\00\00\02\08\00\007\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\97\n\00\00\02\08\00\00(\01\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\bb\n\00\00\02\08\00\00\1e\fc\ff\ff\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00\1e\00\00\00s\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00\a1\03\00\00O\n\00\00\df\n\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00~p\00\00$\01\00\00\fb\n\00\00\01\00\00\00\00\00\00\00\00\00\00\003\0b\00\00\01\10\00\00\00\00\00\00\00\00\00\00\17\0b\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00W\0b\00\00\9dp\00\003\0b\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00\02\a3\0d\00+\n\00\00W\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00=\bd\a6\01\fe\00\00\00s\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ab\0b\00\00\01\10\00\00\00\00\00\00\00\00\00\00\8f\0b\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\cf\0b\00\00\1e\00\00\00\97\n\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\cf\0b\00\00\df\c0\a6\01\ab\0b\00\00\cf\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00\00[13\d9\02\00\00\eb\0b\00\00\01\00\00\00\00\00\00\00\00\00\00\00#\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\07\0c\00\00\00[13\01\00\00\00\00\00\00\00\00\00\00\00G\0c\00\00\1e\00\00\00\1f\07\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00G\0c\00\00\a2^13#\0c\00\00G\0c\00\00\01\00\00\00\00\00\00\00\00\00\00\00\df\n\00\00\9du\fa2\8b\04\00\00c\0c\00\00\01\00\00\00\00\00\00\00\00\00\00\00\9b\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\7f\0c\00\00\9du\fa2\01\00\00\00\00\00\00\00\00\00\00\00\bf\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\e3\0c\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\07\0d\00\00\01\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00+\0d\00\00\02\08\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\ff\ff\ff\01\00\00\00\00\00\00\00\00\00\00\00O\0d\00\00\1e\00\00\00\8b\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00O\0d\00\00\a1\03\00\00g\00\00\00O\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00O\0d\00\00~p\00\00+\0d\00\00k\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00\a3\0d\00\00\01\12\00\00\00\00\00\00\00\00\00\00\87\0d\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c7\0d\00\00\1e\00\00\00g\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c7\0d\00\00\a1\03\00\00+\0d\00\00\c7\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c7\0d\00\00~p\00\00\fe\00\00\00\e3\0d\00\00\01\00\00\00\00\00\00\00\00\00\00\00\1b\0e\00\00\01\10\00\00\00\00\00\00\00\00\00\00\ff\0d\00\00~p\00\00\01\00\00\00\00\00\00\00\00\00\00\00?\0e\00\00\9dp\00\00\1b\0e\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c7\0d\00\00\02\a3\0d\00g\00\00\00?\0e\00\00\01\00\00\00\00\00\00\00\00\00\00\00\c7\0d\00\00=\bd\a6\01+\0d\00\00[\0e\00\00\01\00\00\00\00\00\00\00\00\00\00\00\93\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00w\0e\00\00=\bd\a6\01\01\00\00\00\00\00\00\00\00\00\00\00\b7\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\db\0e\00\00\01\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\ff\0e\00\00\19\ff\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\0f\00\00\003\00\00\00W\00\00\00{\00\00\00\9f\00\00\00\c8\00\00\00\ee\00\00\00\14\01\00\00\8c\01\00\00\04\02\00\002\02\00\00[\02\00\00\7f\02\00\00\a3\02\00\00\c9\02\00\00A\03\00\00\b9\03\00\001\04\00\00U\04\00\00{\04\00\00\f3\04\00\00k\05\00\00\e3\05\00\00[\06\00\00\7f\06\00\00\a3\06\00\00\c7\06\00\00\eb\06\00\00\0f\07\00\003\07\00\00\ab\07\00\00#\08\00\00\9b\08\00\00\13\t\00\007\t\00\00[\t\00\00\7f\t\00\00\f7\t\00\00\1b\n\00\00?\n\00\00c\n\00\00\87\n\00\00\ab\n\00\00#\0b\00\00\9b\0b\00\00\13\0c\00\00\8b\0c\00\00\af\0c\00\00\d3\0c\00\00\f7\0c\00\00\1b\0d\00\00\93\0d\00\00\0b\0e\00\00\83\0e\00\00\a7\0e\00\00\cb\0e\00\008\01\00\00T\01\00\00p\01\00\00\b0\01\00\00\cc\01\00\00\e8\01\00\00\ed\02\00\00\t\03\00\00%\03\00\00e\03\00\00\81\03\00\00\9d\03\00\00\dd\03\00\00\f9\03\00\00\15\04\00\00\9f\04\00\00\bb\04\00\00\d7\04\00\00\17\05\00\003\05\00\00O\05\00\00\8f\05\00\00\ab\05\00\00\c7\05\00\00\07\06\00\00#\06\00\00?\06\00\00W\07\00\00s\07\00\00\8f\07\00\00\cf\07\00\00\eb\07\00\00\07\08\00\00G\08\00\00c\08\00\00\7f\08\00\00\bf\08\00\00\db\08\00\00\f7\08\00\00\a3\t\00\00\bf\t\00\00\db\t\00\00\cf\n\00\00\eb\n\00\00\07\0b\00\00G\0b\00\00c\0b\00\00\7f\0b\00\00\bf\0b\00\00\db\0b\00\00\f7\0b\00\007\0c\00\00S\0c\00\00o\0c\00\00?\0d\00\00[\0d\00\00w\0d\00\00\b7\0d\00\00\d3\0d\00\00\ef\0d\00\00/\0e\00\00K\0e\00\00g\0e\00\00\ef\0e\00\00\13\0f\00\008\00\00\00\f3\0f\00\00?\00\00\00\ef\10\00\00\01\00\00\00\18\00\00\00")
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
    (call $pr_fn
     (i32.const 0)
     (call $new_lval_list
      (call $list_cons
       (i32.add
        (global.get $data_offset)
        (i32.const 31)
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
          (i32.const 67)
         )
         (call $list_cons
          (call $mul_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 103)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 139)
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
                    (i32.const 175)
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
                (i32.const 216)
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
                (i32.const 532)
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
                (i32.const 578)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 175)
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
                 (i32.const 619)
                )
                (call $list_cons
                 (call $add_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 67)
                    )
                    (call $list_cons
                     (call $mul_fn
                      (i32.const 0)
                      (call $new_lval_list
                       (call $list_cons
                        (i32.add
                         (global.get $data_offset)
                         (i32.const 103)
                        )
                        (call $list_cons
                         (i32.add
                          (global.get $data_offset)
                          (i32.const 139)
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
            (i32.const 655)
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
             (i32.const 67)
            )
            (call $list_cons
             (call $mul_fn
              (i32.const 0)
              (call $new_lval_list
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 103)
                )
                (call $list_cons
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 139)
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
           (i32.const 139)
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
                    (i32.const 691)
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
                (i32.const 216)
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
                (i32.const 1089)
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
                (i32.const 578)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 691)
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
                 (i32.const 619)
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
                        (i32.const 67)
                       )
                       (call $list_cons
                        (call $mul_fn
                         (i32.const 0)
                         (call $new_lval_list
                          (call $list_cons
                           (i32.add
                            (global.get $data_offset)
                            (i32.const 103)
                           )
                           (call $list_cons
                            (i32.add
                             (global.get $data_offset)
                             (i32.const 139)
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
                      (i32.const 139)
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
            (i32.const 655)
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
                (i32.const 67)
               )
               (call $list_cons
                (call $mul_fn
                 (i32.const 0)
                 (call $new_lval_list
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 103)
                   )
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 139)
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
              (i32.const 139)
             )
             (i32.const 0)
            )
           )
          )
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 1125)
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
                    (i32.const 103)
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
                (i32.const 216)
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
                (i32.const 1643)
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
                (i32.const 578)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 103)
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
                 (i32.const 619)
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
                           (i32.const 67)
                          )
                          (call $list_cons
                           (call $mul_fn
                            (i32.const 0)
                            (call $new_lval_list
                             (call $list_cons
                              (i32.add
                               (global.get $data_offset)
                               (i32.const 103)
                              )
                              (call $list_cons
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 139)
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
                         (i32.const 139)
                        )
                        (i32.const 0)
                       )
                      )
                     )
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 1125)
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
            (i32.const 655)
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
                (i32.const 1679)
               )
               (call $list_cons
                (call $mul_fn
                 (i32.const 0)
                 (call $new_lval_list
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 1715)
                   )
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 1751)
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
              (i32.const 1787)
             )
             (i32.const 0)
            )
           )
          )
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 1823)
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
                    (i32.const 1859)
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
                (i32.const 216)
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
                (i32.const 2339)
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
                (i32.const 578)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 1859)
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
                 (i32.const 619)
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
                           (i32.const 1679)
                          )
                          (call $list_cons
                           (call $mul_fn
                            (i32.const 0)
                            (call $new_lval_list
                             (call $list_cons
                              (i32.add
                               (global.get $data_offset)
                               (i32.const 1715)
                              )
                              (call $list_cons
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 1751)
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
                         (i32.const 1787)
                        )
                        (i32.const 0)
                       )
                      )
                     )
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 1823)
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
            (i32.const 655)
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
          (i32.const 2375)
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 2411)
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
                    (i32.const 2447)
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
                (i32.const 216)
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
                (i32.const 2567)
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
                (i32.const 578)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 2447)
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
                 (i32.const 619)
                )
                (call $list_cons
                 (call $mul_fn
                  (i32.const 0)
                  (call $new_lval_list
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2375)
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 2411)
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
            (i32.const 655)
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
                (i32.const 2603)
               )
               (call $list_cons
                (call $mul_fn
                 (i32.const 0)
                 (call $new_lval_list
                  (call $list_cons
                   (i32.add
                    (global.get $data_offset)
                    (i32.const 2639)
                   )
                   (call $list_cons
                    (i32.add
                     (global.get $data_offset)
                     (i32.const 2675)
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
              (i32.const 2711)
             )
             (i32.const 0)
            )
           )
          )
         )
         (call $list_cons
          (i32.add
           (global.get $data_offset)
           (i32.const 1823)
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
                    (i32.const 2747)
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
                (i32.const 216)
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
                (i32.const 3227)
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
                (i32.const 578)
               )
               (call $list_cons
                (i32.add
                 (global.get $data_offset)
                 (i32.const 2747)
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
                 (i32.const 619)
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
                           (i32.const 2603)
                          )
                          (call $list_cons
                           (call $mul_fn
                            (i32.const 0)
                            (call $new_lval_list
                             (call $list_cons
                              (i32.add
                               (global.get $data_offset)
                               (i32.const 2639)
                              )
                              (call $list_cons
                               (i32.add
                                (global.get $data_offset)
                                (i32.const 2675)
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
                         (i32.const 2711)
                        )
                        (i32.const 0)
                       )
                      )
                     )
                    )
                    (call $list_cons
                     (i32.add
                      (global.get $data_offset)
                      (i32.const 1823)
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
            (i32.const 655)
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
       (i32.const 3263)
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
                  (i32.const 3299)
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
              (i32.const 216)
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
              (i32.const 3335)
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
              (i32.const 578)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 3299)
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
               (i32.const 619)
              )
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 3263)
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
          (i32.const 655)
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
      (call $new_lval_vector
       (call $list_cons
        (i32.add
         (global.get $data_offset)
         (i32.const 3371)
        )
        (call $list_cons
         (i32.add
          (global.get $data_offset)
          (i32.const 103)
         )
         (call $list_cons
          (call $add_fn
           (i32.const 0)
           (call $new_lval_list
            (call $list_cons
             (i32.add
              (global.get $data_offset)
              (i32.const 3371)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 103)
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
                  (i32.const 3491)
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
              (i32.const 216)
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
              (i32.const 3731)
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
              (i32.const 578)
             )
             (call $list_cons
              (i32.add
               (global.get $data_offset)
               (i32.const 3491)
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
               (i32.const 619)
              )
              (call $list_cons
               (call $new_lval_vector
                (call $list_cons
                 (i32.add
                  (global.get $data_offset)
                  (i32.const 3371)
                 )
                 (call $list_cons
                  (i32.add
                   (global.get $data_offset)
                   (i32.const 103)
                  )
                  (call $list_cons
                   (call $add_fn
                    (i32.const 0)
                    (call $new_lval_list
                     (call $list_cons
                      (i32.add
                       (global.get $data_offset)
                       (i32.const 3371)
                      )
                      (call $list_cons
                       (i32.add
                        (global.get $data_offset)
                        (i32.const 103)
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
          (i32.const 655)
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
       (call $new_lval_vector
        (i32.const 0)
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
                   (i32.const 3767)
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
               (i32.const 216)
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
               (i32.const 3803)
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
               (i32.const 578)
              )
              (call $list_cons
               (i32.add
                (global.get $data_offset)
                (i32.const 3767)
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
                (i32.const 619)
               )
               (call $list_cons
                (call $new_lval_vector
                 (i32.const 0)
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
           (i32.const 655)
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
 ;; custom section "data_size", size 4, contents: "4367"
 ;; custom section "fn_table_size", size 1, contents: "1"
)
