tsize_t _tiffWriteProc(thandle_t hdata, tdata_t buf, tsize_t size) {
	TIFFSTATE *state = (TIFFSTATE *)hdata;
	tsize_t to_write;

	TRACE(("_tiffWriteProc: %d \n", (int)size));
	dump_state(state);

	to_write = min(size, state->size - (tsize_t)state->loc);
	if (state->flrealloc && size>to_write) {
		tdata_t new;
		tsize_t newsize=state->size;
		while (newsize < (size + state->size)) {
            if (newsize > INT_MAX - 64*1024){
                return 0;
            }
			newsize += 64*1024;
			// newsize*=2; // UNDONE, by 64k chunks?
		}
		TRACE(("Reallocing in write to %d bytes\n", (int)newsize));
        /* malloc check ok, overflow checked above */
		new = realloc(state->data, newsize);
		if (!new) {
			// fail out
			return 0;
		}
		state->data = new;
		state->size = newsize;
		to_write = size;
	}

	TRACE(("to_write: %d\n", (int)to_write));

	_TIFFmemcpy((UINT8 *)state->data + state->loc, buf, to_write);
	state->loc += (toff_t)to_write;
	state->eof = max(state->loc, state->eof);

	dump_state(state);
	return to_write;
}