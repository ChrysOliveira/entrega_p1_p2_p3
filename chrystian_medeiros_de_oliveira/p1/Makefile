# Lista todos os arquivos .c
SRC := $(wildcard *.c)

# Gera nomes dos executáveis removendo .c
BIN := $(SRC:.c=)

# Compilador e flags
CC := gcc
CFLAGS := 

# Alvo padrão: compilar todos os .c
all: $(BIN)

# Regra genérica para compilar
%: %.c
	$(CC) $(CFLAGS) -o $@ $<

run:
	./compiler ./programa.lpn ; ./assembler ./programa.asm ; ./executor ./programa.bin

# Alvo para limpar os executáveis
clean:
	rm -f $(BIN)
