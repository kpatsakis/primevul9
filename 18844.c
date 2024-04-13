termcode_star(char_u *code, int len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    if (len >= 3 && code[len - 2] == '*')
    {
	if (len >= 5 && code[len - 3] == ';')
	    return 2;
	else
	    return 1;
    }
    return 0;
}