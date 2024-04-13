static int myRead(void *f, char *buf, int len) {
    return(fread(buf, 1, len, (FILE *) f));
}