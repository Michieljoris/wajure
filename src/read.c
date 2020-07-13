/* #include <errno.h> */

#include "cell.h"
#include "hash.h"
#include "lib.h"
#include "lispy_mempool.h"
#include "lval.h"
#include "print.h"

/* Possible unescapable characters */
char* lval_str_unescapable = "abfnrtv\\\'\"";

/* List of possible escapable characters */
char* lval_str_escapable = "\a\b\f\n\r\t\v\\\'\"";

/* Function to unescape characters */
char lval_str_unescape(char x) {
  switch (x) {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case '\\':
      return '\\';
    case '\'':
      return '\'';
    case '\"':
      return '\"';
  }
  return '\0';
}

/* Function to escape characters */
char* lval_str_escape(char x) {
  switch (x) {
    case '\a':
      return "\\a";
    case '\b':
      return "\\b";
    case '\f':
      return "\\f";
    case '\n':
      return "\\n";
    case '\r':
      return "\\r";
    case '\t':
      return "\\t";
    case '\v':
      return "\\v";
    case '\\':
      return "\\\\";
    case '\'':
      return "\\\'";
    case '\"':
      return "\\\"";
  }
  return "";
}

char* valid_symbol_chars =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789_+-*\\/=<>!&?";

Lval* lval_read_sym(char* s, int* i) {
  /* Allocate Empty String */
  char* part = lalloc_size(1);

  /* While valid identifier characters */
  while (_strchr(valid_symbol_chars, s[*i]) && s[*i] != '\0') {
    /* Append character to end of string */
    part = lrealloc(part, _strlen(part) + 2);
    part[_strlen(part) + 1] = '\0';
    part[_strlen(part) + 0] = s[*i];
    (*i)++;
  }

  /* Check if Identifier looks like number */
  int is_num = _strchr("-0123456789", part[0]) != NULL;
  for (int j = 1; j < _strlen(part); j++) {
    if (_strchr("0123456789", part[j]) == NULL) {
      is_num = 0;
      break;
    }
  }
  if (_strlen(part) == 1 && part[0] == '-') {
    is_num = 0;
  }

  /* Add Symbol or Number as lval */
  Lval* x = NULL;
  if (is_num) {
    long v = _strtol(part, NULL, 10);
    x = (*merrno != ERANGE)
            ? make_lval_num(v)
            : make_lval_err("Invalid Number, too large or too small %s", part);
  } else if (_strcmp(part, "true") == 0) {
    x = make_lval_true();

  } else if (_strcmp(part, "false") == 0) {
    x = make_lval_false();

  } else if (_strcmp(part, "nil") == 0) {
    x = make_lval_nil();
  } else {
    x = make_lval_sym(part);
  }

  /* Free temp string */
  release(part);

  /* Return lval */
  return x;
}
Lval* lval_read_str(char* s, int* i) {
  /* Allocate empty string */
  char* part = lalloc_size(1);

  /* More forward one step past initial " character */
  (*i)++;
  while (s[*i] != '"') {
    char c = s[*i];

    /* If end of input then there is an unterminated string literal */
    if (c == '\0') {
      release(part);
      return make_lval_err("Unexpected end of input");
    }

    /* If backslash then unescape character after it */
    if (c == '\\') {
      (*i)++;
      /* Check next character is escapable */
      if (_strchr(lval_str_unescapable, s[*i])) {
        c = lval_str_unescape(s[*i]);
      } else {
        release(part);
        return make_lval_err("Invalid escape sequence \\%c", s[*i]);
      }
    }

    /* Append character to string */
    part = lrealloc(part, _strlen(part) + 2);
    part[_strlen(part) + 1] = '\0';
    part[_strlen(part) + 0] = c;
    (*i)++;
  }
  /* Move forward past final " character */
  (*i)++;

  Lval* x = make_lval_str(part);

  /* free temp string */
  release(part);

  return x;
}

Lval* lval_read(char* s, int* i);

void skip_ignored_chars(char* s, int* i) {
  /* Skip all trailing whitespace and comments */
  while (_strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
    if (s[*i] == ';') {
      while (s[*i] != '\n' && s[*i] != '\0') {
        (*i)++;
      }
    }
    (*i)++;
  }
}

Lval* get_next_expr(char* s, int* i, char end) {
  Lval* next_expr = NULL;
  if (s[*i] != end) {
    skip_ignored_chars(s, i);
    if (s[*i] == '\0') return NULL;
    next_expr = lval_read(s, i);
    skip_ignored_chars(s, i);
  }
  return next_expr;
}

Lval* lval_err = NULL;

Cell* read_cell(char* s, int* i, char end) {
  Cell* cons = NULL;
  Lval* expr = get_next_expr(s, i, end);
  if (!expr) return NULL;
  /* If an error then return this and stop */
  if (expr->type == LVAL_ERR) {
    lval_err = expr;
    return NULL;
  }
  cons = make_cell();
  cons->car = expr;
  cons->cdr = read_cell(s, i, end);
  cons->hash = calc_list_hash(cons, cons->cdr);
  return cons;
}

Lval* lval_read_list(char* s, int* i, char end) {
  /* Create list, vector or map */
  Lval* x;
  switch (end) {
    case ']':
      x = make_lval_vector();
      break;
    case '}':
      x = make_lval_map();
      break;
    default:;
      x = make_lval_list();
  }
  lval_err = NULL;
  x->head = read_cell(s, i, end);
  if (lval_err) {
    release(x);
    return lval_err;
  }
  x->hash = x->head ? x->head->hash : x->hash;
  /* Move past end character */
  (*i)++;
  return x;
}

/* Lval* lval_read_list_old(char* s, int* i, char end) { */
/*   /\* Create list, vector or map *\/ */
/*   Lval* x; */
/*   switch (end) { */
/*     case ']': */
/*       x = make_lval_vector(); */
/*       break; */
/*     case '}': */
/*       x = make_lval_map(); */
/*       break; */
/*     default:; */
/*       x = make_lval_list(); */
/*   } */
/*   Cell** lp; */
/*   lp = &x->head; */

/*   /\* While not at end character keep reading lvals *\/ */
/*   while (s[*i] != end) { */
/*     skip_ignored_chars(s, i); */
/*     if (s[*i] == '\0') break; */
/*     Lval* next_expr = lval_read(s, i); */
/*     skip_ignored_chars(s, i); */
/*     /\* If an error then return this and stop *\/ */
/*     if (next_expr->type == LVAL_ERR) { */
/*       release(x); */
/*       return next_expr; */
/*     } else { */
/*       Cell* next_cell = make_cell(); */
/*       next_cell->car = next_expr; */
/*       *lp = next_cell; */
/*       lp = &(next_cell->cdr); */
/*     } */
/*   } */
/*   /\* Move past end character *\/ */
/*   (*i)++; */
/*   return x; */
/* } */

static Lval* reader_macro(char* reader_token, char* lispy_fn, char* s, int* i) {
  (*i)++;
  if (s[*i] == '@') {
    reader_token = "~@";
    lispy_fn = "splice-unquote";
    (*i)++;
  };
  char* lsym = lalloc_size(_strlen(lispy_fn));
  _strcpy(lsym, lispy_fn);
  Lval* next_expression = lval_read(s, i);
  if (!next_expression)
    return make_lval_err("The reader macro \"%s\" has nothing to (un)quote.",
                         reader_token);
  if (next_expression->type == LVAL_ERR) return next_expression;
  Lval* lval = make_lval_list();
  Cell* cell = make_cell();
  lval->head = cell;
  cell->car = make_lval_sym(lsym);
  release(lsym);
  cell->cdr = make_cell();
  cell->cdr->car = next_expression;
  return lval;
}

Lval* lval_read_quote(char* s, int* i) {
  return reader_macro("'", "quote", s, i);
}

Lval* lval_read_backquote(char* s, int* i) {
  return reader_macro("`", "quasiquote", s, i);
}

Lval* lval_read_tilde(char* s, int* i) {
  return reader_macro("~", "unquote", s, i);
}
// TODO: reader doesn't throw a hissy fit when last closing token is missing
// :-(
Lval* lval_read(char* s, int* i) {
  Lval* x = NIL;

  char next_char = s[*i];

  if (_strchr(valid_symbol_chars, next_char)) {
    x = lval_read_sym(s, i);
  } else
    switch (next_char) {
      case '\0':
        return make_lval_err("Unexpected end of input");
      case '(':
        (*i)++;
        x = lval_read_list(s, i, ')');
        break;
      case '[':
        (*i)++;
        x = lval_read_list(s, i, ']');
        x->subtype = VECTOR;
        break;
      case '{':
        (*i)++;
        // TODO: implement maps
        printf("maps are not yet implement\n");
        x = lval_read_list(s, i, '}');
        lval_debugln(x);
        break;
      case '\"':
        x = lval_read_str(s, i);
        break;
      case '\'':
        x = lval_read_quote(s, i);
        break;
      case '`':
        x = lval_read_backquote(s, i);
        break;
      case '~':
        x = lval_read_tilde(s, i);
        break;
      default:
        x = make_lval_err("Unexpected character %c", s[*i]);
    }

  return x;
}
