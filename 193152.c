fill2 (unsigned short data[/*n*/], int n, int m, IMATH_NAMESPACE::Rand48 & rand48)
{
    for (int i = 0; i < n; ++i)
	data[i] = 0;

    for (int i = 0; i < m; ++i)
	data[rand48.nexti() % n] = (unsigned short) (rand48.nextf() * (USHRT_MAX + 1));
}