#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,  char *const argv[], char *const envp[])
{
	const char *wrap_ld = getenv ("__WRAP_LD_PATH");
	assert (wrap_ld != NULL);
	char *wrap_ld_sh = malloc (strlen (wrap_ld) + 10);
	strcpy (wrap_ld_sh, wrap_ld);
	strcat (wrap_ld_sh, "-sh");
//把原来的argv0 dup一次插入到argv的头部
	char ** new_argv = malloc ((argc + 2) * sizeof (char *));
	assert (new_argv != NULL);
	memcpy (new_argv + 1, argv, argc * sizeof (char *));
	new_argv[0] = argv[0];

	int i = 0;
	while (new_argv[i] != NULL)
	{
		printf ("new_argv[%d] = %s", i , new_argv[i]);
		i++;
	}
	return execve (wrap_ld_sh, new_argv, envp);
}
