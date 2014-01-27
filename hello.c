// Compile with: gcc -O3 -o hello hello.c -llimit -ldl
#include <limit.h>
#include <stdint.h>
#include <stdio.h> 
 
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
    uint64_t br_last, brm_last, c, br, brm;
    size_t i;
 
    lprof_init(1, EV_CYCLES);
    function_to_watch(); // Do something
    lprof(1, c);
    printf("At Cycle: %7lu\n", c);
    lprof_close();
}
