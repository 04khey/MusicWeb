#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>


char path[100];

int main(int argc, char** argv){
    //FILE* fp = popen("/bin/echo $(< \"/proc/${1:-$PPID}/comm\")", "r");
    FILE* fp = popen("/bin/cat /proc/${1:-$PPID}/comm", "r");


    //FILE* fp = fopen("/proc/${1:-$PPID}/comm", "r");
    fgets(path, sizeof(path), fp); 
    printf("%s", path);


    //system("coproc st -- sh -c \"cmus\";sleep 0.5; cmus-remote -q song1.mp3; cmus-remote -p"); //must be a better way to do this. We can't tell what term they're using, can we? (copy what neofetch does?)


    pclose(fp);
    return 0; //get output from echo $(< "/proc/${1:-$PPID}/comm")
}
