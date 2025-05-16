#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "calcXsum.h"

uint8_t xSumCalc;

void calcXsumInit() {
    xSumCalc = 0;
}

void calcXsumFeed(uint8_t val){
    xSumCalc += val;
}

uint8_t calcXsumResult(){
    uint8_t val;
    val = ~xSumCalc;
    val +=1;
    return val;
}
