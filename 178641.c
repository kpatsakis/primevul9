to_callback_do_write (j_compress_ptr cinfo, gsize length)
{
	ToFunctionDestinationManager *destmgr;

	destmgr	= (ToFunctionDestinationManager*) cinfo->dest;
        if (!destmgr->save_func ((gchar *)destmgr->buffer,
				 length,
				 destmgr->error,
				 destmgr->user_data)) {
		struct error_handler_data *errmgr;
        
		errmgr = (struct error_handler_data *) cinfo->err;
		/* Use a default error message if the callback didn't set one,
		 * which it should have.
		 */
		if (errmgr->error && *errmgr->error == NULL) {
			g_set_error_literal (errmgr->error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                             "write function failed");
		}
		siglongjmp (errmgr->setjmp_buffer, 1);
		g_assert_not_reached ();
        }
}