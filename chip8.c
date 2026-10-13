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

  //load font, this took forever...
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

/*
 * Gets the current instruction for the program in memory, aka wherever pc is pointed to.
 *
 * chip: a struct of representing the state of the chip.
 *
 * RETURNS: a hex representing the opcode for the current instruction.
 */ 
uint16_t fetch(Chip8 *chip){
  
  //get 2 byte of current instruction (whererever pc is pointing)
  uint16_t opcode = ((uint16_t)chip->memory[chip->pc] << 8) | chip->memory[chip->pc + 1];
  chip->pc += 2;

  return opcode;
}

void draw_sprite(Chip8 *chip, uint8_t X, uint8_t Y, uint8_t N){

  //set X coordinate and Y coordinate
  int x_coord = chip->V[X] % 64;
  int y_coord = chip->V[Y] % 32;

  //set VF to 0
  chip->V[0xF] = 0; 

  //pixels in row
  for (int row = 0; row < N; row++){
    int screen_y_position = y_coord + row;
    
    //check if we are off the screen 
    if (screen_y_position >= 32){
      break;
    }
  
    //get current row byte from memory 
    uint8_t sprite_byte = chip->memory[chip->I + row];

    //pixels in col
    for (int col = 0; col < 8; col++){
      int screen_x_position = x_coord + col;
      
      //check if we are off the screen 
      if (screen_x_position >= 64){
        break;
      }

      //check to see if the pixel is filled in 
      if (sprite_byte & (0x80 >> col)){
        if (chip->display[screen_x_position][screen_y_position] == 1){
          chip->V[0xF] = 1;
        }
        chip->display[screen_x_position][screen_y_position] ^= 1;//XOR flip, if pixel is empty now it is filled ans same other way around 
      }
    }
  }
}



void execute(Chip8 *chip, uint16_t opcode){
  
  uint8_t first_nibble;
  uint8_t X;
  uint8_t Y;
  uint8_t N;
  uint8_t NN;
  uint16_t NNN;

  //first nibble, tells kind of instruction
  first_nibble = (opcode & 0xF000) >> 12;

  //second nibble, look up register
  X = (opcode & 0x0F00) >> 8;

  //third nibble, look up reister
  Y = (opcode & 0x00F0) >> 4; 

  //the fourth nibble, 4 bit number
  N = (opcode & 0x000F);

  //second byte (third + fourth nibbles), 8 bit number
  NN = (opcode & 0x00FF);

  //the second, third and fourth nibbles, 12 bit memory address
  NNN = (opcode & 0x0FFF);

  //figure out what kind of instruction it is based on first nibble
  switch(first_nibble){
    case 0x0:
      if (opcode == 0x00E0){
        //clear screen, dont have the screen yet so cant do this
        memset(chip->display, 0, sizeof(chip->display));
      } 
      else if (opcode == 0x00EE){
          //return from subroutine
          chip->sp--;
          chip->pc = chip->stack[chip->sp];
          //pop value from stack
          chip->stack[chip->sp] = 0;
      }
      break;
     
    case 0x1:
      //jump
      chip->pc = NNN;
      break;

    case 0x2:
      //subroutines
      //push pc to stack
      chip->stack[chip->sp] = chip->pc;
      chip->sp++;
      //set pc to NNN 
      chip->pc = NNN;

    case 0x3:
      //skip conditionally
      if (chip->V[X] == NN){
        chip->pc += 2;
      }
      break;

    case 0x4:
      //skip conditionally
      if (chip->V[X] != NN){
        chip->pc += 2;
      
      }
      break;

    case 0x5:
      //skip conditionally
      if (chip->V[X] == chip->V[Y]){
        chip->pc += 2;
      }
      break;

    case 0x6:
      //set register
      chip->V[X] = NN;
      break;

    case 0x7:
      //add to register
      chip->V[X] += NN;
      break;

    case 0x8:
      if (N == 0x0){
        chip->V[X] = chip->V[Y];
      } else if (N == 0x1){
        chip->V[X] = chip->V[X] | chip->V[Y];
      } else if (N == 0x2){
        chip->V[X] = chip->V[X] & chip->V[Y];
      } else if (N == 0x3){
        chip->V[X] = chip->V[X] ^ chip->V[Y];
      }//TODO: implement the rest of these codes
      break;

    case 0x9:
      //skip conditionally
      if (chip->V[X] != chip->V[Y]){
        chip->pc += 2;
      }
      break;

    case 0xA:
      //set I
      chip->I = NNN;
      break;
    case 0xD:
      //draw sprite
      draw_sprite(chip, X, Y, N);
      break;

    default:
      printf("HAVE NOT WRITTEN THIS CODE YET: %04X\n", opcode);
      break;
  }
}

void print_display(Chip8 *chip){
  for (int y = 0; y < DISPLAY_HEIGHT; y++){        // one text line per screen row
    for (int x = 0; x < DISPLAY_WIDTH; x++){       // one character per pixel
      if (chip->display[x][y] == 1){
        printf("#");
      } else {
        printf(" ");
      }
    }
    printf("\n");                                  // end of this row
  }
}

int main(){


  //testing rom load
  Chip8 chip;

  //reset chip
  chip8_reset(&chip);

  //load ROM file into memory
  chip8_load_ROM(&chip, "3-corax+.ch8");

  //print the first few bytes of the program to test if it loaded in
  for (int i = 0; i < 5000; i++){
    //printf("current instruction: %04X\n", fetch(&chip));
    //printf("byte %d: %02X\n", i, chip.memory[PROGRAM_START_ADDRESS + i]);
    
    uint16_t opcode = fetch(&chip);
    //printf("%04X\n", opcode);
    execute(&chip, opcode);
  }
  //printf("I: %0x\n", chip.I);
  //DISPLAY_HEIGHTrintf("PC: %0x\n", chip.pc);
  print_display(&chip); 


  return EXIT_SUCCESS;
}



