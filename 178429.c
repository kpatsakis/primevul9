getchr(void)
{
    int chr = peekchr();

    skipchr();
    return chr;
}