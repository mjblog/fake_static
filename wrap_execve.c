#define _GNU_SOURCE
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dlfcn.h>
#include <libgen.h>

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

#define LDWRAP_NAME "ldwrap-sh"
__attribute__ ((constructor)) static void setup_path ()
{
	real_execve = dlsym(RTLD_NEXT, "execve");
	assert (real_execve != NULL);

	real_execvp = dlsym(RTLD_NEXT, "execvp");
	assert (real_execve != NULL);

	wrap_ld = getenv ("__WRAP_LD_PATH");
	if (wrap_ld == NULL)
	{
		Dl_info info;
		int ret = dladdr (&__FUNCTION__, &info);
		assert (ret != 0);
		char *abspath = realpath(info.dli_fname, NULL);
		wrap_ld = malloc (strlen (abspath) + strlen (LDWRAP_NAME));
		sprintf ( (char *)wrap_ld, "%s/%s", dirname (abspath), LDWRAP_NAME);
	}
	return;
}


int execv (const char *path, char *const argv[])
{
	return execve (path, argv, environ);
}

static int argv_count (char *const argv[])
{
	int i = 0;

	while (argv[i] != NULL)
		i++;

	return i;
}

int execvp (const char *file, char *const argv[])
{
	if (is_linker (file))
	{
	//把原来的argv0 dup一次插入到argv的头部
		int argc = argv_count (argv);
		char ** new_argv = malloc ((argc + 2) * sizeof (char *));
		assert (new_argv != NULL);
		memcpy (new_argv + 1, argv, argc * sizeof (char *));
		new_argv[0] = argv[0];
//		sleep(20);
		return real_execvp (wrap_ld, new_argv);
	}
	else
	{
		return real_execvp (file, argv);;
	}

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
