int ImagingLibTiffEncodeInit(ImagingCodecState state, char *filename, int fp) {
	// Open the FD or the pointer as a tiff file, for writing.
	// We may have to do some monkeying around to make this really work.
	// If we have a fp, then we're good.
	// If we have a memory string, we're probably going to have to malloc, then
	// shuffle bytes into the writescanline process.
	// Going to have to deal with the directory as well.

	TIFFSTATE *clientstate = (TIFFSTATE *)state->context;
	int bufsize = 64*1024;
	char *mode = "w";

    TRACE(("initing libtiff\n"));
	TRACE(("Filename %s, filepointer: %d \n", filename,  fp));
	TRACE(("State: count %d, state %d, x %d, y %d, ystep %d\n", state->count, state->state,
		   state->x, state->y, state->ystep));
	TRACE(("State: xsize %d, ysize %d, xoff %d, yoff %d \n", state->xsize, state->ysize,
		   state->xoff, state->yoff));
	TRACE(("State: bits %d, bytes %d \n", state->bits, state->bytes));
	TRACE(("State: context %p \n", state->context));

	clientstate->loc = 0;
	clientstate->size = 0;
	clientstate->eof =0;
	clientstate->data = 0;
	clientstate->flrealloc = 0;
	clientstate->fp = fp;

	state->state = 0;

	if (fp) {
		TRACE(("Opening using fd: %d for writing \n",clientstate->fp));
		clientstate->tiff = TIFFFdOpen(clientstate->fp, filename, mode);
	} else {
		// malloc a buffer to write the tif, we're going to need to realloc or something if we need bigger.
		TRACE(("Opening a buffer for writing \n"));
        /* malloc check ok, small constant allocation */
		clientstate->data = malloc(bufsize);
		clientstate->size = bufsize;
		clientstate->flrealloc=1;

		if (!clientstate->data) {
			TRACE(("Error, couldn't allocate a buffer of size %d\n", bufsize));
			return 0;
		}

		clientstate->tiff = TIFFClientOpen(filename, mode,
										   (thandle_t) clientstate,
										   _tiffReadProc, _tiffWriteProc,
										   _tiffSeekProc, _tiffCloseProc, _tiffSizeProc,
										   _tiffNullMapProc, _tiffUnmapProc); /*force no mmap*/

	}

	if (!clientstate->tiff) {
		TRACE(("Error, couldn't open tiff file\n"));
		return 0;
	}

    return 1;

}