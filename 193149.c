fill5 (unsigned short data[/*n*/], int n)
{
    for (int i = 0; i < n; ++i)
	data[i] = 0;

    int j = 0, k = 0;

    for (int i = 0; i < n; ++i)
    {
	data[i] = j;
	j = j + k;
	k = k + 1;

	if (j > USHRT_MAX)
	    break;
    }
}