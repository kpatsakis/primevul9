static inline double myround(double a)
{
    if (a < 0)
	return a-0.5;
    return a+0.5;
}