static void print_trampolines(RCore *core, ut64 a, ut64 b, size_t element_size) {
	int i;
	for (i = 0; i < core->blocksize; i += element_size) {
		ut32 n;
		memcpy (&n, core->block + i, sizeof (ut32));
		if (n >= a && n <= b) {
			if (element_size == 4) {
				r_cons_printf ("f trampoline.%x @ 0x%" PFMT64x "\n", n, core->offset + i);
			} else {
				r_cons_printf ("f trampoline.%" PFMT64x " @ 0x%" PFMT64x "\n", n, core->offset + i);
			}
			r_cons_printf ("Cd %u @ 0x%" PFMT64x ":%u\n", element_size, core->offset + i, element_size);
			// TODO: add data xrefs
		}
	}
}