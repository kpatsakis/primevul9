OOM(void)
{
    fprintf(stderr, "Ran out of memory needs > %d bytes\n", maxmem);
    progresult = XMLLINT_ERR_MEM;
}