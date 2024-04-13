int ImagingLibTiffSetField(ImagingCodecState state, ttag_t tag, ...){
    // after tif_dir.c->TIFFSetField.
    TIFFSTATE *clientstate = (TIFFSTATE *)state->context;
    va_list ap;
    int status;

    va_start(ap, tag);
    status = TIFFVSetField(clientstate->tiff, tag, ap);
    va_end(ap);
    return status;
}