#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>

static const char *wrap_ld = NULL;
static  int (*real_execve) (const char *filename, char *const argv[],  char *const envp[]) = NULL;
static  int (*real_execvp) (const char *file, char *const argv[]) = NULL;
static int is_linker (const char *prog)
{
	const char *file = strrchr(prog, '/');
	if (file == NULL)
		file = prog;
	else
		file++;

	const char * skip_cross_prefix = strrchr(file, '-');
	if (skip_cross_prefix == NULL)
		skip_cross_prefix = file;
	else
		skip_cross_prefix++;

	const char * linker_names[] = {"ld", "ld.bfd", "ld.gold", NULL};
	int i = 0;
	int ret = 0;
	while (linker_names[i] != NULL)
	{
		if (strcmp (linker_names[i], skip_cross_prefix) == 0)
		{
			ret = 1;
			break;
		}
		i++;
	}

	return ret;
}


__attribute__ ((constructor)) static void setup_path ()
{
	wrap_ld = getenv ("__WRAP_LD_PATH");
	assert (wrap_ld != NULL);

	real_execve = dlsym(RTLD_NEXT, "execve");
	assert (real_execve != NULL);

	real_execvp = dlsym(RTLD_NEXT, "execvp");
	assert (real_execve != NULL);

	return;
}


int execv (const char *path, char *const argv[])
{
	return execve (path, argv, environ);
}

int execvp (const char *file, char *const argv[])
{
//	sleep (10);
	const char * exe_name;

	if (is_linker (file))
		exe_name = wrap_ld;
	else
		exe_name = file;

	return real_execvp (exe_name, argv);
}

#if 0
int execve (const char *filename, char *const argv[],  char *const envp[])
{

	printf ("executing %s\n", exe_name);
	real_execve (exe_name, argv, envp);

	assert (0);
	return 0;
}
#endif
