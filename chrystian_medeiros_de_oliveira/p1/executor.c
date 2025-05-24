#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void print_memory(uint8_t *bytes, size_t size, uint8_t ac, uint8_t pc, bool z,
                  bool n) {
  size_t offset = 0;

  printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  printf("AC: %03d  | PC: %03d\n", ac, pc);
  printf("Z : %s | N : %s\n", z ? "true" : "false", n ? "true" : "false");

  while (offset < size) {
    printf("%08zx: ", offset);

    for (size_t i = 0; i < 16; i++) {
      if (offset + i < size)
        printf("%02x ", bytes[offset + i]);
      else
        printf("   ");
    }

    printf("\n");
    offset += 16;
  }

  offset += 16;
}

int calcula_posicao(uint8_t hex) {
  uint8_t offset = 4;
  return hex * 2 + offset;
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  uint8_t ac = 0, pc = 0;
  bool z = 0, n = 0;

  const char *file_name = argv[1];
  /* FILE *file = fopen("multiplicacao-entrega.mem", "rb"); */
  FILE *file = fopen(file_name, "rb");
  if (!file) {
    perror("Error opening file");
    return 1;
  }

  uint8_t bytes[516];
  uint8_t file_id[4];
  fread(file_id, 1, 4, file);

  // 0x03 0x4E 0x44 0x52 -> ".NDR"
  const uint8_t expected_id[] = {0x03, 0x4E, 0x44, 0x52};

  if (memcmp(file_id, expected_id, 4) != 0) {
    fprintf(stderr, "Invalid file identifier! Expected 03 4E 44 52\n");
    fclose(file);
    return 1;
  }

  printf("Valid file identifier detected: %02x %02x %02x %02x\n", file_id[0],
         file_id[1], file_id[2], file_id[3]);

  fread(bytes + 4, 1, 512, file);
  fclose(file);

  print_memory(bytes, 516, ac, pc, z, n);

  // 0xF0 = HLT
  while (bytes[pc] != 0xF0) {

    if (ac == 0) {
      z = true;
    } else {
      z = false;
    }

    n = (ac & 0x80) != 0;

    switch (bytes[pc]) {
      // NOP
    case 0x00:
      break;
      // STA
    case 0x10:
      bytes[calcula_posicao(bytes[pc + 2])] = ac;
      break;
      // LDA
    case 0x20:
      ac = bytes[calcula_posicao(bytes[pc + 2])];
      break;
      // ADD
    case 0x30:
      ac += bytes[calcula_posicao(bytes[pc + 2])];
      break;
      // OR
    case 0x40:
      ac = bytes[calcula_posicao(bytes[pc + 2])] | ac;
      break;
      // AND
    case 0x50:
      ac = bytes[calcula_posicao(bytes[pc + 2])] & ac;
      break;
      // NOT
    case 0x60:
      ac = ~ac;
      pc += 2;
      continue;
      break;
      // JMP
    case 0x80:
      pc = calcula_posicao(bytes[pc + 2]);
      continue;
      break;
      // JN
    case 0x90:
      if (n) {
        pc = calcula_posicao(bytes[pc + 2]);
        continue;
      }
      break;
      // JZ
    case 0xA0:
      if (z) {
        pc = calcula_posicao(bytes[pc + 2]);
        continue;
      }
      break;
    }

    pc += 4;
    print_memory(bytes, 516, ac, pc, z, n);
  }

  print_memory(bytes, 516, ac, pc, z, n);

  return 0;
}
