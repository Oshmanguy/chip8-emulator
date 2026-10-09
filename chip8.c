#include <stdio.h>
#include <stdint.h>

//create struct for chip8 state (memory suff)
typedef struct chip_8 {
  int8_t memory[4096];//4 kilobytes
  int8_t V[16];//general purpose vars 
  int16_t I;//used to point a locations in memory
  int16_t PC;//program counter
  int16_t stack[16];
  int8_t sp;//stack pointer 
  int8_t dt;//delay timer
  int8_t st;//sound timer
  int8_t display[64][32];
} Chip8; 
