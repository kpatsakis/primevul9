static const char *syscallNumber(int n) {
	return sdb_fmt (n > 1000 ? "0x%x" : "%d", n);
}