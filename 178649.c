to_callback_empty_output_buffer (j_compress_ptr cinfo)
{
	ToFunctionDestinationManager *destmgr;

	destmgr	= (ToFunctionDestinationManager*) cinfo->dest;
	to_callback_do_write (cinfo, TO_FUNCTION_BUF_SIZE);
	destmgr->pub.next_output_byte = destmgr->buffer;
	destmgr->pub.free_in_buffer = TO_FUNCTION_BUF_SIZE;
	return TRUE;
}