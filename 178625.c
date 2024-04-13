to_callback_init (j_compress_ptr cinfo)
{
	ToFunctionDestinationManager *destmgr;

	destmgr	= (ToFunctionDestinationManager*) cinfo->dest;
	destmgr->pub.next_output_byte = destmgr->buffer;
	destmgr->pub.free_in_buffer = TO_FUNCTION_BUF_SIZE;
}