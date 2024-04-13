_compare_func(const void* x, const void *y) {
	const MTPProperties *px = x;
	const MTPProperties *py = y;

	return px->ObjectHandle - py->ObjectHandle;
}