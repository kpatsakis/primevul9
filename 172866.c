static void my_emit_message(j_common_ptr cinfo, int msg_level)
{
	my_error_ptr myerr=(my_error_ptr)cinfo->err;
	myerr->emit_message(cinfo, msg_level);
	if(msg_level<0)
	{
		myerr->warning=TRUE;
		if(myerr->stopOnWarning) longjmp(myerr->setjmp_buffer, 1);
	}
}