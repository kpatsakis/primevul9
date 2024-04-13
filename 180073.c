prelink_method (MonoMethod *method)
{
	const char *exc_class, *exc_arg;
	if (!(method->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL))
		return;
	mono_lookup_pinvoke_call (method, &exc_class, &exc_arg);
	if (exc_class) {
		mono_raise_exception( 
			mono_exception_from_name_msg (mono_defaults.corlib, "System", exc_class, exc_arg ) );
	}
	/* create the wrapper, too? */
}