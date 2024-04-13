  Supports_Block_Obj Parser::parse_supports_directive()
  {
    Supports_Condition_Obj cond = parse_supports_condition();
    if (!cond) {
      css_error("Invalid CSS", " after ", ": expected @supports condition (e.g. (display: flexbox)), was ", false);
    }
    // create the ast node object for the support queries
    Supports_Block_Obj query = SASS_MEMORY_NEW(Supports_Block, pstate, cond);
    // additional block is mandatory
    // parse inner block
    query->block(parse_block());
    // return ast node
    return query;
  }