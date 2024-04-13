_zip_write2(unsigned short i, FILE *fp)
{
    putc(i&0xff, fp);
    putc((i>>8)&0xff, fp);

    return;
}