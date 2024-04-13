reg_nextline(void)
{
    rex.line = reg_getline(++rex.lnum);
    rex.input = rex.line;
    fast_breakcheck();
}