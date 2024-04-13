reg_iswordc(int c)
{
    return vim_iswordc_buf(c, rex.reg_buf);
}