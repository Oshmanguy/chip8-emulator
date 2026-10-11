#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <err.h>

//define constants
#define MEMORY_SIZE 4096 
#define REGISTER_COUNT 16 
#define STACK_SIZE 16 
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define PROGRAM_START_ADDRESS 0x200

//create struct for chip8 state (memory suff)
typedef struct chip_8 {
  uint8_t memory[MEMORY_SIZE];//4 kilobytes
  uint8_t V[REGISTER_COUNT];//general purpose vars 
  uint16_t I;//used to point a locations in memory
  uint16_t pc;//program counter
  uint16_t stack[STACK_SIZE];
  uint8_t sp;//stack pointer 
  uint8_t dt;//delay timer
  uint8_t st;//sound timer
  uint8_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT];
} Chip8;

/*
 * reset the state of chip8, inizalize font in memory.
 *
 * *chip: a pointer of type Chip8 (struct). 
 *
 */
void chip8_reset(Chip8 *chip){

  //inizalize pointer I
  chip->I = 0;

  //inizalize program counter
  chip->pc = PROGRAM_START_ADDRESS;

  //inizalize stack pointer, chip8 starts reading program at memory address 0x200
  chip->sp = 0;

  //inizalize delay timer
  chip->dt = 0;

  //inizalize sound timer
  chip->st = 0;

 //inizalize registers 
  memset(chip->V, 0, REGISTER_COUNT);

  //(inixalize memory) set all values in memory to 0
  memset(chip->memory, 0, MEMORY_SIZE);

  //inizalize stack
  memset(chip->stack, 0, STACK_SIZE * sizeof(uint16_t));//32bytes total

  //inizalize display, gonna do all zeros for now this might change
  memset(chip->display, 0, sizeof(chip->display));

  //load 0
  chip->memory[0x50] = 0xF0;
  chip->memory[0x51] = 0x90;
  chip->memory[0x52] = 0x90;
  chip->memory[0x53] = 0x90;
  chip->memory[0x54] = 0xF0;

  //load 1
  chip->memory[0x55] = 0x20;
  chip->memory[0x56] = 0x60;
  chip->memory[0x57] = 0x20;
  chip->memory[0x58] = 0x20;
  chip->memory[0x59] = 0x70;

  //load 2
  chip->memory[0x5A] = 0xF0;
  chip->memory[0x5B] = 0x10;
  chip->memory[0x5C] = 0xF0;
  chip->memory[0x5D] = 0x80;
  chip->memory[0x5E] = 0xf0;

  //load 3
  chip->memory[0x5F] = 0xF0;
  chip->memory[0x60] = 0x10;
  chip->memory[0x61] = 0xF0;
  chip->memory[0x62] = 0x10;
  chip->memory[0x63] = 0xF0;

  //load 4
  chip->memory[0x64] = 0x90;
  chip->memory[0x65] = 0x90;
  chip->memory[0x66] = 0xF0;
  chip->memory[0x67] = 0x10;
  chip->memory[0x68] = 0x10; 

  //load 5
  chip->memory[0x69] = 0xF0;
  chip->memory[0x6A] = 0x80;
  chip->memory[0x6B] = 0xF0;
  chip->memory[0x6C] = 0x10;
  chip->memory[0x6D] = 0xF0;

  //load 6
  chip->memory[0x6E] = 0xF0;
  chip->memory[0x6F] = 0x80;
  chip->memory[0x70] = 0xF0;
  chip->memory[0x71] = 0x90;
  chip->memory[0x72] = 0xF0;

  //load 7 
  chip->memory[0x73] = 0xF0;
  chip->memory[0x74] = 0x10;
  chip->memory[0x75] = 0x20;
  chip->memory[0x76] = 0x40;
  chip->memory[0x77] = 0x40;

  //load 8
  chip->memory[0x78] = 0xF0;
  chip->memory[0x79] = 0x90;
  chip->memory[0x7A] = 0xF0;
  chip->memory[0x7B] = 0x90;
  chip->memory[0x7C] = 0xF0;

  //load 9
  chip->memory[0x7D] = 0xF0;
  chip->memory[0x7E] = 0x90;
  chip->memory[0x7F] = 0xF0;
  chip->memory[0x80] = 0x10;
  chip->memory[0x81] = 0xF0;

  //load A
  chip->memory[0x82] = 0xF0;
  chip->memory[0x83] = 0x90;
  chip->memory[0x84] = 0xF0;
  chip->memory[0x85] = 0x90;
  chip->memory[0x86] = 0x90;

  //load B
  chip->memory[0x87] = 0xE0;
  chip->memory[0x88] = 0x90;
  chip->memory[0x89] = 0xE0;
  chip->memory[0x8A] = 0x90;
  chip->memory[0x8B] = 0xE0;

  //load C
  chip->memory[0x8C] = 0xF0;
  chip->memory[0x8D] = 0x80;
  chip->memory[0x8E] = 0x80;
  chip->memory[0x8F] = 0x80;
  chip->memory[0x90] = 0xF0;

  //load D
  chip->memory[0x91] = 0xE0;
  chip->memory[0x92] = 0x90;
  chip->memory[0x93] = 0x90;
  chip->memory[0x94] = 0x90;
  chip->memory[0x95] = 0xE0;

  //load E
  chip->memory[0x96] = 0xF0;
  chip->memory[0x97] = 0x80;
  chip->memory[0x98] = 0xF0;
  chip->memory[0x99] = 0x80;
  chip->memory[0x9A] = 0xF0;

  //load F
  chip->memory[0x9B] = 0xF0;
  chip->memory[0x9C] = 0x80;
  chip->memory[0x9D] = 0xF0;
  chip->memory[0x9E] = 0x80;
  chip->memory[0x9F] = 0x80; 
  
}

/*
 * Loads a ROM file (binary file) into memory so that it is ready to be run.
 *
 * chip: a struct of type Chip8 which represents the state of the chip.
 * filename: a String representing the name of the ROM file we want to run.
 *
 */
void chip8_load_ROM(Chip8 *chip, char *filename){
 
  //open ROM file
  FILE* rom_file = fopen(filename, "rb");//need open in "rb" to open in "binary mode"
  if (rom_file == NULL){
    perror(filename);
    errx(EXIT_FAILURE, "Failed to open ROM file");
  }

  //get size of ROM file
  fseek(rom_file, 0, SEEK_END);//go to end
  long ROM_size = ftell(rom_file);
  fseek(rom_file, 0, SEEK_SET);//go to start 

  //read binary file and save to memory from 0x200 and onwards
  if (fread(&chip->memory[PROGRAM_START_ADDRESS], sizeof(uint8_t), ROM_size, rom_file) != (size_t)ROM_size){
    errx(EXIT_FAILURE, "ROM file did not get loaded into memory.");
  }

  //close ROM file
  fclose(rom_file);
}
