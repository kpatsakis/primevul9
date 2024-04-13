  Expression_Obj Parser::fold_operands(Expression_Obj base, std::vector<Expression_Obj>& operands, std::vector<Operand>& ops, size_t i)
  {
    if (String_Schema* schema = Cast<String_Schema>(base)) {
      // return schema;
      if (schema->has_interpolants()) {
        if (i + 1 < operands.size() && (
             (ops[0].operand == Sass_OP::EQ)
          || (ops[0].operand == Sass_OP::ADD)
          || (ops[0].operand == Sass_OP::DIV)
          || (ops[0].operand == Sass_OP::MUL)
          || (ops[0].operand == Sass_OP::NEQ)
          || (ops[0].operand == Sass_OP::LT)
          || (ops[0].operand == Sass_OP::GT)
          || (ops[0].operand == Sass_OP::LTE)
          || (ops[0].operand == Sass_OP::GTE)
        )) {
          Expression_Obj rhs = fold_operands(operands[i], operands, ops, i + 1);
          rhs = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), ops[0], schema, rhs);
          return rhs;
        }
        // return schema;
      }
    }

    for (size_t S = operands.size(); i < S; ++i) {
      if (String_Schema* schema = Cast<String_Schema>(operands[i])) {
        if (schema->has_interpolants()) {
          if (i + 1 < S) {
            // this whole branch is never hit via spec tests
            Expression_Obj rhs = fold_operands(operands[i+1], operands, ops, i + 2);
            rhs = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), ops[i], schema, rhs);
            base = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), ops[i], base, rhs);
            return base;
          }
          base = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), ops[i], base, operands[i]);
          return base;
        } else {
          base = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), ops[i], base, operands[i]);
        }
      } else {
        base = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), ops[i], base, operands[i]);
      }
      Binary_Expression* b = Cast<Binary_Expression>(base.ptr());
      if (b && ops[i].operand == Sass_OP::DIV && b->left()->is_delayed() && b->right()->is_delayed()) {
        base->is_delayed(true);
      }
    }
    // nested binary expression are never to be delayed
    if (Binary_Expression* b = Cast<Binary_Expression>(base)) {
      if (Cast<Binary_Expression>(b->left())) base->set_delayed(false);
      if (Cast<Binary_Expression>(b->right())) base->set_delayed(false);
    }
    return base;
  }