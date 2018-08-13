#include <sys/types.h>
#include <sys/wait.h>
void wait_child (pid_t child)
{
	int wstatus;
	pid_t w;

	while (1)
	{
		w = waitpid (child, &wstatus, 0);
		if (w == -1) {
			perror ("waitpid");
			exit(1);
		}

		if (WIFEXITED(wstatus)) 
		{
//			printf ("child exited, status=%d\n", WEXITSTATUS(wstatus));
			return;
		}
	
		if (WIFSIGNALED(wstatus))
		{
			printf ("child killed by signal %d\n", WTERMSIG(wstatus));
			exit (1);
		}
	}
}

int main (int argc, char *argv[], char *envp[])
{
	pid_t pid;
	pid = fork ();
	if (pid)
	{
		wait_child (pid);
	}
	else
	{
		execve (argv[0], argv, envp);

	}
}
