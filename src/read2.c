#include <errno.h>

#include "lib.h"
#include "lval.h"

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

Lval* lval_read_sym(char* s, int* i) {
  /* Allocate Empty String */
  char* part = calloc(1, 1);

  /* While valid identifier characters */
  while (_strchr("abcdefghijklmnopqrstuvwxyz"
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "0123456789_+-*\\/=<>!&",
                 s[*i]) &&
         s[*i] != '\0') {
    /* Append character to end of string */
    part = realloc(part, _strlen(part) + 2);
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
    errno = 0;
    long v = strtol(part, NULL, 10);
    x = (errno != ERANGE) ? make_lval_num(v)
                          : make_lval_err("Invalid Number %s", part);
  } else {
    x = make_lval_sym(part);
  }

  /* Free temp string */
  free(part);

  /* Return lval */
  return x;
}
Lval* lval_read_str(char* s, int* i) {
  /* Allocate empty string */
  char* part = calloc(1, 1);

  /* More forward one step past initial " character */
  (*i)++;
  while (s[*i] != '"') {
    char c = s[*i];

    /* If end of input then there is an unterminated string literal */
    if (c == '\0') {
      free(part);
      return make_lval_err("Unexpected end of input");
    }

    /* If backslash then unescape character after it */
    if (c == '\\') {
      (*i)++;
      /* Check next character is escapable */
      if (_strchr(lval_str_unescapable, s[*i])) {
        c = lval_str_unescape(s[*i]);
      } else {
        free(part);
        return make_lval_err("Invalid escape sequence \\%c", s[*i]);
      }
    }

    /* Append character to string */
    part = realloc(part, _strlen(part) + 2);
    part[_strlen(part) + 1] = '\0';
    part[_strlen(part) + 0] = c;
    (*i)++;
  }
  /* Move forward past final " character */
  (*i)++;

  Lval* x = make_lval_str(part);

  /* free temp string */
  free(part);

  return x;
}

Lval* lval_read(char* s, int* i);

Lval* lval_read_expr(char* s, int* i, char end) {
  /* Either create new qexpr or sexpr */
  Lval* x = (end == ']') ? make_lval_vector() : make_lval_list();

  /* While not at end character keep reading lvals */
  while (s[*i] != end) {
    Lval* y = lval_read(s, i);
    /* If an error then return this and stop */
    if (y->type == LVAL_ERR) {
      lval_del(x);
      return y;
    } else {
      lval_add_child(x, y);
    }
  }

  /* Move past end character */
  (*i)++;

  return x;
}

Lval* lval_read(char* s, int* i) {
  /* Skip all trailing whitespace and comments */
  while (_strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
    if (s[*i] == ';') {
      while (s[*i] != '\n' && s[*i] != '\0') {
        (*i)++;
      }
    }
    (*i)++;
  }

  Lval* x = NIL;

  char next_char = s[*i];

  /* If we reach end of input then we're missing something */
  if (next_char == '\0') {
    return make_lval_err("Unexpected end of input");
  }

  /* If next character is ( then read list */
  else if (next_char == '(') {
    (*i)++;
    x = lval_read_expr(s, i, ')');
  }

  /* If next character is { then read vector */
  else if (s[*i] == '[') {
    (*i)++;
    x = lval_read_expr(s, i, ']');
  }

  /* If next character is { then read vector */
  else if (s[*i] == '{') {
    (*i)++;
    x = lval_read_expr(s, i, '}');
  }

  /* If next character is part of a symbol then read symbol */
  else if (_strchr("abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "0123456789_+-*\\/=<>!&",
                   s[*i])) {
    x = lval_read_sym(s, i);
  }

  /* If next character is " then read string */
  else if (_strchr("\"", s[*i])) {
    x = lval_read_str(s, i);
  }

  /* Encountered some unexpected character */
  else {
    x = make_lval_err("Unexpected character %c", s[*i]);
  }

  /* Skip all trailing whitespace and comments */
  while (_strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
    if (s[*i] == ';') {
      while (s[*i] != '\n' && s[*i] != '\0') {
        (*i)++;
      }
    }
    (*i)++;
  }

  return x;
}
