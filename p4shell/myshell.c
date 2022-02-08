#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

void printError(){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
}

void my_exec(char *cmd){
    const char* delim = " \n\t";
    char *hi = NULL;
    char *parsed_cmd_buff = strtok_r(cmd, delim, &hi);
    char *command[514];
    unsigned int i = 0;
         
    while(parsed_cmd_buff != NULL && i < 514){
        command[i] = parsed_cmd_buff;
        parsed_cmd_buff = strtok_r(NULL, delim, &hi);
        i++;
        }
            
    command[i] = NULL;

    if(execvp(command[0], command)< 0){
        printError();
        exit(0);
    };
    //fork();
}

// need to parse string into 2 separate arguments in case that cd comes with extra
void my_cd(char *s){
    const char* delim = " \n\t";
    char *hi = NULL;
    char *parsed_s = strtok_r(s, delim, &hi);
    //printf("after parsed s\n");
    //fflush(stdout);
    char *cdd[2];
    unsigned int i = 0;

    while(parsed_s != NULL && i < 2){
        //printf("in while loop\n");
        //fflush(stdout);
        cdd[i] = parsed_s;
        if(strcmp(cdd[i], ">") == 0){
            printError();
            printf("INSIDE WHILE LOOP IF\n");
            return;
        }
        //printf("passed cdd[i]\n");
        parsed_s = strtok_r(NULL, delim, &hi);
        /*if(parsed_s == '>'){
            printError();
            return;
        }*/
        //printf("current command is: ...%s...\n", parsed_s);
        //printf("passed parsed_s\n");
        i++;
    }
            
    cdd[i] = '\0';
    //printf("after parsed_s[i]\n");
    if(strcmp(cdd[0], "cd>") == 0){
        printError();
        return;
    }
    if(cdd[1] != NULL){
        //printf("inside if\n");
        if (chdir(cdd[1]) != 0){
            //printf("cdd[1] is %s", cdd[1]);
           // printf("CHDIR inside if\n");
            printError();
            return;
        }
    } else if(strcmp(cdd[0], "cd") == 0){
        //printf("inside else\n");
        if (chdir(getenv("HOME")) != 0){
            //printf("inside if in else\n");
            printError();
            return;
        }
    } else {
        printError();
        return;
    }
}

void my_exit(char *s){
     const char* delim = " \n\t";
    char *hi = NULL;
    char *parsed_s = strtok_r(s, delim, &hi);
    //printf("after parsed s\n");
    //fflush(stdout);
    char *cdd[2];
    unsigned int i = 0;

    while(parsed_s != NULL && i < 2){
        //printf("in while loop\n");
        //fflush(stdout);
        cdd[i] = parsed_s;
        if(strcmp(cdd[i], ">") == 0){
            printError();
            return;
        }
        //printf("passed cdd[i]\n");
        parsed_s = strtok_r(NULL, delim, &hi);
        /*if(parsed_s == '>'){
            printError();
            return;
        }*/
        //printf("current command is: ...%s...\n", parsed_s);
        //printf("passed parsed_s\n");
        i++;
    }
            
    cdd[i] = '\0';
    //printf("after parsed_s[i]\n");
    if(strcmp(cdd[0], "exit>") == 0){
        printError();
        return;
    }

    if(cdd[1]){
        printError();
        return;
    } else{
        exit(0);
    }
    //exit(0);
}

void my_pwd(char *s){
    const char* delim = " \n\t";
    char *hi = NULL;
    char *parsed_s = strtok_r(s, delim, &hi);
    char *cdd[2];
    unsigned int i = 0;

    while(parsed_s != NULL && i < 2){
        cdd[i] = parsed_s;
        if(strcmp(cdd[i], ">") == 0){
            printError();
            return;
        }
        parsed_s = strtok_r(NULL, delim, &hi);
        /*if(parsed_s == '>'){
            printError();
            return;
        }*/
        i++;
    }
            
    cdd[i] = '\0';
    //printf("after parsed_s[i]\n");

    if(strcmp(cdd[0], "pwd>") == 0){
        printError();
        return;
    }

    if(cdd[1]){
        printError();
        return;
    }
    char directory[1000000];
    // used https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
    // to learn more about getcwd
    if(getcwd(directory, 1000000) != NULL){
        myPrint(directory);
        myPrint("\n");
    } else{
        printError();
        return;
     }


}

void execute(char *string){
    //printf("string is: .....%s.....\n", string);
    if(strncmp(string, "cd", 2) == 0){
        /*if(chdir(getenv("HOME") != 0)){
            printError();
        }*/
        
        my_cd(string);
    } else if(strncmp(string, "exit", 4) == 0){
        my_exit(string);
    } else if(strncmp(string, "pwd", 3) == 0){
        /*char directory[1000000];
        // used https://pubs.opengroup.org/onlinepubs/9699919799/functions/getcwd.html
        // to learn more about getcwd
        if(getcwd(directory, 1000000) != NULL){
            myPrint(directory);
            myPrint("\n");
        } else{
            printError();
        }*/
        my_pwd(string);
    } else {
        pid_t id = fork();
        int stat = 0;
        if(id == 0){
            my_exec(string);
        } else{
            waitpid(id, &stat, 0);
        }
        
    }
}


// function to trim leading and trailing spaces
// link: https://codeforwin.org/2016/04/c-program-to-trim-both-leading-and-trailing-white-spaces-in-string.html
char* trim(char * str)
{
    int index, i;
    /*
     * Trim leading white spaces
     */
    index = 0;
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }
    /* Shift all trailing characters to its left */
    i = 0;
    while(str[i + index] != '\0')
    {
        str[i] = str[i + index];
        i++;
    }
    str[i] = '\0'; // Terminate string with NULL
    /*
     * Trim trailing white spaces
     */
    i = 0;
    index = -1;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index = i;
        }
        i++;
    }
    /* Mark the next character to last non white space character as NULL */
    str[index + 1] = '\0';
    return str;
}



int main(int argc, char *argv[]) 
{
    char cmd_buff[10000];
    char *pinput;
    if(argc > 2){
        printError();
        exit(0);
    }
    //Code for no batch file given
    if(argc == 1) {

        while (1) {
            myPrint("myshell> ");
            pinput = fgets(cmd_buff, 10000, stdin);
            if(!pinput){
                exit(0);
            }

    
            int is_terminate_char = 0;
            int index = 0;
            for(index = 0; index < 514; index++){
                if(cmd_buff[index] ==  '\0'){
                    is_terminate_char = 1;
                    break;
                }   
            }
            //
            cmd_buff[index] = '\0';
            if(is_terminate_char == 0){
                write(STDOUT_FILENO, pinput, index);
                printError();
                continue;
            }




            const char* delim_semi = ";\n";
            char* hi = NULL;
            char* cmd_buff_cpy = strdup(cmd_buff);
            char *separated_cmd_buff = strtok_r(cmd_buff_cpy, delim_semi, &hi);
                
            char *array[514];
            unsigned int i = 0;

            

            
            
            while(separated_cmd_buff != NULL && i < 514){
                array[i] = separated_cmd_buff;
                separated_cmd_buff = strtok_r(NULL, delim_semi, &hi);
                i++;
            }

            array[i] = NULL;
            int j = 0;
            while(array[j] != NULL){
                execute(trim(array[j]));
                j++;
            }
            
        }
    }

    //Code for batch mode
    if(argc == 2) {
        char* batchfile = argv[1];
        //printf("batchfile = %s\n", batchfile);

        //myPrint("YO\n");
        FILE * fp;
        fp = fopen(batchfile, "r");
        if(fp == NULL)
        {
            printError();
            exit(0);
        }
        

        //int linenum = 0;
        while (1) {
            pinput = fgets(cmd_buff, 514, fp);
            if( feof(fp) ) { 
                write(STDOUT_FILENO, "\n", 1);
                exit(0);
            }
            if(!pinput || (strcmp(pinput, "\n") == 0)){
                continue;
            }
            int isBlank = 1;
            for(int z=0; z<514; z++){
                
                if( (cmd_buff[z] != ' ')&&(cmd_buff[z] != '\n')&&(cmd_buff[z] != '\t')){
                    isBlank = 0;
                    break;
                }
                if(cmd_buff[z] == '\n')
                {
                    break;
                }
            }
       

            if(isBlank == 1){
                continue;
            }

            write(STDOUT_FILENO, pinput, strlen(pinput));
            if( feof(fp) ) { 
                write(STDOUT_FILENO, "\n", 1);
                exit(0);
            }

            int is_terminate_char = 0;
            int k;
            for(k = 0; k < 514; k++){
                //printf("\ncurrent char is %c and index is %d\n", cmd_buff[k], k);
                if(cmd_buff[k] ==  '\0'){
                    is_terminate_char = 1;
                    //printf("found term char\n");
                    break;
                }
                
            }
            cmd_buff[k] = '\0';
            if(is_terminate_char == 0){
                while(1){
                    if(feof(fp)){
                        exit(0);
                    }
                    char c = getc(fp);
                    //printf("char is this: %c", c);
                    fflush(stdout);
                    if(c == '\n'){
                        putc(c, stdout);
                        break;
                    }

                }
                myPrint("\n");
                printError();
                continue;
            }

            //printf("\n %d %s \n", linenum, pinput);

            
            

            const char* delim_semi = ";\n";
            char* hi = NULL;
            char* cmd_buff_cpy = strdup(cmd_buff);
            char *separated_cmd_buff = strtok_r(cmd_buff_cpy, delim_semi, &hi);
                
            char *array[514];
            unsigned int i = 0;

            
            while(separated_cmd_buff != NULL && i < 514){
                array[i] = separated_cmd_buff;
                separated_cmd_buff = strtok_r(NULL, delim_semi, &hi);
                i++;
            }

            array[i] = NULL;
            int j = 0;
            while(array[j] != NULL){
                execute(trim(array[j]));
                j++;
            }
            if( feof(fp) ) { 
                break;
            }
        }
    }
}