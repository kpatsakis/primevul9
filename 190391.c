  bool is_complex_type(t_type* ttype) {
    ttype = get_true_type(ttype);

    return ttype->is_container() || ttype->is_struct() || ttype->is_xception()
           || (ttype->is_base_type()
               && (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING));
  }