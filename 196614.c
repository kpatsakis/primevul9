dir_open_dir(int argc, VALUE *argv)
{
    VALUE dir = rb_funcallv_kw(rb_cDir, rb_intern("open"), argc, argv, RB_PASS_CALLED_KEYWORDS);

    rb_check_typeddata(dir, &dir_data_type);
    return dir;
}