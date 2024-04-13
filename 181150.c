void ipc_print_perms(FILE *f, struct ipc_stat *is)
{
	struct passwd *pw;
	struct group *gr;

	fprintf(f, "%-10d %-10o", is->id, is->mode & 0777);

	if ((pw = getpwuid(is->cuid)))
		fprintf(f, " %-10s", pw->pw_name);
	else
		fprintf(f, " %-10u", is->cuid);

	if ((gr = getgrgid(is->cgid)))
		fprintf(f, " %-10s", gr->gr_name);
	else
		fprintf(f, " %-10u", is->cgid);

	if ((pw = getpwuid(is->uid)))
		fprintf(f, " %-10s", pw->pw_name);
	else
		fprintf(f, " %-10u", is->uid);

	if ((gr = getgrgid(is->gid)))
		fprintf(f, " %-10s\n", gr->gr_name);
	else
		fprintf(f, " %-10u\n", is->gid);
}