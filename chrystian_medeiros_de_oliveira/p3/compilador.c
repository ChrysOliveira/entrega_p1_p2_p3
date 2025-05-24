#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE 1024
#define MAX_TERMS 100

typedef struct {
  int sign;              // +1 for positive, -1 for negative
  int is_multiplication; // 1 for A*B term, 0 for single value
  int multiplier;        // First factor in multiplication (A)
  int multiplicand;      // Second factor in multiplication (B)
  int value;             // Single value for non-multiplication terms
} ExpressionTerm;

// Removes all whitespace from the input string
void remove_whitespace(char *str) {
  char *source = str;
  char *destination = str;
  while (*source) {
    if (!isspace((unsigned char)*source)) {
      *destination++ = *source;
    }
    source++;
  }
  *destination = '\0';
}

int main() {
  char input_line[MAX_INPUT_LINE];
  if (!fgets(input_line, sizeof(input_line), stdin)) {
    return 0;
  }

  // Remove trailing newline
  char *newline = strchr(input_line, '\n');
  if (newline) {
    *newline = '\0';
  }

  // Split input into variable name and expression
  char *equals_sign = strchr(input_line, '=');
  if (!equals_sign) {
    return 1;
  }

  // Extract variable name
  int variable_name_length = equals_sign - input_line;
  char variable_name[MAX_INPUT_LINE];
  strncpy(variable_name, input_line, variable_name_length);
  variable_name[variable_name_length] = '\0';
  remove_whitespace(variable_name);
  variable_name_length = (int)strlen(variable_name);

  // Extract expression
  char expression[MAX_INPUT_LINE];
  strcpy(expression, equals_sign + 1);
  remove_whitespace(expression);

  // Parse expression into terms
  ExpressionTerm terms[MAX_TERMS];
  int term_count = 0;
  char *current_pos = expression;
  int current_sign = 1;

  while (*current_pos && term_count < MAX_TERMS) {
    // Handle sign
    if (*current_pos == '+') {
      current_sign = 1;
      current_pos++;
      continue;
    }
    if (*current_pos == '-') {
      current_sign = 0;
      current_pos++;
      continue;
    }

    // Extract term
    char term_str[MAX_INPUT_LINE];
    char *term_end = current_pos;
    while (*term_end && *term_end != '+' && *term_end != '-') {
      term_end++;
    }
    int term_length = term_end - current_pos;
    strncpy(term_str, current_pos, term_length);
    term_str[term_length] = '\0';
    remove_whitespace(term_str);

    // Parse multiplication or single value
    char *multiplication = strchr(term_str, '*');
    if (multiplication) {
      *multiplication = '\0';
      char left_operand[MAX_INPUT_LINE];
      char right_operand[MAX_INPUT_LINE];
      strcpy(left_operand, term_str);
      strcpy(right_operand, multiplication + 1);
      remove_whitespace(left_operand);
      remove_whitespace(right_operand);

      terms[term_count].sign = current_sign;
      terms[term_count].is_multiplication = 1;
      terms[term_count].multiplier = atoi(left_operand);
      terms[term_count].multiplicand = atoi(right_operand);
    } else {
      terms[term_count].sign = current_sign;
      terms[term_count].is_multiplication = 0;
      terms[term_count].value = atoi(term_str);
    }
    term_count++;
    current_pos = term_end;
  }

  // Generate Brainfuck code for variable name
  for (int i = 0; i < variable_name_length; i++) {
    unsigned char current_char = (unsigned char)variable_name[i];
    printf(">");
    for (int j = 0; j < current_char; j++) {
      printf("+");
    }
    printf(".");
  }

  // Generate Brainfuck code for equals sign (ASCII 61)
  printf(">");
  for (int i = 0; i < 61; i++) {
    printf("+");
  }
  printf(".");

  // Move back to cell 0
  for (int i = 0; i < variable_name_length + 1; i++) {
    printf("<");
  }

  // Clear cell 0
  printf("[-]");

  // Generate Brainfuck code for expression terms
  for (int i = 0; i < term_count; i++) {
    ExpressionTerm *term = &terms[i];
    if (term->is_multiplication) {
      int multiplier = term->multiplier;
      int multiplicand = term->multiplicand;

      // Move to cell1 and clear
      printf(">");
      printf("[-]");

      // Set multiplicand in cell1
      for (int j = 0; j < multiplicand; j++) {
        printf("+");
      }

      // Generate loop for multiplication
      if (term->sign) {
        printf("[<");
        for (int j = 0; j < multiplier; j++) {
          printf("+");
        }
        printf(">-]");
      } else {
        printf("[<");
        for (int j = 0; j < multiplier; j++) {
          printf("-");
        }
        printf(">-]");
      }
      printf("<"); // Return to cell0
    } else {
      int value = abs(term->value);
      if (term->sign) {
        for (int j = 0; j < value; j++) {
          printf("+");
        }
      } else {
        for (int j = 0; j < value; j++) {
          printf("-");
        }
      }
    }
  }

  return 0;
}
