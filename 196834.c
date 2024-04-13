Function::Function( Function_Type type, std::string dispatch_chars,
		    void (*s_function)( Framebuffer *, Dispatcher * ),
		    bool s_clears_wrap_state )
  : function( s_function ), clears_wrap_state( s_clears_wrap_state )
{
  register_function( type, dispatch_chars, *this );
}