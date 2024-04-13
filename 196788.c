static int _zzip_strcasecmp(char* __zzip_restrict a, char* _zzip_restrict b)
{
    if (! a) return (b) ? 1 : 0;
    if (! b) return -1;
    while (1) 
    {
	int v = tolower(*a) - tolower(*b);
	if (v) return v;
	if (! *a) return 1;
	if (! *b) return -1;
	a++; b++;
    }
}