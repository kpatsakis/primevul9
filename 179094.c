int ImagingLibTiffInit(ImagingCodecState state, int fp, int offset) {
    TIFFSTATE *clientstate = (TIFFSTATE *)state->context;

    TRACE(("initing libtiff\n"));
    TRACE(("filepointer: %d \n",  fp));
    TRACE(("State: count %d, state %d, x %d, y %d, ystep %d\n", state->count, state->state,
           state->x, state->y, state->ystep));
    TRACE(("State: xsize %d, ysize %d, xoff %d, yoff %d \n", state->xsize, state->ysize,
           state->xoff, state->yoff));
    TRACE(("State: bits %d, bytes %d \n", state->bits, state->bytes));
    TRACE(("State: context %p \n", state->context));

    clientstate->loc = 0;
    clientstate->size = 0;
    clientstate->data = 0;
    clientstate->fp = fp;
    clientstate->ifd = offset;
    clientstate->eof = 0;

    return 1;
}