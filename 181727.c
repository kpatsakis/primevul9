static int cmpname (const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	return (int)strcmp (a->name, b->name);
}