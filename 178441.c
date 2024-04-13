mb_decompose(int c, int *c1, int *c2, int *c3)
{
    decomp_T d;

    if (c >= 0xfb20 && c <= 0xfb4f)
    {
	d = decomp_table[c - 0xfb20];
	*c1 = d.a;
	*c2 = d.b;
	*c3 = d.c;
    }
    else
    {
	*c1 = c;
	*c2 = *c3 = 0;
    }
}