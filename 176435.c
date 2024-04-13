  While_Obj Parser::parse_while_directive()
  {
    stack.push_back(Scope::Control);
    bool root = block_stack.back()->is_root();
    // create the initial while call object
    While_Obj call = SASS_MEMORY_NEW(While, pstate, {}, {});
    // parse mandatory predicate
    Expression_Obj predicate = parse_list();
    List_Obj l = Cast<List>(predicate);
    if (!predicate || (l && !l->length())) {
      css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ", false);
    }
    call->predicate(predicate);
    // parse mandatory block
    call->block(parse_block(root));
    // return ast node
    stack.pop_back();
    // return ast node
    return call.detach();
  }