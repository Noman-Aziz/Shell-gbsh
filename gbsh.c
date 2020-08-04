#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

void Prompt();
void Pwd();
void ListDirectory();
void ChangeDirectory(char []);
void ShowEnvironment();
void SetEnvironment(char []);
void UnSetEnvironment(char []);
void OtherCommands(char []);
void signalhandle(int );
void OutputRedirection(char []);
void InputRedirection(char []);
void InputOutputRedirection(char []);
void PipeRedirection(char []);

//for storing the home directory from where program is launched
char home[500] ;

int main(int argc, char *argv[])
{	
	char command[500] = "" ;
	char backup[500];

	getcwd(home,500);

	system("clear");

	//changine the default env var
	char gbsh[] = "/gbsh";
	char temp[500];
	getcwd(temp,500);
	strcat(temp,gbsh);
	if(setenv("SHELL",temp,1) < 0)
	{
		printf("ERROR SETTING THE SHELL ENV VARIABLE\n");
		exit(1);
	}
	/////////////////////////////

	//sigint signal interruptor
	signal(SIGINT,signalhandle);

	//The Main Prompt of Shell
	while(1)
	{
		//used for printing prompt
		Prompt();

		//used for reading a whole line and removing new line character with null
		fgets(command,500,stdin);
		command[strlen(command)-1] = '\0';

		for(int i=0 ; command[i] != '\0' ; i++)
		{
			backup[i] = command[i];
			backup[i+1] = command[i+1];		
		}

		//changing color of output to yellow
		printf("\033[01;33m");

		//> command or < command
		char *token = strtok(backup," ");
		bool pip = false;
		bool in = false;
		bool out = false;
		bool both = false;
		while(token != NULL)
		{
			if(strcmp(token,">") == 0)
			{
				if(in)
				{
					in = false;
					both = true;
					break;
				}

				else
					out = true;
			}

			if(strcmp(token,"|") == 0)
			{
				pip = true;
				break;
			}

			if(strcmp(token,"<") == 0)
				in = true;

			if(both)
				break;

			token = strtok(NULL," ");
		}

		//input redirection
		if(in)
			InputRedirection(command);

		//output redirection
		else if(out)
			OutputRedirection(command);

		//io redirection
		else if(both)
			InputOutputRedirection(command);

		//piping
		else if(pip)
			PipeRedirection(command);

		//continue
		else if(strcmp(command,"") == 0)
			continue;

		//print working directory
		else if(strcmp(command,"pwd") == 0)
			Pwd();

		//& support
		else if(strcmp(command,"pwd&") == 0 || strcmp(command,"pwd &") == 0 )
		{
			int pid = fork();

			if(pid>0)
			{
				printf("[1] %d\n",pid);
				continue;
			}

			else if(pid == 0)
			{
				Pwd();
				exit(0);	
			}

			else
			{
				printf("Creation of Child Process was Unsucessfull\n");
			}
		}

		//clear the screen
		else if(strcmp(command,"clear") == 0)
			system("clear");

		//& support
		else if(strcmp(command,"clear&") == 0 || strcmp(command,"clear &") == 0 )
		{
			int pid = fork();

			if(pid>0)
			{
				printf("[1] %d\n",pid);
				continue;
			}

			else if(pid == 0)
			{
				system("clear");
				exit(0);	
			}

			else
			{
				printf("Creation of Child Process was Unsucessfull\n");
			}
		}

		//exit the shell
		else if(strcmp(command,"exit") == 0)
			break;

		//& support
		else if(strcmp(command,"exit&") == 0 || strcmp(command,"exit &") == 0 )
		{
			int pid = fork();

			if(pid>0)
			{
				printf("[1] %d\n",pid);
				continue;
			}

			else if(pid == 0)
			{
				break;
			}

			else
			{
				printf("Creation of Child Process was Unsucessfull\n");
			}
		}

		//ls command
		else if(strcmp(command,"ls") == 0)
			ListDirectory();

		//& support
		else if(strcmp(command,"ls&") == 0 || strcmp(command,"ls &") == 0 )
		{
			int pid = fork();

			if(pid>0)
			{
				printf("[1] %d\n",pid);
				continue;
			}

			else if(pid == 0)
			{
				ListDirectory();
				exit(0);	
			}

			else
			{
				printf("Creation of Child Process was Unsucessfull\n");
			}
		}

		//cd command
		else if(command[0] == 'c' && command[1] == 'd' && (command[2] == ' ' || command[2] == '\0' || command[2] == '&') )
			ChangeDirectory(command);

		//environ command
		else if(strcmp(command,"environ") == 0)
			ShowEnvironment();

		//& support
		else if(strcmp(command,"environ&") == 0 || strcmp(command,"environ &") == 0 )
		{
			int pid = fork();

			if(pid>0)
			{
				printf("[1] %d\n",pid);
				continue;
			}

			else if(pid == 0)
			{
				ShowEnvironment();
				exit(0);	
			}

			else
			{
				printf("Creation of Child Process was Unsucessfull\n");
			}
		}

		//setenv command
		else if(command[0] == 's' && command[1] == 'e' && command[2] == 't' && command[3] == 'e' &&
		 		command[4] == 'n' && command[5] == 'v' && command[6] == ' ')
			SetEnvironment(command);

		//unsetenv command
		else if(command[0] == 'u' && command[1] == 'n' && command[2] == 's' && command[3] == 'e' &&
		 		command[4] == 't' && command[5] == 'e' && command[6] == 'n' && command[7] == 'v' && command[8] == ' ')
			UnSetEnvironment(command);

		//any other command
		else
			OtherCommands(command);
	}

	exit(0); // exit normally	
}

void Prompt()
{
	//for hostname
	char hostname[100];

	//for current working directory
	char temp[500];

	gethostname(hostname, 100);
	getcwd(temp, 500);

	//removing the /home/username from the cwd if its there to make it look more like terminal
	if(temp[0] == '/' && temp[1] == 'h' && temp[2] == 'o' && temp[3] == 'm' && temp[4] == 'e' && temp[5] == '/')
	{
		char cwd[500];

		int j=0;
		int i=6;

		while(temp[i] != '/')
		{
			if(temp[i] == '\0')
			{
				printf("\033[1;32m%s@",getenv("USER"));
				printf("%s\033[1;31m:\033[1;34m~",hostname);
				printf("\033[1;31m$ ");
				return;
			}

			i++;
		}

		for( ; temp[i] != '\0' ; i++,j++)
			cwd[j] = temp[i];

		cwd[j] = '\0';

		printf("\033[1;32m%s@",getenv("USER"));
		printf("%s\033[1;31m:\033[1;34m~",hostname);
		printf("%s\033[1;31m$ ",cwd);
	}

	else
	{
		printf("\033[1;32m%s@",getenv("USER"));
		printf("%s\033[1;31m:\033[1;34m~",hostname);
		printf("%s\033[1;31m$ ",temp);
	}
	
	return ;
}

void Pwd()
{
	//for current working directory
	char cwd[500];

	getcwd(cwd, 500);

	printf("%s \n",cwd);

	return;
}

void ListDirectory()
{
	struct dirent ** listdirectory;

	int ret = scandir(".",&listdirectory,NULL,alphasort);

	if(ret < 0)
	{
		printf("ERROR SCANNING DIRECTORY\n");
		exit(1);
	}

	else if(ret <= 2)
	{
		free(listdirectory);
		return;
	}

	else
	{
		while( ret > 2 )
		{
			ret--;

			//hidden directory
			if(listdirectory[ret]->d_name[0] == '.')
			{
				free(listdirectory[ret]);
				continue;
			}

			printf("%s  ",listdirectory[ret]->d_name);
			free(listdirectory[ret]);
		}
	}

	free(listdirectory);
	printf("\n");

	return;
}

void ChangeDirectory(char path[])
{
	//cd with no path and & support
	if( 
		(path[0] == 'c' && path[1] == 'd' && path[2] == '&' ) ||
		(path[0] == 'c' && path[1] == 'd' && path[2] == ' ' && path[3] == '&' && path[4] == '\0' ) 
	)
	{
		if(fork() > 0)
			return;

		else
		{
			if(chdir(home) < 0)
			{
				printf("ERROR CHANGING DIRECTORY TO HOME\n");
				exit(1);
			}
			exit(0);
		}
		
	}

	//cd with no path
	else if(path[0] == 'c' && path[1] == 'd' && path[2] == '\0')
	{
		if(chdir(home) < 0)
		{
			printf("ERROR CHANGING DIRECTORY TO HOME\n");
			exit(1);
		}
	}

	else
	{
		char newpath[500];

		int j=0;
		bool bg = false;
		for(int i = 3 ; path[i] != '\0' ; i++,j++ )
		{
			if(path[i] == ' ' && path[i+1] == '&')
			{
				j--;
				continue;
			}

			if(path[i] == '&' && path[i+1] == '\0')
			{
				bg = true;
				break;
			}

			newpath[j] = path[i];
		}

		newpath[j] = '\0';

		if(bg)
		{
			if(fork() > 0)
				return;
			
			else
			{
				if(chdir(newpath) < 0)
				{
					printf("bash: cd: %s: No such file or directory\n",newpath);
				}
				exit(0);
			}
		}

		if(chdir(newpath) < 0)
		{
			printf("bash: cd: %s: No such file or directory\n",newpath);
		}
	}
	
}

void ShowEnvironment()
{
	extern char ** environ;

	for(int i=0 ; environ[i] != NULL ; i++ )
		printf("%s\n", __environ[i]);
}

void SetEnvironment(char str[])
{
	char name[500];
	char value[500] = "";


	int i=7;
	int j=0;
	bool bg = false;

	while (str[i] != ' ' && str[i] != '\0')
	{
		name[j] = str[i];
		i++;
		j++;

		if(str[i] == '&' && str[i+1] == '\0')
			bg = true; 

		else if(str[i+1] == '&' && str[i+2] == '\0')
			bg = true;
	}

	name[j] = '\0';

	if(bg)
	{
		if(fork() > 0)
			return;
		else
		{
			//checking whether or not variable is already defined
			if(getenv(name))
			{
				printf("Error Changing Value : %s Already Defined\n",name);
				exit(0);
			}

			if(setenv(name,"",0) < 0)
			{
				printf("ERROR SETTING THE ENVIRONMENT VARIABLES\n");
				exit(1);
			}

			exit(0);
		}
		
	}

	if(str[i] == ' ')
	{
		i++;
		j=0;
		while(str[i] != '\0')
		{
			value[j] = str[i];
			i++;
			j++;
		
			if(str[i] == '&' && str[i+1] == '\0')
				bg = true;
		}
		value[j] = '\0';
	}

	if(bg)
	{
		if(fork()>0)
			return;
		else
		{
			//checking whether or not variable is already defined
			if(getenv(name))
			{
				printf("Error Changing Value : %s Already Defined\n",name);
				exit(0);
			}

			if(setenv(name,value,0) < 0)
			{
				printf("ERROR SETTING THE ENVIRONMENT VARIABLES\n");
				exit(1);
			}

			exit(0);
		}
	}

	//checking whether or not variable is already defined
	if(getenv(name))
	{
		printf("Error Changing Value : %s Already Defined\n",name);
		return;
	}

	if(setenv(name,value,0) < 0)
	{
		printf("ERROR SETTING THE ENVIRONMENT VARIABLES\n");
		exit(1);
	}
}

void UnSetEnvironment(char str[])
{
	char name[500];

	int i=9;
	int j=0;
	bool bg = false;
	while (str[i] != '\0')
	{
		name[j] = str[i];
		i++;
		j++;

		if(str[i] == '&' && str[i+1] == '\0')
			bg = true;
	}

	name[j] = '\0';

	if(bg)
	{
		if(fork() > 0)
			return;
		else
		{
			if(getenv(name))
			{
				if(unsetenv(name) < 0)
				{
					printf("ERROR UNDEFINING %s VARIABLE\n",name);
					exit(1);
				}
			}

			else
			{
				printf("Error Undefining Variable : %s Not Found\n",name);
			}

			exit(0);
		}
		
	}

	if(getenv(name))
	{
		if(unsetenv(name) < 0)
		{
			printf("ERROR UNDEFINING %s VARIABLE\n",name);
			exit(1);
		}
	}

	else
	{
		printf("Error Undefining Variable : %s Not Found\n",name);
	}
	
}

void OtherCommands(char command[])
{
	bool bg = false;
	char * args[50] ;
	int arguments = 0;

	char * token = strtok(command," ");

	while(token != NULL)
	{
		if(strcmp(token,"&") == 0)
		{
			bg = true;
			token = strtok(NULL," ");
			continue;
		}

		args[arguments] = token;
		arguments++;
		token = strtok(NULL," ");
	}

	if(args[arguments-1][strlen(args[arguments-1])-1] == '&' && !bg)
	{
		args[arguments-1][strlen(args[arguments-1])-1] = '\0';
		bg = true;
	}

	args[arguments] = NULL;

	int pid = fork();

	if(pid > 0)
	{
		if(bg)
			printf("[1] %d\n",pid);

		if(!bg)
			wait();

		fflush(stdout);
		return;
	}

	else
	{
		setenv("parent",getenv("SHELL"),1);
		execvp(args[0],args);
		printf("%s: command not found\n",command);
		fflush(stdout);
		exit(1);
	}
}

void signalhandle(int sig)
{
	//printing new line
	printf("\n");

	//printing prompt
	Prompt();

	//emptying buffer 
	fflush(stdout); 
}

void OutputRedirection(char str[])
{
	char fullpath[50] = "/bin/";
	bool bg = false;

	int j=5;
	for(int i=0 ; str[i] != ' ' ;i++,j++)
	{
		if(str[i] == '\0')
			break;

		fullpath[j] = str[i];
	}
	fullpath[j] = '\0';


	char * args[50];
	int arguments = 0;
	bool last = false;
	char output[50];

	char * token = strtok(str," ");
	while(token != NULL)
	{
		if(strcmp(token,">") == 0)
		{
			last = true;
			token = strtok(NULL," ");			
			continue;
		}

		if(strcmp(token,"&") == 0)
		{
			bg = true;
			token = strtok(NULL," ");			
			continue;
		}

		if(last)
		{
			if(token[strlen(token)-1] == '&')
			{
				token[strlen(token)-1] = '\0';
				bg = true;
			}

			for(int i=0 ; token[i] != '\0' ;i++)
			{
				output[i] = token[i];
				output[i+1] = token[i+1];
			}
		}	

		else
		{
			args[arguments] = token;
			arguments++;
		}
				
		token = strtok(NULL," ");
	}

	args[arguments] = NULL;

	int backup_stdout = dup(1);
	
	//redirecting the stdout to file
	//here dup2 is not used since output file is created if not created thats why ftn freopen is used and it returns
	//FILE ptr which is not supported in C
	FILE *file = freopen(output, "w",stdout);

	if(file == NULL)
	{
		dup2(backup_stdout,1);
		printf("Error | File Not Created\n");
		return;
	}

	if(fork() > 0)
	{
		if(!bg)
			wait();
	
		dup2(backup_stdout,1);
	}

	else
	{
		execv(fullpath,args);

		char * command2 = fullpath;
		for(int i=5,j=0 ; fullpath[i] != '\0' ; i++,j++)
		{
			command2[j] = fullpath[i];
			command2[j+1] = fullpath[i+1];
		}

		dup2(backup_stdout,1);
		printf("%s: command not found\n",command2);
		fflush(stdout);
		exit(0);
	}
	
}

void InputRedirection(char str[])
{
	char fullpath[50] = "/bin/";
	bool bg = false;

	int j=5;
	for(int i=0 ; str[i] != ' ' ;i++,j++)
	{
		if(str[i] == '\0')
			break;

		fullpath[j] = str[i];
	}
	fullpath[j] = '\0';

	char * args[50];
	int arguments = 0;
	bool last = false;
	char input[50];

	char * token = strtok(str," ");
	while(token != NULL)
	{
		if(strcmp(token,"&") == 0)
		{
			bg = true;
			token = strtok(NULL," ");			
			continue;
		}

		if(strcmp(token,"<") == 0)
		{
			last = true;
			token = strtok(NULL," ");			
			continue;
		}

		if(last)
		{
			if(token[strlen(token)-1] == '&')
			{
				token[strlen(token)-1] = '\0';
				bg = true;
			}

			for(int i=0 ; token[i] != '\0' ;i++)
			{
				input[i] = token[i];
				input[i+1] = token[i+1];
			}
		}	

		else
		{
			args[arguments] = token;
			arguments++;
		}
				
		token = strtok(NULL," ");
	}

	args[arguments] = NULL;

	int backup_stdin = dup(0);
	
	//redirecting the stdinput to file
	int file = open(input, O_RDONLY);

	if(file == -1)
	{
		char * command2 = fullpath;
		for(int i=5,k=0 ; fullpath[i] != '\0' ; i++,k++)
		{
			command2[k] = fullpath[i];
			command2[k+1] = fullpath[i+1];
		}

		printf("%s: %s: No such file or directory\n",command2,input);
		return;
	}

	dup2(file,0);

	if(fork() > 0)
	{
		if(!bg)
			wait();
		
		dup2(backup_stdin,0);
	}

	else
	{
		execv(fullpath,args);
		dup2(backup_stdin,0);

		char * command2 = fullpath;
		for(int i=5,k=0 ; fullpath[i] != '\0' ; i++,k++)
		{
			command2[k] = fullpath[i];
			command2[k+1] = fullpath[i+1];
		}

		printf("%s: command not found\n",command2);
		fflush(stdin);
		exit(0);
	}	
}

void InputOutputRedirection(char str[])
{
	char fullpath[50] = "/bin/";

	bool bg =false;

	int j=5;
	for(int i=0 ; str[i] != ' ' ;i++,j++)
	{
		if(str[i] == '\0')
			break;

		fullpath[j] = str[i];
	}
	fullpath[j] = '\0';

	char * args[50];
	int arguments = 0;
	bool secondlast = false;
	bool last = false;
	char input[50];
	char output[50];

	char * token = strtok(str," ");
	while(token != NULL)
	{
		if(strcmp(token,"<") == 0)
		{
			secondlast = true;
			token = strtok(NULL," ");			
			continue;
		}

		if(strcmp(token,"&") == 0)
		{
			bg = true;
			token = strtok(NULL," ");			
			continue;
		}

		else if(strcmp(token,">") == 0)
		{
			last = true;
			token = strtok(NULL," ");			
			continue;
		}

		if(last)
		{
			if(token[strlen(token)-1] == '&')
			{
				token[strlen(token)-1] = '\0';
				bg = true;
			}

			for(int i=0 ; token[i] != '\0' ;i++)
			{
				output[i] = token[i];
				output[i+1] = token[i+1];
			}
		}	

		else if(secondlast)
		{
			for(int i=0 ; token[i] != '\0' ;i++)
			{
				input[i] = token[i];
				input[i+1] = token[i+1];
			}
		}

		else
		{
			args[arguments] = token;
			arguments++;
		}
				
		token = strtok(NULL," ");
	}

	args[arguments] = NULL;

	int backup_stdin = dup(0);
	int backup_stdout = dup(1);

	//redirecting the stdinput to file
	int file = open(input, O_RDONLY);
	FILE * file2 = freopen(output, "w",stdout);

	if(file == -1)
	{
		char * command2 = fullpath;
		for(int i=5,k=0 ; fullpath[i] != '\0' ; i++,k++)
		{
			command2[k] = fullpath[i];
			command2[k+1] = fullpath[i+1];
		}

		printf("%s: %s: No such file or directory\n",command2,input);
		return;
	}

	if(file2 == NULL)
	{
		dup2(backup_stdout,1);
		printf("Error | File Not Created\n");
		return;
	}

	dup2(file,0);

	if(fork() > 0)
	{
		if(!bg)
			wait();
		dup2(backup_stdin,0);
		dup2(backup_stdout,1);
	}

	else
	{
		execv(fullpath,args);
		dup2(backup_stdin,0);
		dup2(backup_stdout,1);
		char * command2 = fullpath;
		for(int i=5,k=0 ; fullpath[i] != '\0' ; i++,k++)
		{
			command2[k] = fullpath[i];
			command2[k+1] = fullpath[i+1];
		}
		printf("%s: command not found\n",command2);
		fflush(stdin);
		fflush(stdout);
		exit(0);
	}		
}

void PipeRedirection(char str[])
{
	int pipes = 0;
	bool bg =false;
	char str2[500] ;
	int first = 0;
	int second = 1;
	for(int i=0 ; str[i] != '\0' ; i++)
	{
		str2[i] = str[i];
		str2[i+1] = str[i+1];
	}

	char *tok = strtok(str2," ");
	while(tok != NULL)
	{
		if(strcmp(tok,"|") == 0)
			pipes++;

		tok = strtok(NULL," ");
	}

	int ** fd = (int**)malloc(pipes * sizeof(int *)) ;
	for(int i=0 ; i<pipes ; i++)
		fd[i] = (int*)malloc(2*sizeof(int));

	for(int i=0 ; i<pipes ; i++)
		pipe(fd[i]);

	char ** fullpath = (char **)malloc( (pipes+1) * sizeof(char*) );
	for(int i=0 ; i<pipes+1 ; i++)
		fullpath[i] = (char*)malloc(50 * sizeof(char));

	for(int i=0 ; i<pipes+1 ; i++)
	{
		fullpath[i][0] = '/';
		fullpath[i][1] = 'b';
		fullpath[i][2] = 'i';
		fullpath[i][3] = 'n';
		fullpath[i][4] = '/';
		fullpath[i][5] = '\0';
	}

	char *** args = (char***)malloc( (pipes+1) * sizeof(char **) );
	for(int i=0 ; i<pipes+1 ; i++)
		args[i] = (char**)malloc(50 * sizeof(char *));

	int * arguments = (int*)malloc( (pipes+1) * sizeof(int));
	for(int i=0 ; i<pipes+1 ; i++)
		arguments[i] = 0;

	bool firs = false;
	bool last = false;
	bool last2 = true;
	bool again = true;
	bool remove_commas = false;

	char * token = strtok(str," ");

	int backup_stdin = dup(0);
	int backup_stdout = dup(1);

	for(int l = 0 ; l < pipes ; l++)
	{
		while(token != NULL && again)
		{
			if(strcmp(token,"grep") == 0)
				remove_commas = true;

			if(!firs)
			{
				firs = true;

				args[first][arguments[first]] = token;
				arguments[first]++;

				int j=5;
				for(int i=0 ; token[i] != ' ' ;i++,j++)
				{
					if(token[i] == '\0')
						break;

					fullpath[first][j] = token[i];
				}
				fullpath[first][j] = '\0';
			}

			else if(strcmp(token,"|") == 0)
			{
				if(last)
				{
					firs = true;
					last = true;
					last2 = true;
					again = false;
					token = strtok(NULL," ");			
					continue;
				}

				last = true;
				token = strtok(NULL," ");			
				continue;
			}

			else if(strcmp(token,"&") == 0)
			{
				bg = true;
				token = strtok(NULL," ");			
				continue;
			}

			else if(last)
			{
				if(l == pipes - 1 && token[strlen(token)-1] == '&')
				{
					token[strlen(token)-1] = '\0';
					bg = true;
				}

				if(last2)
				{
					last2 = false;
					args[second][arguments[second]] = token;
					arguments[second]++;

					int j=5;
					for(int i=0 ; token[i] != ' ' ;i++,j++)
					{
						if(token[i] == '\0')
							break;

						fullpath[second][j] = token[i];
					}
					fullpath[second][j] = '\0';
				}

				else if(!last2)
				{
					if(remove_commas && token[0] == '"')
					{
						int i=0;
						do
						{
							if(token[i+1] == '"' && token[i+2] == '\0')
							{
								token[i] = '\0';
								break;
							}

							token[i] = token[i+1];
							i++;
						} while (token[0] != '"');
						
					}

					args[second][arguments[second]] = token;
					arguments[second]++;
				}
			
			}	

			else
			{
				args[first][arguments[first]] = token;
				arguments[first]++;
			}
				
			token = strtok(NULL," ");
		}

		args[first][arguments[first]] = NULL;
		args[second][arguments[second]] = NULL;

		dup2(fd[first][0],0);

		if(l == 0 && l == pipes-1)
			dup2(fd[first][1],1);

		else if(l == pipes-1 && l != 0)
			dup2(backup_stdout,1);

		else if (l != pipes-1)
			dup2(fd[second][1],1);

		int temp_ret = 0;

		if(l > 0)
		{
			//second parent
			if(fork() > 0)
			{
				if(l == pipes-1 && bg)
				{
					dup2(backup_stdin,0);
					dup2(backup_stdout,1);
					fflush(stdout);
					fflush(stdin);
					return;		
				}

				wait(&temp_ret);

				if(temp_ret != 0)
				{
					dup2(backup_stdin,0);
					dup2(backup_stdout,1);
					fflush(stdout);
					fflush(stdin);
					return;		
				}

				//sucessfull second execution

				if(l == pipes-1)
					dup2(backup_stdout,1);

				else
					dup2(fd[second][1],1);

				if( l != pipes-1)
					close(fd[second][1]);
			}

			//second child
			else
			{
				execv(fullpath[second],args[second]);
				dup2(backup_stdout,1);
				char * command2 = fullpath[second];
				for(int i=5,k=0 ; fullpath[second][i] != '\0' ; i++,k++)
				{
					command2[k] = fullpath[second][i];
					command2[k+1] = fullpath[second][i+1];
				}
				printf("%s: command not found\n",command2);
				fflush(stdin);
				fflush(stdout);
				exit(1);
			}
		}

		else
		{
			dup2(fd[first][1],1);

			//parent					
			if(fork() > 0)
			{				
				wait(&temp_ret);

				//succesfull first execution
				if(temp_ret == 0)
				{	
					if(l == pipes-1)
						dup2(backup_stdout,1);

					else
						dup2(fd[second][1],1);

					close(fd[first][1]);

					//second parent
					if(fork() > 0)
					{
						if(l == pipes-1 && bg)
						{
							dup2(backup_stdin,0);
							dup2(backup_stdout,1);
							fflush(stdout);
							fflush(stdin);
							return;		
						}
			
						wait(&temp_ret);

						//sucessfull second execution
						dup2(backup_stdin,0);

						if(l != pipes-1)
							close(fd[second][1]);
					}

					//second child
					else
					{
						execv(fullpath[second],args[second]);
						dup2(backup_stdout,1);
						char * command2 = fullpath[second];
						for(int i=5,k=0 ; fullpath[second][i] != '\0' ; i++,k++)
						{
							command2[k] = fullpath[second][i];
							command2[k+1] = fullpath[second][i+1];
						}
						printf("%s: command not found\n",command2);
						fflush(stdin);
						fflush(stdout);
						exit(1);	
					}
				
				}

				else
				{
					dup2(backup_stdin,0);
					dup2(backup_stdout,1);
					fflush(stdout);
					fflush(stdin);
					return;		
				}
			}

			//child 1
			else
			{
				execv(fullpath[first],args[first]);
				dup2(backup_stdout,1);
				char * command2 = fullpath[first];
				for(int i=5,k=0 ; fullpath[first][i] != '\0' ; i++,k++)
				{
					command2[k] = fullpath[first][i];
					command2[k+1] = fullpath[first][i+1];
				}
				printf("%s: command not found\n",command2);
				fflush(stdin);
				fflush(stdout);
				exit(1);
			}
		}

		first++;
		second++;
		again = true;
	}	

	dup2(backup_stdout,1);
	dup2(backup_stdin,0);
	fflush(stdin);
	fflush(stdout);
}
