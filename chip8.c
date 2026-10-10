#include <stdio.h>
#include <stdint.h>

//create struct for chip8 state (memory suff)
typedef struct chip_8 {
  uint8_t memory[4096];//4 kilobytes
  uint8_t V[16];//general purpose vars 
  uint16_t I;//used to point a locations in memory
  uint16_t PC;//program counter
  uint16_t stack[16];
  uint8_t sp;//stack pointer 
  uint8_t dt;//delay timer
  uint8_t st;//sound timer
  uint8_t display[64][32];
} Chip8; 
