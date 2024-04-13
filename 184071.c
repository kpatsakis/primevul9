QPDFObjectHandle::pipeStreamData(Pipeline* p,
                                 unsigned long encode_flags,
                                 qpdf_stream_decode_level_e decode_level,
                                 bool suppress_warnings, bool will_retry)
{
    assertStream();
    return dynamic_cast<QPDF_Stream*>(m->obj.getPointer())->pipeStreamData(
	p, encode_flags, decode_level, suppress_warnings, will_retry);
}