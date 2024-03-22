#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>






void getParentOfPID(char* stringToWriteTo, int PID){ //returns string since we'll be passing back in as a string most likely.

   printf("Getting parent of PID %d\n", PID);
   char PIDAsString[40];
   sprintf(PIDAsString, "%d", PID);
 

    char getParentString[90];

    strcpy(getParentString, "/bin/cat /proc/");
    strcat(getParentString, PIDAsString);
    strcat(getParentString, "/status");

    FILE* fp = popen(getParentString, "r");

    char temp[100];
    //char result[40];

    const char start[] = "PPid:";
    //FILE* fp = fopen("/proc/${1:-$PPID}/comm", "r");
    while (fgets(temp, sizeof(temp), fp)){ 
        if(!strncmp(start, temp, 4) ){
            char* token = strtok(temp, "\t");
            strcpy(stringToWriteTo, strtok(NULL, ""));
            //strcpy(stringToWriteTo, strtok(stringToWriteTo, "\n"));
            strtok(stringToWriteTo, "\n");

            //printf("%s", stringToWriteTo);
        }
        
    }
    pclose(fp);
}

void getNameOfPID(char* stringToWriteTo, char* PID){

    printf("Finding name of PID %s\n", PID);

    char getNameCommand[40];
    char PIDName[40];

    strcpy(getNameCommand, "/proc/");
    strcat(getNameCommand, PID);
    strcat(getNameCommand, "/comm");

    FILE* fp = fopen(getNameCommand, "r");
    fgets(PIDName, sizeof(PIDName), fp);
    fclose(fp);

    //printf("PID NAME: %s\n", PIDName);

    strcpy(stringToWriteTo, PIDName);


    //char PIDasString[10];
    //sprintf(PIDasString, "%d")

}


int main(int argc, char** argv){

    char parent[40];
    char TermName[40];

    int PPID = getppid();
    getParentOfPID(parent, PPID);
    //printf("parent: %s\n", parent);
    getNameOfPID(TermName, parent);
    printf("You ran this from: %s", TermName);

    //system("coproc st -- sh -c \"cmus\";sleep 0.5; cmus-remote -q song1.mp3; cmus-remote -p"); //must be a better way to do this. We can't tell what term they're using, can we? (copy what neofetch does?)



    return 0; //get output from echo $(< "/proc/${1:-$PPID}/comm")
}
