// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
//#define BUFFER_SIZE 50
//static char buffer[BUFFER_SIZE];

char history[100000][COMMAND_LENGTH];
int historyCountArray[100000];
int historyCount = 0;
int ctrlC = 0;



void addHistory(int count,char *cmd){
  strcpy(history[historyCount],cmd);
  historyCountArray[count] = count;
}

void printHistory(){
  write(STDOUT_FILENO,"Shell command history:\n",strlen("Shell command history:\n"));
  char command[COMMAND_LENGTH];
  for (int i = historyCount -9; (i <= historyCount) ; i++) {
      if (historyCountArray[i] != 0) {
          sprintf(command, "%d\t%s\n", historyCountArray[i], history[i]);
          write(STDOUT_FILENO, command, strlen(command));
      }
  }
    /*for(int i=1;i<=10;i++){
      char str[100];
      sprintf(str, "%d", historyCountArray[i]);
      write(STDOUT_FILENO,str,strlen(str));
      write(STDOUT_FILENO,history[i],strlen(history[i]));
      write(STDOUT_FILENO, "\n", strlen("\n"));
    }*/
}

void handle_SIGINT(){
    ctrlC = 1;
    write(STDOUT_FILENO, "\n", strlen("\n"));
    if(historyCount ==0){
      write(STDOUT_FILENO,"NO HISTORY TO DISPLAY",strlen("NO HISTORY TO DISPLAY"));
      write(STDOUT_FILENO, "\n", strlen("\n"));
    }
    else if(historyCount >= 1){
      printHistory();
    }
    //printHistory();
    signal(SIGINT, handle_SIGINT);
}

void  execute(char **argv){
     pid_t  pid;
     int status;
     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }else {                                  /* for the parent:      */
          while (wait(&status) != pid)       /* wait for completion  */
               ;
     }
}


/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
 void init_shell(){
   printf(" ||====================================================================|| \n");
   printf(" ||//$\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/$\\|| \n");
   printf(" ||(100)==================| Nebula Cyber Bucks |==================(100)|| \n");
   printf(" ||\\$//         ~         '------========------'                   \\$//|| \n");
   printf(" ||<</         /$\\              // ____ \\\\                          \\>>|| \n");
   printf(" ||>>|        //L\\\\            // ///..) \\\\           XXXX          |<<|| \n");
   printf(" ||<<|        \\\\ //           || <||  >\\\\  ||                       |>>|| \n");
   printf(" ||>>|         \\$/            ||  $$ --/  ||          XXXXXXXXX     |<<|| \n");
   printf(" ||<<|     Free to Use        *\\  |\\_/  //*                         |>>|| \n");
   printf(" ||>>|     No Warranty                    *\\/___\\_//*               |<<|| \n");
   printf(" ||<<\\      Rating: E     _____/ Kevin Virk \\_____   XX XXXXX       />>|| \n");
   printf(" ||//$\\                 ~|    REPUBLIC OF SPACE   |~                /$\\|| \n");
   printf(" ||(100)===========      | ONE HUNDRED SPACE BUCKS|     ==========(100)|| \n");
   printf(" ||\\$//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$\\|| \n");
   printf(" ||====================================================================|| \n");

   printf("\n\n\n\t\t ****Kevin Virks Nebula Cyber Shell****");
   printf("\n\n\t\t   USE AT YOUR OWN RISK, NO WARRANTY");
   char* username = getenv("USER");
   printf("\n\n\n\t\t\t  USER is: %s", username);
   printf("\n");
 }

void printWorkingDirectory(){
    char cwd[1024];
   	getcwd(cwd, sizeof(cwd));
		write(STDOUT_FILENO, cwd, strlen(cwd));
		write(STDOUT_FILENO, "\n", strlen("\n"));
		//printf("\nDir: %s", cwd);
 }


int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background){
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

  if ( (length < 0) && (errno !=EINTR) ){
    perror("Unable to read command. Terminating.\n");
    exit(-1);
    /* terminate with error */
  }

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[]){
  signal(SIGINT, handle_SIGINT);
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

	init_shell();
  //printf("%s\n",myPrompt );
  //char *tmp = "test";
  //strcat(myPrompt,tmp);
	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		char cwd[1024];
   	getcwd(cwd, sizeof(cwd));
		write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, "> ", strlen("> "));
		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);

		// DEBUG: Dump out arguments:
		for (int i = 0; tokens[i] != NULL; i++) {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		if (in_background) {
			write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
		}

		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */
    // int historyCount  = 0;
    /* set up the signal handler */
    /*struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    handler.sa_flags = 1;
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);
    strcpy(buffer,"I caught a Ctrl-C!\n");
    //printf("Program now waiting for Ctrl-C.\n");*/
    if(tokens[0] == NULL){
      ;
    }
    else if(strcmp(tokens[0],"!!")==0){
       if(historyCount == 0){
         write(STDOUT_FILENO,"Error: No Commands Entered Yet!",strlen("Error: No Commands Entered Yet!"));
       }
       else if(historyCount >= 1){
         char *commands = history[historyCount];
         write(STDOUT_FILENO,history[historyCount],strlen(history[historyCount]));
         write(STDOUT_FILENO, "\n", strlen("\n"));
         write(STDOUT_FILENO,"You are going to execute your last command:",strlen("You are going to execute your last command:"));
         write(STDOUT_FILENO,commands,strlen(commands));
         write(STDOUT_FILENO, "\n", strlen("\n"));
         tokenize_command(commands,tokens);
         strcpy(tokens[0],commands);
       }
       //execute(&commands);
     }
     else if(strncmp(tokens[0], "!", 1)==0){
       if(historyCount == 0){
         write(STDOUT_FILENO,"ERROR NO HISTORY",strlen("ERROR NO HISTORY"));
         write(STDOUT_FILENO,"\n",strlen("\n"));
       }
       else if(historyCount >= 1){
         //write(STDOUT_FILENO,tokens[0],strlen(tokens[0]));
         //write(STDOUT_FILENO, "\n", strlen("\n"));
         char word[COMMAND_LENGTH] = "";
         strcat(word,tokens[0]);
         int idxToDel = 0;
         memmove(&word[idxToDel], &word[idxToDel + 1], strlen(word) - idxToDel);
         write(STDOUT_FILENO,word,strlen(word));
         write(STDOUT_FILENO, "\n", strlen("\n"));
         int number = atoi(word);
         if(number <= 0){
           write(STDOUT_FILENO,"ERROR: NEED A NUMBER AFTER ! TO EXECUTE the Nth COMMAND",strlen("ERROR: NEED A NUMBER AFTER ! TO EXECUTE the Nth COMMAND"));
         }
         if(number > historyCount){
           write(STDOUT_FILENO,"ERROR NUMBER GIVEN TO ! IS NOT IN HISTORY YET",strlen("ERROR NUMBER GIVE TO ! IS NOT IN HISTORY YET"));
           write(STDOUT_FILENO, "\n", strlen("\n"));
         }
         else{
           char *commands = history[number];
           write(STDOUT_FILENO,"You are going to execute your nth command:",strlen("You are going to execute your nth command:"));
           write(STDOUT_FILENO,commands,strlen(commands));
           write(STDOUT_FILENO, "\n", strlen("\n"));
           tokenize_command(commands,tokens);
           strcpy(tokens[0],commands);
         }
       }
     }

     if(ctrlC >= 1){
       ctrlC = 0;
     }
     else if(tokens[0] == NULL){
 			 ;
 		 }
 		 else if(strcmp(tokens[0],"exit")==0){
 			 return 0;
 		 }
 	 	 else if(strcmp(tokens[0],"pwd")==0){
 	 		  printWorkingDirectory();
         historyCount++;
         char cmd[COMMAND_LENGTH] = "";
         strcat(cmd,tokens[0]);
         /*for (int i = 0; tokens[i] != NULL; i++) {
           strcat(cmd,tokens[i]);
           strcat(cmd, " ");
           strcat(cmd,tokens[i]);
     		}*/
        if(in_background == true){
          strcat(cmd, " ");
          strcat(cmd, "&");
        }
         addHistory(historyCount,cmd);
 	 	 }
 		 else if(strcmp(tokens[0],"cd")==0){
 			 //chdir(tokens[1]);
        if(chdir(tokens[1]) == -1){
          write(STDOUT_FILENO, "Invalid Directory ", strlen("Invalid Directory "));
          write(STDOUT_FILENO, "\n", strlen("\n"));
          historyCount++;
          char cmd[COMMAND_LENGTH] = "";
          for (int i = 0; tokens[i] != NULL; i++) {
            strcat(cmd,tokens[i]);
            strcat(cmd, " ");

         }
         if(in_background == true){
           strcat(cmd, " ");
           strcat(cmd, "&");
         }
          addHistory(historyCount,cmd);
        }
        else{
          chdir(tokens[1]);
          historyCount++;
          char cmd[COMMAND_LENGTH] = "";
          strcat(cmd,tokens[0]);
          strcat(cmd, " ");
          strcat(cmd,tokens[1]);
          addHistory(historyCount,cmd);
        }
        //myPrompt[0] = '\0';
        //char *tmp = strdup(myPrompt);
        //strcpy(myPrompt, tokens[1]);
        ///strcat(myPrompt, tmp);
        //strcat(myPrompt,"> ");
        //free(tmp);
 		 }
     else if(strcmp(tokens[0],"history")==0){
       if(historyCount > 0){
          printHistory();
          historyCount++;
          char *cmd = tokens[0];
          addHistory(historyCount,cmd);
       }
       else{
         write(STDOUT_FILENO, "No History to Display: Try some commands first", strlen("No History to Display: Try some commands first"));
         write(STDOUT_FILENO, "\n", strlen("\n"));
       }
     }
     else{
       pid_t pid = fork();
       if (pid < 0){
          write(STDOUT_FILENO,"Fork failed.\n",strlen("Fork failed.\n"));
          exit (1);
        }
        else if (pid == 0){
          if (execvp(tokens[0], tokens) == -1){
              write(STDOUT_FILENO,tokens[0],strlen(tokens[0]));
              write(STDOUT_FILENO,"\n",strlen("\n"));
		          write(STDOUT_FILENO,": Unkown Command\n",strlen(": Unkown Command\n"));
              return 0;
          }
       	}
        else{
          wait(NULL);
     }
     historyCount++;
     char cmd[COMMAND_LENGTH] = "";
     for (int i = 0; tokens[i] != NULL; i++) {
       strcat(cmd,tokens[i]);
       strcat(cmd, " ");
    }
     addHistory(historyCount,cmd);
     while (waitpid(-1, NULL, WNOHANG) > 0){
       ; // do nothing.
     }
    }
  }
    return 0;
}
