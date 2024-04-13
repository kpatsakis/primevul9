  Block_Obj Parser::parse()
  {

    // consume unicode BOM
    read_bom();

    // scan the input to find invalid utf8 sequences
    const char* it = utf8::find_invalid(position, end);

    // report invalid utf8
    if (it != end) {
      pstate += Offset::init(position, it);
      traces.push_back(Backtrace(pstate));
      throw Exception::InvalidSass(pstate, traces, "Invalid UTF-8 sequence");
    }

    // create a block AST node to hold children
    Block_Obj root = SASS_MEMORY_NEW(Block, pstate, 0, true);

    // check seems a bit esoteric but works
    if (ctx.resources.size() == 1) {
      // apply headers only on very first include
      ctx.apply_custom_headers(root, path, pstate);
    }

    // parse children nodes
    block_stack.push_back(root);
    parse_block_nodes(true);
    block_stack.pop_back();

    // update final position
    root->update_pstate(pstate);

    if (position != end) {
      css_error("Invalid CSS", " after ", ": expected selector or at-rule, was ");
    }

    return root;
  }