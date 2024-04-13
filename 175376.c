_decode_cleanup(ImagingDecoderObject* decoder, PyObject* args)
{
    int status = 0;

    if (decoder->cleanup){
        status = decoder->cleanup(&decoder->state);
    }

    return Py_BuildValue("i", status);
}