fill1 (unsigned short data[/*n*/], int n, float bias, IMATH_NAMESPACE::Rand48 & rand48)
{
    for (int i = 0; i < n; ++i)
	data[i] = (unsigned short)
		  (pow (rand48.nextf(), double(bias)) * (USHRT_MAX + 1));
}