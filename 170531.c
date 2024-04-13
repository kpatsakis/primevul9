static void dump_header(const char *name, const char *value, const char *raw, void *rock)
{
    FILE *f = (FILE *)rock;

    if (raw) fputs(raw, f);
    else fprintf(f, "%s: %s\r\n", name, value);
}