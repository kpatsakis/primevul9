static void fast_mix(struct fast_pool *f)
{
	__u32 a = f->pool[0],	b = f->pool[1];
	__u32 c = f->pool[2],	d = f->pool[3];

	a += b;			c += d;
	b = rol32(b, 6);	d = rol32(d, 27);
	d ^= a;			b ^= c;

	a += b;			c += d;
	b = rol32(b, 16);	d = rol32(d, 14);
	d ^= a;			b ^= c;

	a += b;			c += d;
	b = rol32(b, 6);	d = rol32(d, 27);
	d ^= a;			b ^= c;

	a += b;			c += d;
	b = rol32(b, 16);	d = rol32(d, 14);
	d ^= a;			b ^= c;

	f->pool[0] = a;  f->pool[1] = b;
	f->pool[2] = c;  f->pool[3] = d;
	f->count++;
}