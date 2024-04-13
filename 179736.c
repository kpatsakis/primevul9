  Media_Block_Obj Parser::parse_media_block()
  {
    stack.push_back(Scope::Media);
    Media_Block_Obj media_block = SASS_MEMORY_NEW(Media_Block, pstate, 0, 0);

    media_block->media_queries(parse_media_queries());

    Media_Block_Obj prev_media_block = last_media_block;
    last_media_block = media_block;
    media_block->block(parse_css_block());
    last_media_block = prev_media_block;
    stack.pop_back();
    return media_block.detach();
  }