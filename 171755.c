write_pid_file(const char *path)
{
    FILE *file;
    unsigned long pid;

    file = fopen(path, "w");
    if (file == NULL)
        return errno;
    pid = (unsigned long) getpid();
    if (fprintf(file, "%ld\n", pid) < 0 || fclose(file) == EOF)
        return errno;
    return 0;
}