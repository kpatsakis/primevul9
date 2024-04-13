  void Inspect::operator()(Binary_Expression_Ptr expr)
  {
    expr->left()->perform(this);
    if ( in_media_block ||
         (output_style() == INSPECT) || (
          expr->op().ws_before
          && (!expr->is_interpolant())
          && (expr->is_left_interpolant() ||
              expr->is_right_interpolant())

    )) append_string(" ");
    switch (expr->optype()) {
      case Sass_OP::AND: append_string("&&"); break;
      case Sass_OP::OR:  append_string("||");  break;
      case Sass_OP::EQ:  append_string("==");  break;
      case Sass_OP::NEQ: append_string("!=");  break;
      case Sass_OP::GT:  append_string(">");   break;
      case Sass_OP::GTE: append_string(">=");  break;
      case Sass_OP::LT:  append_string("<");   break;
      case Sass_OP::LTE: append_string("<=");  break;
      case Sass_OP::ADD: append_string("+");   break;
      case Sass_OP::SUB: append_string("-");   break;
      case Sass_OP::MUL: append_string("*");   break;
      case Sass_OP::DIV: append_string("/"); break;
      case Sass_OP::MOD: append_string("%");   break;
      default: break; // shouldn't get here
    }
    if ( in_media_block ||
         (output_style() == INSPECT) || (
          expr->op().ws_after
          && (!expr->is_interpolant())
          && (expr->is_left_interpolant() ||
              expr->is_right_interpolant())
    )) append_string(" ");
    expr->right()->perform(this);
  }