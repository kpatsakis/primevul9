Init_fiddle_function(void)
{
    /*
     * Document-class: Fiddle::Function
     *
     * == Description
     *
     * A representation of a C function
     *
     * == Examples
     *
     * === 'strcpy'
     *
     *   @libc = Fiddle.dlopen "/lib/libc.so.6"
     *	    #=> #<Fiddle::Handle:0x00000001d7a8d8>
     *   f = Fiddle::Function.new(
     *     @libc['strcpy'],
     *     [Fiddle::TYPE_VOIDP, Fiddle::TYPE_VOIDP],
     *     Fiddle::TYPE_VOIDP)
     *	    #=> #<Fiddle::Function:0x00000001d8ee00>
     *   buff = "000"
     *	    #=> "000"
     *   str = f.call(buff, "123")
     *	    #=> #<Fiddle::Pointer:0x00000001d0c380 ptr=0x000000018a21b8 size=0 free=0x00000000000000>
     *   str.to_s
     *   => "123"
     *
     * === ABI check
     *
     *   @libc = Fiddle.dlopen "/lib/libc.so.6"
     *	    #=> #<Fiddle::Handle:0x00000001d7a8d8>
     *   f = Fiddle::Function.new(@libc['strcpy'], [TYPE_VOIDP, TYPE_VOIDP], TYPE_VOIDP)
     *	    #=> #<Fiddle::Function:0x00000001d8ee00>
     *   f.abi == Fiddle::Function::DEFAULT
     *	    #=> true
     */
    cFiddleFunction = rb_define_class_under(mFiddle, "Function", rb_cObject);

    /*
     * Document-const: DEFAULT
     *
     * Default ABI
     *
     */
    rb_define_const(cFiddleFunction, "DEFAULT", INT2NUM(FFI_DEFAULT_ABI));

#ifdef HAVE_CONST_FFI_STDCALL
    /*
     * Document-const: STDCALL
     *
     * FFI implementation of WIN32 stdcall convention
     *
     */
    rb_define_const(cFiddleFunction, "STDCALL", INT2NUM(FFI_STDCALL));
#endif

    rb_define_alloc_func(cFiddleFunction, allocate);

    /*
     * Document-method: call
     *
     * Calls the constructed Function, with +args+.
     * Caller must ensure the underlying function is called in a
     * thread-safe manner if running in a multi-threaded process.
     *
     * For an example see Fiddle::Function
     *
     */
    rb_define_method(cFiddleFunction, "call", function_call, -1);

    /*
     * Document-method: new
     * call-seq: new(ptr, args, ret_type, abi = DEFAULT)
     *
     * Constructs a Function object.
     * * +ptr+ is a referenced function, of a Fiddle::Handle
     * * +args+ is an Array of arguments, passed to the +ptr+ function
     * * +ret_type+ is the return type of the function
     * * +abi+ is the ABI of the function
     *
     */
    rb_define_method(cFiddleFunction, "initialize", initialize, -1);
}