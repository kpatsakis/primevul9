void adv_png_unfilter_8(unsigned width, unsigned height, unsigned char* p, unsigned line)
{
	unsigned i, j;

	for(i=0;i<height;++i) {
		unsigned char f = *p++;

		if (f == 0) { /* none */
			p += width;
		} else if (f == 1) { /* sub */
			++p;
			for(j=1;j<width;++j) {
				p[0] += p[-1];
				++p;
			}
		} else if (f == 2) { /* up */
			if (i) {
				unsigned char* u = p - line;
				for(j=0;j<width;++j) {
					*p += *u;
					++p;
					++u;
				}
			} else {
				p += width;
			}
		} else if (f == 3) { /* average */
			if (i) {
				unsigned char* u = p - line;
				p[0] += u[0] / 2;
				++p;
				++u;
				for(j=1;j<width;++j) {
					unsigned a = (unsigned)u[0] + (unsigned)p[-1];
					p[0] += a >> 1;
					++p;
					++u;
				}
			} else {
				++p;
				for(j=1;j<width;++j) {
					p[0] += p[-1] / 2;
					++p;
				}
			}
		} else if (f == 4) { /* paeth */
			unsigned char* u = p - line;
			for(j=0;j<width;++j) {
				unsigned a, b, c;
				int v;
				int da, db, dc;
				a = j<1 ? 0 : p[-1];
				b = i<1 ? 0 : u[0];
				c = (j<1 || i<1) ? 0 : u[-1];
				v = a + b - c;
				da = v - a;
				if (da < 0)
					da = -da;
				db = v - b;
				if (db < 0)
					db = -db;
				dc = v - c;
				if (dc < 0)
					dc = -dc;
				if (da <= db && da <= dc)
					p[0] += a;
				else if (db <= dc)
					p[0] += b;
				else
					p[0] += c;
				++p;
				++u;
			}
		}

		p += line - width - 1;
	}
}