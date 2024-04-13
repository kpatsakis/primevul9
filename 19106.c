static int __addrs_cmp(void *_a, void *_b) {
	ut64 a = r_num_get (NULL, _a);
	ut64 b = r_num_get (NULL, _b);
	if (a > b) {
		return 1;
	}
	if (a < b) {
		return -1;
	}
	return 0;
}