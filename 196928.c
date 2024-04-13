static int myClose(void *context) {
    FILE *f = (FILE *) context;
    if (f == stdin)
        return(0);
    return(fclose(f));
}