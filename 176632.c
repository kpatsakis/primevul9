  void Inspect::operator()(Declaration_Ptr dec)
  {
    if (dec->value()->concrete_type() == Expression::NULL_VAL) return;
    bool was_decl = in_declaration;
    in_declaration = true;
    LOCAL_FLAG(in_custom_property, dec->is_custom_property());

    if (output_style() == NESTED)
      indentation += dec->tabs();
    append_indentation();
    if (dec->property())
      dec->property()->perform(this);
    append_colon_separator();

    if (dec->value()->concrete_type() == Expression::SELECTOR) {
      Listize listize;
      Expression_Obj ls = dec->value()->perform(&listize);
      ls->perform(this);
    } else {
      dec->value()->perform(this);
    }

    if (dec->is_important()) {
      append_optional_space();
      append_string("!important");
    }
    append_delimiter();
    if (output_style() == NESTED)
      indentation -= dec->tabs();
    in_declaration = was_decl;
  }