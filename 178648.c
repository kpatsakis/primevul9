to_callback_terminate (j_compress_ptr cinfo)
{
	ToFunctionDestinationManager *destmgr;

	destmgr	= (ToFunctionDestinationManager*) cinfo->dest;
	to_callback_do_write (cinfo, TO_FUNCTION_BUF_SIZE - destmgr->pub.free_in_buffer);
}