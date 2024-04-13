vim_iswordc(int c)
{
    return vim_iswordc_buf(c, curbuf);
}