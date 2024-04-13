ves_icall_MonoMethodMessage_InitMessage (MonoMethodMessage *this, 
					 MonoReflectionMethod *method,
					 MonoArray *out_args)
{
	MONO_ARCH_SAVE_REGS;

	mono_message_init (mono_object_domain (this), this, method, out_args);
}