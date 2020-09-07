#ifndef __NAMESPACE_H_
#define __NAMESPACE_H_

#include "env.h"

Namespace* make_or_get_namespace(char* namespace_str);
char* ns_to_file_name(char* ns_str);

int is_ns_env(Lenv* env);

Namespace* get_current_ns();
void set_current_ns(Namespace* ns);
Namespace* get_namespace(char* namespace_str);

Namespace* get_ns_for_namespaced_symbol(Lval* lval_symbol,
                                        char* namespace_or_alias);
Namespace* get_ns_for_referred_symbol(Lval* lval_symbol);

Namespace* get_stdlib_ns();

Lenv* get_stdlib_env();

Lval* make_lval_ns_symbol();

void install_builtins();
Namespace* install_ns(char* str);
Namespace* install_stdlib();
#endif  // __NAMESPACE_H_
