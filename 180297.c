_zip_write4(unsigned int i, FILE *fp)
{
    putc(i&0xff, fp);
    putc((i>>8)&0xff, fp);
    putc((i>>16)&0xff, fp);
    putc((i>>24)&0xff, fp);
    
    return;
}