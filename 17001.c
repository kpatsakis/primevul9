format_clsid(char *buf, size_t len, const uint64_t uuid[2]) {
	snprintf(buf, len, "%.8" PRIx64 "-%.4" PRIx64 "-%.4" PRIx64 "-%.4" 
	    PRIx64 "-%.12" PRIx64,
	    (uuid[0] >> 32) & (uint64_t)0x000000000ffffffffLLU,
	    (uuid[0] >> 16) & (uint64_t)0x0000000000000ffffLLU,
	    (uuid[0] >>  0) & (uint64_t)0x0000000000000ffffLLU, 
	    (uuid[1] >> 48) & (uint64_t)0x0000000000000ffffLLU,
	    (uuid[1] >>  0) & (uint64_t)0x0000fffffffffffffLLU);
	return buf;
}