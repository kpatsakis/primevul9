static int read_wrapper_data (WavpackContext *wpc, WavpackMetadata *wpmd)
{
    if ((wpc->open_flags & OPEN_WRAPPER) && wpc->wrapper_bytes < MAX_WRAPPER_BYTES && wpmd->byte_length) {
        wpc->wrapper_data = (unsigned char *)realloc (wpc->wrapper_data, wpc->wrapper_bytes + wpmd->byte_length);
	if (!wpc->wrapper_data)
	    return FALSE;
        memcpy (wpc->wrapper_data + wpc->wrapper_bytes, wpmd->data, wpmd->byte_length);
        wpc->wrapper_bytes += wpmd->byte_length;
    }

    return TRUE;
}