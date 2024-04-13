static void pstr(const char *s)
{
	static const char *HEX = "0123456789ABCDEF";
	Rune c;
	pc(minify ? '\'' : '"');
	while (*s) {
		s += chartorune(&c, s);
		switch (c) {
		case '\'': ps("\\'"); break;
		case '"': ps("\\\""); break;
		case '\\': ps("\\\\"); break;
		case '\b': ps("\\b"); break;
		case '\f': ps("\\f"); break;
		case '\n': ps("\\n"); break;
		case '\r': ps("\\r"); break;
		case '\t': ps("\\t"); break;
		default:
			if (c < ' ' || c > 127) {
				ps("\\u");
				pc(HEX[(c>>12)&15]);
				pc(HEX[(c>>8)&15]);
				pc(HEX[(c>>4)&15]);
				pc(HEX[c&15]);
			} else {
				pc(c); break;
			}
		}
	}
	pc(minify ? '\'' : '"');
}