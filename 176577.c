static inline void clear_table(unsigned long *s, unsigned long val, size_t n)
{
	typedef struct { char _[n]; } addrtype;

	*s = val;
	n = (n / 256) - 1;
	asm volatile(
		"	mvc	8(248,%0),0(%0)\n"
		"0:	mvc	256(256,%0),0(%0)\n"
		"	la	%0,256(%0)\n"
		"	brct	%1,0b\n"
		: "+a" (s), "+d" (n), "=m" (*(addrtype *) s)
		: "m" (*(addrtype *) s));
}