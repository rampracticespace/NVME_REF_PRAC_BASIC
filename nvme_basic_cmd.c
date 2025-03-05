//Intended this file to send random commands and display name as per


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

enum {
flush        = 0x00,
write        = 0x01,
read         = 0x02,
wr_uncr      = 0x04,
cmp          = 0x05,
wr_zros      = 0x08,
data_set_mgmt= 0x09,
verify       = 0x0C,
res_reg      = 0x0D,
res_rep      = 0x0E,
res_acq      = 0x11,
res_rel      = 0x15,
cpy          = 0x19,
};

//uint8_t lut_cmd_tbl[] = { 0x00,0x01,0x02,0x04,0x05,0x08,0x09,0x0C,0x0D,0x0E,0x11,0x15,0x19};
uint8_t lut_cmd_tbl[] = {flush,write,read,wr_uncr,cmp,wr_zros,data_set_mgmt,verify,res_reg,res_rep,res_acq,res_rel,cpy};    
uint8_t *lut_str_tbl[] = {"flush","write","read","wr_uncr","cmp","wr_zros","data_set_mgmt","verify",
                          "res_reg","res_rep","res_acq","res_rel","cpy"};
//0b 000 10b 00b 08h 
union opcode{
    struct cmd_type{
        uint8_t  data_trnsfer:2;
        uint8_t function:5;
        uint8_t cmd_type:1;
    }st_t;
    uint8_t combined_opcode;
}ut_t;
void send_cmd(){
    srand(time(NULL));
    ut_t.combined_opcode = lut_cmd_tbl[rand()%14];
}
void print_str(uint8_t num){
        int i = 0;
        for(i=0;i<(sizeof(lut_cmd_tbl)/sizeof(lut_cmd_tbl[0]));i++)
        {
            if(num == lut_cmd_tbl[i]){
                break;
            }
        }
            printf("\n%s***\t",lut_str_tbl[i]);
}
void print_bit_wise(uint8_t value){
    for (int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
}
void print_value(){
    printf("combined opcode is 0x%X\n",ut_t.combined_opcode);
    // printf("0b%X__",ut_t.st_t.cmd_type);
    // printf("%X__",ut_t.st_t.function);
    // printf("%X__",ut_t.st_t.data_trnsfer);
    print_bit_wise(ut_t.combined_opcode);
    print_str(ut_t.combined_opcode);
}
int main()
{
    send_cmd();
    print_value();
    return 0;
}



//this program intended to print the opcode with corresponding opcode name
