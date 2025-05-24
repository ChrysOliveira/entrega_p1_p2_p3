## Uso

- Para gerar os binários:  
  `make`
- Para remover os binários:  
  `make clean`
- Para realizar uma execução completa:  
  `make run`
- Para executar cada programa individualmente:  
  `./programa <arquivo de entrada>`

---

## Limitações do Projeto

- Operações de **divisão** e **subtração** ainda **não estão implementadas**
- Operação de **multiplicação** está implementada, porém com bug
- As operações aritméticas funcionam **apenas com variáveis**.  
  Por exemplo: a operação `A+B` é permitida, mas `A+1` não.
- Limite de **64 variáveis**
- Apenas **variáveis com 1 caractere** (limitação do assembler)
- Não é permitido usar variáveis com o nome **"R"** (reservada para subrotinas)
- Não é permitido usar variáveis com o nome **"J"** (reservada para subrotinas)

---

## Explicações

O bug na operação de multiplicação e a restrição quanto ao uso dos nomes "R" e "J" para variáveis surgiram durante a tentativa de implementar subrotinas para as operações de multiplicação, subtração (via complemento de dois) e divisão.  
A ideia era incluir a subrotina de cada operação no assembly gerado pelo compilador, utilizando saltos (`JMP`) estratégicos para executar essas operações. O fluxo principal do assembly armazenaria os operandos em variáveis auxiliares, salvaria o endereço da próxima instrução em um salto específico, e chamaria a subrotina da operação desejada. Após a execução da operação, um novo salto retornaria a execução para a instrução correta no fluxo principal do programa.

Encontrei dificuldades para ajustar todos os detalhes dessa estratégia e não consegui finalizar a tempo da primeira entrega (P1).  
Analisando o assembly e o binário gerados pelo compilador, é possível visualizar as variáveis auxiliares e a subrotina (atualmente com bug).
