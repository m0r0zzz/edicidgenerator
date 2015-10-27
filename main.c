#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#define CRC_POLY 0x1021

typedef struct __attribute__((__packed__)){
    uint8_t ver;
    uint64_t id;
    uint16_t crc;
} edicid;

void print_help(char* nm){
    printf("EDIC ID block generation utility.\nUsage\n\t%s <outfile> <ver> [keyfile]\n", nm);
}

uint16_t crc16(uint8_t* data, uint8_t len){
    uint32_t rem = 0;
    for(uint8_t i = 0; i < len; i++){
        rem ^= (uint16_t)data[i] << 8;
        for(uint8_t j = 0; j < 8; j++){
            if(rem&0x8000) rem = (rem << 1) ^ CRC_POLY;
            else rem <<= 1;
            rem &= 0xFFFF;
        }
    }
    return rem;
}

int main(int argc, char** argv){
    if(argc != 3 && argc != 4){
        print_help(argv[0]);
        return 0;
    }
    FILE* in = NULL;
    if(argc == 4) in = fopen(argv[3], "r");
    else in = stdin;

    if(in == NULL){
        printf("Can't open instream\n");
        return -1;
    }

    edicid id = {0, 0, 0};
    id.ver = atoi(argv[2]);
    char keystream[64];
    fscanf(in, "%s\n", keystream);
    if(argc == 4) fclose(in);
    id.id = atoll(keystream);
    id.crc = crc16((uint8_t *)&id.id, 8);

    FILE* out = fopen(argv[1], "wb");
    if(out == NULL){
        printf("Can't open outfile\n");
        return -2;
    }
    fwrite(&id.ver, 1 , 1, out);
    fwrite(&id.id, 8 , 1, out);
    fwrite(&id.crc, 2, 1, out);
    uint8_t nuls[21] = {0};
    fwrite(nuls, 21, 1, out);
    fclose(out);

    return 0;
}
