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

/*
 * reset chip8s memeory and load font starting at 0x50.
 *
 */
void chip8_reset(){
  
  //load 0
  memory[0x50] = 0xF0;
  memory[0x51] = 0x90;
  memory[0x52] = 0x90;
  memory[0x53] = 0x90;
  memory[0x54] = 0xF0;

  //load 1
  memory[0x55] = 0x20;
  memory[0x56] = 0x60;
  memory[0x57] = 0x20;
  memory[0x58] = 0x20;
  memory[0x59] = 0x70;

  //load 2
  memory[0x5A] = 0xF0;
  memory[0x5B] = 0x10;
  memory[0x5C] = 0xF0;
  memory[0x5D] = 0x80;
  memory[0x5E] = 0xf0;

  //load 3
  memory[0x5F] = 0xF0;
  memory[0x60] = 0x10;
  memory[0x61] = 0xF0;
  memory[0x62] = 0x10;
  memory[0x63] = 0xF0;

  //load 4
  memory[0x64] = 0x90;
  memory[0x65] = 0x90;
  memory[0x66] = 0xF0;
  memory[0x67] = 0x10;
  memory[0x68] = 0x10; 

  
}
