match_i(VALUE regexp, VALUE klass, VALUE memo)
{
    if (regexp == Qundef) return ST_STOP;
    if (RTEST(rb_funcall(klass, i_json_creatable_p, 0)) &&
      RTEST(rb_funcall(regexp, i_match, 1, rb_ary_entry(memo, 0)))) {
        rb_ary_push(memo, klass);
        return ST_STOP;
    }
    return ST_CONTINUE;
}