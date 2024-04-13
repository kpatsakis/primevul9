_zip_read2(unsigned char **a)
{
    unsigned short ret;

    ret = (*a)[0]+((*a)[1]<<8);
    *a += 2;

    return ret;
}