//
//  simulator.c
//  370p3
//
//  Created by Theo Delgizzi on 11/25/20.
//  Copyright © 2020 Theo Delgizzi. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for Project 3 */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* number of cycles run so far */
} stateType;

void printState(stateType *);
int convertNum(int);
void printInstruction(int instr);
int field0(int instruction);
int field1(int instruction);
int field2(int instruction);
int opcode(int instruction);
int convertNum(int num);

int
main(int argc, char *argv[])
{
     char line[MAXLINELENGTH];
     FILE *filePtr;
    
    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }
    
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }
    
    
    struct stateStruct state;
    struct stateStruct newState;
    
    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++) {
        
        if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }

        printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
    }
    
    for (int i = 0; i < NUMMEMORY; i++){
        state.dataMem[i] = state.instrMem[i];
    }
    
    
    
    for (int i = 0; i < NUMREGS;i++){
        state.reg[i] = 0;
    }
    
    state.pc = 0;
    state.cycles = 0;
    
    state.IFID.instr = 29360128;
    state.IFID.pcPlus1 = 0;
    state.IDEX.instr = 29360128;
    state.EXMEM.instr = 29360128;
    state.MEMWB.instr = 29360128;
    state.WBEND.instr = 29360128;
    
    //newState = state;
    
    
    while (1) {
        
        printState(&state);
        
        /* check for halt */
        if (opcode(state.MEMWB.instr) == HALT) {
            printf("machine halted\n");
            printf("total of %d cycles executed\n", state.cycles);
            exit(0);
        }
        
        newState = state;
        newState.cycles++;
        
        /* --------------------- IF stage --------------------- */
       
        
        newState.IDEX.instr = state.IFID.instr;
        newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
        
        newState.IFID.instr = state.instrMem[state.IFID.pcPlus1];
        newState.IFID.pcPlus1++;
        
        
        
        /* --------------------- ID stage --------------------- */
        
        
        bool stall = false;
        //***** WATCH OUT FOR WHERE YOU LOAD STUFF HERE AND SUCH
        if (opcode(state.IFID.instr) == 0 || opcode(state.IFID.instr) == 1){
            if (opcode(state.IDEX.instr) == 2){
                if (field1(state.IDEX.instr) == field0(state.IFID.instr)){
                    stall = true;
                }
                if (field1(state.IDEX.instr) == field1(state.IFID.instr)){
                    stall = true;
                }
            }
        }
        if (opcode(state.IFID.instr) == 2){
            if (opcode(state.IDEX.instr) == 2){
                if (field1(state.IDEX.instr) == field0(state.IFID.instr)){
                    stall = true;
                }
                if (field1(state.IDEX.instr) == field1(state.IFID.instr)){
                    stall = true;
                }
            }
        }
        if (opcode(state.IFID.instr) == 3){
            if (opcode(state.IDEX.instr) == 2){
                if (field1(state.IDEX.instr) == field0(state.IFID.instr)){
                    stall = true;
                }
                if (field1(state.IDEX.instr) == field1(state.IFID.instr)){
                    stall = true;
                }
            }
        }
        if (opcode(state.IFID.instr) == 4){
            if (opcode(state.IDEX.instr) == 2){
                if (field1(state.IDEX.instr) == field0(state.IFID.instr)){
                    stall = true;
                }
                if (field1(state.IDEX.instr) == field1(state.IFID.instr)){
                    stall = true;
                }
            }
        }
        
        if (stall){
            newState.pc--;
            newState.IDEX.instr = NOOPINSTRUCTION;
            newState.IFID.pcPlus1 = state.IFID.pcPlus1;
            newState.IFID.instr = state.IFID.instr;
            
        }//if
//        int temp = opcode(state.IFID.instr);
        if ( opcode(state.IFID.instr) == 0 || opcode(state.IFID.instr) == 1){
            newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
            newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
        }//if opcode is add or nor
        
        if ( opcode(state.IFID.instr) == 2 || opcode(state.IFID.instr) == 3){
            newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
            newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
            newState.IDEX.offset = convertNum(field2(state.IFID.instr));
        }//if
        
        
        
        if (opcode(state.IFID.instr) == 4){
            newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
            newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
            newState.IDEX.offset = convertNum(field2(state.IFID.instr));
        }
        
        newState.EXMEM.instr = state.IDEX.instr;
        
        
        
        
        /* --------------------- EX stage --------------------- */
        
        if ( opcode(state.IDEX.instr) == 0){
            int temp1 = state.IDEX.readRegA;
            int temp2 = state.IDEX.readRegB;
            
            if (opcode(state.WBEND.instr) == 0){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 1){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 2 ){
                if(field1(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }
                if ( field1(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 0){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 1){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 2 ){
                if(field1(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }
                if ( field1(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if(opcode(state.EXMEM.instr) == 0){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if and
            
            if(opcode(state.EXMEM.instr) == 1){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if nor
            
             newState.EXMEM.aluResult = temp1 + temp2;
        }//if
        //**************************
        //**************************
        //**************************
        //**************************
        if (opcode(state.IDEX.instr) == 1){
            int temp1 = state.IDEX.readRegA;
            int temp2 = state.IDEX.readRegB;
            
            if (opcode(state.WBEND.instr) == 0){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 1){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 2 ){
                if(field1(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }
                if ( field1(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 0){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 1){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 2 ){
                if(field1(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }
                if ( field1(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if(opcode(state.EXMEM.instr) == 0){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if and
            
            if(opcode(state.EXMEM.instr) == 1){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if nor
            
            newState.EXMEM.aluResult = ~(temp1 | temp2);

         }//if
        //**************************
        //**************************
        //**************************
        //**************************
        if ( opcode(state.IDEX.instr) == 2 ){
            int temp1 = state.IDEX.readRegA;
            
            if (opcode(state.WBEND.instr) == 0){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 1){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 2 ){
                if(field1(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }
            }
            
            if (opcode(state.MEMWB.instr) == 0){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 1){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 2 ){
                if(field1(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }
            }
            
            if(opcode(state.EXMEM.instr) == 0){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
            }//if and
            
            if(opcode(state.EXMEM.instr) == 1){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
            }//if nor
            
            newState.EXMEM.aluResult = temp1 + state.IDEX.offset;
            
        }//if
        //**************************
        //**************************
        //**************************
        //**************************
        if ( opcode(state.IDEX.instr) == 3 ){
            int temp1 = state.IDEX.readRegA;
            int temp2 = state.IDEX.readRegB;
            
            if (opcode(state.WBEND.instr) == 0){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 1){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 2 ){
                if(field1(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }
                if ( field1(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 0){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 1){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 2 ){
                if(field1(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }
                if ( field1(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if(opcode(state.EXMEM.instr) == 0){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if and
            
            if(opcode(state.EXMEM.instr) == 1){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if nor
            
            
            newState.EXMEM.aluResult = temp1 + state.IDEX.offset;
            //something for data dependencies for reg b here
            newState.EXMEM.readRegB = temp2;
            
        }//if
        //**************************
        //**************************
        //**************************
        //**************************
        if ( opcode(state.IDEX.instr) == 4 ){
            int temp1 = state.IDEX.readRegA;
            int temp2 = state.IDEX.readRegB;
            
            if (opcode(state.WBEND.instr) == 0){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 1){
                if(field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }//if
                if(field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.WBEND.instr) == 2 ){
                if(field1(state.WBEND.instr) == field0(state.IDEX.instr)){
                    temp1 = state.WBEND.writeData;
                }
                if ( field1(state.WBEND.instr) == field1(state.IDEX.instr)){
                    temp2 = state.WBEND.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 0){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 1){
                if(field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }//if
                if(field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if (opcode(state.MEMWB.instr) == 2 ){
                if(field1(state.MEMWB.instr) == field0(state.IDEX.instr)){
                    temp1 = state.MEMWB.writeData;
                }
                if ( field1(state.MEMWB.instr) == field1(state.IDEX.instr)){
                    temp2 = state.MEMWB.writeData;
                }//if
            }
            
            if(opcode(state.EXMEM.instr) == 0){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if and
            
            if(opcode(state.EXMEM.instr) == 1){
                if(field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                    temp1 = state.EXMEM.aluResult;
                }//if
                if(field2(state.EXMEM.instr) == field1(state.IDEX.instr)){
                    temp2 = state.EXMEM.aluResult;
                }//if
            }//if nor
            
            
            if (temp1 == temp2){
                newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
                newState.EXMEM.aluResult = 1;
            }
            else {
                newState.EXMEM.aluResult = 0;
            }
            
            newState.EXMEM.readRegB = temp2;
            
            
        }//if
        //**************************
        //**************************
        //**************************
        //**************************
        newState.MEMWB.instr = state.EXMEM.instr;
        /* --------------------- MEM stage --------------------- */
        
        if (opcode(state.EXMEM.instr) == 0){
            newState.MEMWB.writeData = state.EXMEM.aluResult;
        }//if
        if (opcode(state.EXMEM.instr) == 1){
            newState.MEMWB.writeData = state.EXMEM.aluResult;
        }//if
        if (opcode(state.EXMEM.instr) == 2){
            newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
        }//if
        if (opcode(state.EXMEM.instr) == 3){
            newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
        }//if
        if (opcode(state.EXMEM.instr) == 4){
            //branch taken
            if (state.EXMEM.aluResult == 1){
                newState.IFID.pcPlus1 = state.EXMEM.branchTarget;
                newState.pc = state.EXMEM.branchTarget - 1;
                newState.IFID.instr = 29360128;
                newState.IDEX.instr = 29360128;
                newState.EXMEM.instr = 29360128;
            }
        
        }//if
        
        newState.WBEND.instr = state.MEMWB.instr;
        newState.WBEND.writeData = state.MEMWB.writeData;
        
        /* --------------------- WB stage --------------------- */
        
        if ( opcode(state.MEMWB.instr) == 0){
            newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
        }//if
        if ( opcode(state.MEMWB.instr) == 1){
            newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
        }//if
        if ( opcode(state.MEMWB.instr) == 2){
            newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
        }//if
        if ( opcode(state.MEMWB.instr) == 3){
            
        }//if
        if ( opcode(state.MEMWB.instr) == 4){
            
        }//if
        
        
        
        state = newState; /* this is the last statement before end of the loop.
                           It marks the end of the cycle and updates the
                           current state with the values calculated in this
                           cycle */
        state.pc++;
        
    }//while 1
}//main









































//DO NOT MODIFY BELOW THIS LINE
//***************************
//***************************
//***************************
//***************************
void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);
    
    printf("\tdata memory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("\tIFID:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IFID.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IDEX.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
    printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
    printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
    printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->EXMEM.instr);
    printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
    printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
    printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->MEMWB.instr);
    printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->WBEND.instr);
    printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
    return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
    return(instruction>>22);
}

void
printInstruction(int instr)
{
    
    char opcodeString[10];
    
    if (opcode(instr) == ADD) {
        strcpy(opcodeString, "add");
    } else if (opcode(instr) == NOR) {
        strcpy(opcodeString, "nor");
    } else if (opcode(instr) == LW) {
        strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
        strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
        strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
        strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
        strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
        strcpy(opcodeString, "noop");
    } else {
        strcpy(opcodeString, "data");
    }
    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
           field2(instr));
}


int
convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}

