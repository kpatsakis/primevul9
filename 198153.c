xmlExpHashNameComputeKey(const xmlChar *name) {
    unsigned short value = 0L;
    char ch;

    if (name != NULL) {
	value += 30 * (*name);
	while ((ch = *name++) != 0) {
	    value = value ^ ((value << 5) + (value >> 3) + (unsigned long)ch);
	}
    }
    return (value);
}