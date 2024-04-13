dir_s_chdir(int argc, VALUE *argv, VALUE obj)
{
    VALUE path = Qnil;

    if (rb_check_arity(argc, 0, 1) == 1) {
        path = rb_str_encode_ospath(rb_get_path(argv[0]));
    }
    else {
	const char *dist = getenv("HOME");
	if (!dist) {
	    dist = getenv("LOGDIR");
	    if (!dist) rb_raise(rb_eArgError, "HOME/LOGDIR not set");
	}
	path = rb_str_new2(dist);
    }

    if (chdir_blocking > 0) {
	if (!rb_block_given_p() || rb_thread_current() != chdir_thread)
	    rb_warn("conflicting chdir during another chdir block");
    }

    if (rb_block_given_p()) {
	struct chdir_data args;

	args.old_path = rb_str_encode_ospath(rb_dir_getwd());
	args.new_path = path;
	args.done = FALSE;
	return rb_ensure(chdir_yield, (VALUE)&args, chdir_restore, (VALUE)&args);
    }
    else {
	char *p = RSTRING_PTR(path);
	int r = (int)(VALUE)rb_thread_call_without_gvl(nogvl_chdir, p,
							RUBY_UBF_IO, 0);
	if (r < 0)
	    rb_sys_fail_path(path);
    }

    return INT2FIX(0);
}