#include <stdio.h>

int main(int argc, char *argv[]) 
{
   for(int i=1;i<100;i++){

        printf("%c",i);
    }
    printf("\n");
    printf("↑→↓←🡐🡒🡑🡓 ↵⏎⇦⇧");
    char in=' ';
    while(in != 'x'){
        in = getchar();
        printf("%d", in);
    }
    
}