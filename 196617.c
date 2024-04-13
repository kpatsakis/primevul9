chdir_yield(VALUE v)
{
    struct chdir_data *args = (void *)v;
    dir_chdir(args->new_path);
    args->done = TRUE;
    chdir_blocking++;
    if (chdir_thread == Qnil)
	chdir_thread = rb_thread_current();
    return rb_yield(args->new_path);
}