// Compile with: gcc -O3 -o hello hello.c -llimit -ldl
#include <limit.h>
#include <stdint.h>
#include <stdio.h> 
#include <ctype.h>
 
#define str1 "Hello World, Hello World, Hello World"
#define str2 "HELLOWORLDHELLOWORLDHELLOWORLDHELLOWO"
 
const char* testStr = str1;  //Also try str2
 
uint64_t uppersFound = 0, lowersFound = 0;
void function_to_watch(void) {
    size_t i;
    const char* c = testStr;
 
    while (*c != 0) {
      if (isupper(*c))
        uppersFound++;
      else if (islower(*c))
        lowersFound++;
      c++;
    }
}
 
int main(void) {
    uint64_t ins=0,cycles=0;
    size_t i;
 
    lprof_init(2, EV_INSTR, EV_CYCLES);
    lprof_debug();
    function_to_watch(); // Do something
    lprof(0, ins);
    lprof(1, cycles);
    printf("ins: %7lu\ncycles: %7lu\n", ins, cycles);
    //lprof_close();
}
