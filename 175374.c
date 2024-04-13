get_packer(ImagingEncoderObject* encoder, const char* mode,
           const char* rawmode)
{
    int bits;
    ImagingShuffler pack;

    pack = ImagingFindPacker(mode, rawmode, &bits);
    if (!pack) {
	Py_DECREF(encoder);
	PyErr_SetString(PyExc_SystemError, "unknown raw mode");
	return -1;
    }

    encoder->state.shuffle = pack;
    encoder->state.bits = bits;

    return 0;
}