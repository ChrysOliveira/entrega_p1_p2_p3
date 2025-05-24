#include <stdio.h>

#define MEMORY_SIZE 30000
#define MAX_CODE_LENGTH 100000
#define MAX_NESTED_LOOPS 100

// Executes the given Brainfuck code
int run_brainfuck_code(const char *program_code) {
  unsigned char data_memory[MEMORY_SIZE] = {0};
  int memory_pointer = 0;
  int code_position = 0;
  int loop_stack[MAX_NESTED_LOOPS];
  int loop_stack_index = 0;

  while (program_code[code_position] != '\0') {
    switch (program_code[code_position]) {
    case '>':
      memory_pointer++;
      if (memory_pointer >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Memory pointer out of bounds (forward)\n");
        return 1;
      }
      break;

    case '<':
      memory_pointer--;
      if (memory_pointer < 0) {
        fprintf(stderr, "Error: Memory pointer out of bounds (backward)\n");
        return 1;
      }
      break;

    case '+':
      data_memory[memory_pointer]++;
      break;

    case '-':
      data_memory[memory_pointer]--;
      break;

    case '.':
      putchar(data_memory[memory_pointer]);
      break;

    case ',':
      data_memory[memory_pointer] = getchar();
      break;

    case '[':
      if (data_memory[memory_pointer] == 0) {
        // Skip to matching closing bracket
        int nesting_depth = 1;
        while (nesting_depth > 0) {
          code_position++;
          if (program_code[code_position] == '[') {
            nesting_depth++;
          } else if (program_code[code_position] == ']') {
            nesting_depth--;
          }

          if (program_code[code_position] == '\0') {
            fprintf(stderr, "Error: Unclosed loop\n");
            return 1;
          }
        }
      } else {
        if (loop_stack_index >= MAX_NESTED_LOOPS) {
          fprintf(stderr, "Error: Too many nested loops\n");
          return 1;
        }
        loop_stack[loop_stack_index++] = code_position;
      }
      break;

    case ']':
      if (loop_stack_index <= 0) {
        fprintf(stderr, "Error: ']' without matching '['\n");
        return 1;
      }

      if (data_memory[memory_pointer] != 0) {
        code_position = loop_stack[loop_stack_index - 1];
      } else {
        loop_stack_index--;
      }
      break;
    }

    code_position++;
  }

  int final_result = data_memory[0];
  printf("%d\n", final_result);

  return 0;
}

int main() {
  char program_code[MAX_CODE_LENGTH];
  int code_index = 0;
  int input_char;

  // Read Brainfuck code from standard input
  while ((input_char = getchar()) != EOF && code_index < MAX_CODE_LENGTH - 1) {
    // Store only valid Brainfuck characters
    if (input_char == '>' || input_char == '<' || input_char == '+' ||
        input_char == '-' || input_char == '.' || input_char == ',' ||
        input_char == '[' || input_char == ']') {
      program_code[code_index++] = input_char;
    }
  }
  program_code[code_index] = '\0';

  // Execute the Brainfuck code
  run_brainfuck_code(program_code);

  return 0;
}
