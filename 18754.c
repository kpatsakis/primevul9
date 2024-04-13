static void __attribute__((__noreturn__)) print_version(void)
{
	const char *ver = NULL;
	const char **features = NULL, **p;

	mnt_get_library_version(&ver);
	mnt_get_library_features(&features);

	printf(_("%s from %s (libmount %s"),
			program_invocation_short_name,
			PACKAGE_STRING,
			ver);
	p = features;
	while (p && *p) {
		fputs(p == features ? ": " : ", ", stdout);
		fputs(*p++, stdout);
	}
	fputs(")\n", stdout);
	exit(MOUNT_EX_SUCCESS);
}