fill4 (unsigned short data[/*n*/], int n)
{
    for (int i = 0; i < n; ++i)
	data[i] = i & USHRT_MAX;
}