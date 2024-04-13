void Dispatcher::dispatch( Function_Type type, const Parser::Action *act, Framebuffer *fb )
{
  /* add final char to dispatch key */
  if ( (type == ESCAPE) || (type == CSI) ) {
    assert( act->char_present );
    Parser::Collect act2;
    act2.char_present = true;
    act2.ch = act->ch;
    collect( &act2 ); 
  }

  dispatch_map_t *map = NULL;
  switch ( type ) {
  case ESCAPE:  map = &get_global_dispatch_registry().escape;  break;
  case CSI:     map = &get_global_dispatch_registry().CSI;     break;
  case CONTROL: map = &get_global_dispatch_registry().control; break;
  }

  std::string key = dispatch_chars;
  if ( type == CONTROL ) {
    assert( act->ch <= 255 );
    char ctrlstr[ 2 ] = { (char)act->ch, 0 };
    key = std::string( ctrlstr, 1 );
  }

  dispatch_map_t::const_iterator i = map->find( key );
  if ( i == map->end() ) {
    /* unknown function */
    fb->ds.next_print_will_wrap = false;
    return;
  } else {
    act->handled = true;
    if ( i->second.clears_wrap_state ) {
      fb->ds.next_print_will_wrap = false;
    }
    return i->second.function( fb, this );
  }
}