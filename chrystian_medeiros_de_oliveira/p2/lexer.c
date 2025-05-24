#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 2048
#define MAX_TOKEN_LEN 128
#define MAX_FILE_SIZE 16384

typedef enum {
  // Palavras-chave
  TOKEN_PROGRAM,
  TOKEN_BEGIN,
  TOKEN_END,
  TOKEN_IF,
  TOKEN_ELIF,
  TOKEN_WHILE,
  TOKEN_FUNC,
  TOKEN_VOID,
  TOKEN_INT,
  TOKEN_FLOAT,
  TOKEN_CHAR,
  TOKEN_RETURN,
  // Símbolos
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_COLON,
  TOKEN_COMMA,
  TOKEN_SEMICOLON,
  // Operadores
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_MULT,
  TOKEN_DIV,
  TOKEN_ASSIGN, // '='
  TOKEN_EQ,
  TOKEN_NEQ,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LEQ,
  TOKEN_GEQ,
  // Literais
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_STRING,
  // Diversos
  TOKEN_NEWLINE,
  TOKEN_EOF,
  TOKEN_UNKNOWN
} TokenType;

typedef struct {
  TokenType type;
  char value[MAX_TOKEN_LEN];
  int line;
  int col;
} Token;

const char *keywords[] = {"PROGRAMA", "INICIO", "FIM",  "if",
                          "elif",     "while",  "func", "void",
                          "int",      "float",  "char", "return"};
TokenType keyword_types[] = {TOKEN_PROGRAM, TOKEN_BEGIN, TOKEN_END,
                             TOKEN_IF,      TOKEN_ELIF,  TOKEN_WHILE,
                             TOKEN_FUNC,    TOKEN_VOID,  TOKEN_INT,
                             TOKEN_FLOAT,   TOKEN_CHAR,  TOKEN_RETURN};
const int KEYWORDS_COUNT = sizeof(keywords) / sizeof(keywords[0]);

int is_keyword(const char *str) {
  for (int i = 0; i < KEYWORDS_COUNT; i++) {
    if (strcmp(str, keywords[i]) == 0)
      return keyword_types[i];
  }
  return 0;
}

int tokenize(const char *input, Token *tokens, int max_tokens) {
  int pos = 0, line = 1, col = 1, token_count = 0;
  while (input[pos] && token_count < max_tokens) {
    // Espaços e tabs
    if (input[pos] == ' ' || input[pos] == '\t') {
      col++;
      pos++;
      continue;
    }
    // Nova linha
    if (input[pos] == '\n') {
      tokens[token_count++] = (Token){TOKEN_NEWLINE, "\\n", line, col};
      pos++;
      line++;
      col = 1;
      continue;
    }
    // String entre aspas
    if (input[pos] == '"') {
      int start = ++pos, len = 0;
      while (input[pos] && input[pos] != '"' && len < MAX_TOKEN_LEN - 1) {
        pos++;
        len++;
      }
      if (input[pos] == '"') {
        tokens[token_count].type = TOKEN_STRING;
        strncpy(tokens[token_count].value, input + start, len);
        tokens[token_count].value[len] = '\0';
        tokens[token_count].line = line;
        tokens[token_count].col = col;
        token_count++;
        pos++;
        col += len + 2;
        continue;
      } else {
        fprintf(stderr, "Unterminated string at line %d\n", line);
        break;
      }
    }
    // Identificadores ou palavras-chave
    if (isalpha((unsigned char)input[pos]) || input[pos] == '_') {
      int start = pos, len = 0;
      while ((isalnum((unsigned char)input[pos]) || input[pos] == '_') &&
             len < MAX_TOKEN_LEN - 1) {
        pos++;
        len++;
      }
      char tmp[MAX_TOKEN_LEN];
      strncpy(tmp, input + start, len);
      tmp[len] = '\0';
      int kw = is_keyword(tmp);
      tokens[token_count].type = kw ? kw : TOKEN_IDENTIFIER;
      strcpy(tokens[token_count].value, tmp);
      tokens[token_count].line = line;
      tokens[token_count].col = col;
      token_count++;
      col += len;
      continue;
    }
    // Números (inteiros e floats)
    if (isdigit((unsigned char)input[pos]) ||
        (input[pos] == '-' && isdigit((unsigned char)input[pos + 1]))) {
      int start = pos, len = 0, has_dot = 0, has_exp = 0;
      if (input[pos] == '-') {
        pos++;
        len++;
      }
      while (isdigit((unsigned char)input[pos])) {
        pos++;
        len++;
      }
      if (input[pos] == '.') {
        has_dot = 1;
        pos++;
        len++;
        while (isdigit((unsigned char)input[pos])) {
          pos++;
          len++;
        }
      }
      if (input[pos] == 'e' || input[pos] == 'E') {
        has_exp = 1;
        pos++;
        len++;
        if (input[pos] == '-') {
          pos++;
          len++;
        }
        while (isdigit((unsigned char)input[pos])) {
          pos++;
          len++;
        }
      }
      strncpy(tokens[token_count].value, input + start, len);
      tokens[token_count].value[len] = '\0';
      tokens[token_count].type = TOKEN_NUMBER;
      tokens[token_count].line = line;
      tokens[token_count].col = col;
      token_count++;
      col += len;
      continue;
    }
    // Operadores relacionais de 2 caracteres
    if ((input[pos] == '=' && input[pos + 1] == '=') ||
        (input[pos] == '!' && input[pos + 1] == '=') ||
        (input[pos] == '<' && input[pos + 1] == '=') ||
        (input[pos] == '>' && input[pos + 1] == '=')) {
      TokenType ttype = TOKEN_EQ;
      if (input[pos] == '!' && input[pos + 1] == '=')
        ttype = TOKEN_NEQ;
      else if (input[pos] == '<' && input[pos + 1] == '=')
        ttype = TOKEN_LEQ;
      else if (input[pos] == '>' && input[pos + 1] == '=')
        ttype = TOKEN_GEQ;
      tokens[token_count++] = (Token){ttype, "", line, col};
      tokens[token_count - 1].value[0] = input[pos];
      tokens[token_count - 1].value[1] = input[pos + 1];
      tokens[token_count - 1].value[2] = '\0';
      pos += 2;
      col += 2;
      continue;
    }
    // Operadores e símbolos de 1 caractere
    switch (input[pos]) {
    case '+':
      tokens[token_count++] = (Token){TOKEN_PLUS, "+", line, col};
      break;
    case '-':
      tokens[token_count++] = (Token){TOKEN_MINUS, "-", line, col};
      break;
    case '*':
      tokens[token_count++] = (Token){TOKEN_MULT, "*", line, col};
      break;
    case '/':
      tokens[token_count++] = (Token){TOKEN_DIV, "/", line, col};
      break;
    case '=':
      tokens[token_count++] = (Token){TOKEN_ASSIGN, "=", line, col};
      break;
    case '<':
      tokens[token_count++] = (Token){TOKEN_LT, "<", line, col};
      break;
    case '>':
      tokens[token_count++] = (Token){TOKEN_GT, ">", line, col};
      break;
    case '{':
      tokens[token_count++] = (Token){TOKEN_LBRACE, "{", line, col};
      break;
    case '}':
      tokens[token_count++] = (Token){TOKEN_RBRACE, "}", line, col};
      break;
    case '(':
      tokens[token_count++] = (Token){TOKEN_LPAREN, "(", line, col};
      break;
    case ')':
      tokens[token_count++] = (Token){TOKEN_RPAREN, ")", line, col};
      break;
    case ':':
      tokens[token_count++] = (Token){TOKEN_COLON, ":", line, col};
      break;
    case ',':
      tokens[token_count++] = (Token){TOKEN_COMMA, ",", line, col};
      break;
    default: // Caracteres desconhecidos
      tokens[token_count++] = (Token){TOKEN_UNKNOWN, "", line, col};
      tokens[token_count - 1].value[0] = input[pos];
      tokens[token_count - 1].value[1] = '\0';
      break;
    }
    pos++;
    col++;
  }
  tokens[token_count] = (Token){TOKEN_EOF, "", line, col};
  return token_count + 1;
}

const char *token_type_str(TokenType type) {
  switch (type) {
  case TOKEN_PROGRAM:
    return "PROGRAM";
  case TOKEN_BEGIN:
    return "BEGIN";
  case TOKEN_END:
    return "END";
  case TOKEN_IF:
    return "IF";
  case TOKEN_ELIF:
    return "ELIF";
  case TOKEN_WHILE:
    return "WHILE";
  case TOKEN_FUNC:
    return "FUNC";
  case TOKEN_VOID:
    return "VOID";
  case TOKEN_INT:
    return "INT";
  case TOKEN_FLOAT:
    return "FLOAT";
  case TOKEN_CHAR:
    return "CHAR";
  case TOKEN_RETURN:
    return "RETURN";
  case TOKEN_LBRACE:
    return "LBRACE";
  case TOKEN_RBRACE:
    return "RBRACE";
  case TOKEN_LPAREN:
    return "LPAREN";
  case TOKEN_RPAREN:
    return "RPAREN";
  case TOKEN_COLON:
    return "COLON";
  case TOKEN_COMMA:
    return "COMMA";
  case TOKEN_SEMICOLON:
    return "SEMICOLON";
  case TOKEN_PLUS:
    return "PLUS";
  case TOKEN_MINUS:
    return "MINUS";
  case TOKEN_MULT:
    return "MULT";
  case TOKEN_DIV:
    return "DIV";
  case TOKEN_ASSIGN:
    return "ASSIGN";
  case TOKEN_EQ:
    return "EQ";
  case TOKEN_NEQ:
    return "NEQ";
  case TOKEN_LT:
    return "LT";
  case TOKEN_GT:
    return "GT";
  case TOKEN_LEQ:
    return "LEQ";
  case TOKEN_GEQ:
    return "GEQ";
  case TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case TOKEN_NUMBER:
    return "NUMBER";
  case TOKEN_STRING:
    return "STRING";
  case TOKEN_NEWLINE:
    return "NEWLINE";
  case TOKEN_EOF:
    return "EOF";
  case TOKEN_UNKNOWN:
    return "UNKNOWN";
  default:
    return "???";
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }
  FILE *f = fopen(argv[1], "r");
  if (!f) {
    perror("Error opening file");
    return 1;
  }
  char buffer[MAX_FILE_SIZE];
  size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
  fclose(f);
  buffer[n] = '\0';

  Token tokens[MAX_TOKENS];
  int num_tokens = tokenize(buffer, tokens, MAX_TOKENS);

  printf("Tokens:\n");
  for (int i = 0; i < num_tokens; i++) {
    printf("%3d [%s] (%d,%d): \"%s\"\n", i, token_type_str(tokens[i].type),
           tokens[i].line, tokens[i].col, tokens[i].value);
    if (tokens[i].type == TOKEN_EOF)
      break;
  }
  return 0;
}
