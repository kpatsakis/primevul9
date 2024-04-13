fatal_error_handler (j_common_ptr cinfo)
{
	struct error_handler_data *errmgr;
        char buffer[JMSG_LENGTH_MAX];
        
	errmgr = (struct error_handler_data *) cinfo->err;
        
        /* Create the message */
        (* cinfo->err->format_message) (cinfo, buffer);

        /* broken check for *error == NULL for robustness against
         * crappy JPEG library
         */
        if (errmgr->error && *errmgr->error == NULL) {
                g_set_error (errmgr->error,
                             GDK_PIXBUF_ERROR,
                             cinfo->err->msg_code == JERR_OUT_OF_MEMORY 
			     ? GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY 
			     : GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                             _("Error interpreting JPEG image file (%s)"),
                             buffer);
        }
        
	siglongjmp (errmgr->setjmp_buffer, 1);

        g_assert_not_reached ();
}