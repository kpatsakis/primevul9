save_file_ff(buf_T *buf)
{
    buf->b_start_ffc = *buf->b_p_ff;
    buf->b_start_eol = buf->b_p_eol;
#ifdef FEAT_MBYTE
    buf->b_start_bomb = buf->b_p_bomb;

    /* Only use free/alloc when necessary, they take time. */
    if (buf->b_start_fenc == NULL
			     || STRCMP(buf->b_start_fenc, buf->b_p_fenc) != 0)
    {
	vim_free(buf->b_start_fenc);
	buf->b_start_fenc = vim_strsave(buf->b_p_fenc);
    }
#endif
}