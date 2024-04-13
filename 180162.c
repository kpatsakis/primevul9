ves_icall_System_Delegate_SetMulticastInvoke (MonoDelegate *this)
{
	/* Reset the invoke impl to the default one */
	this->invoke_impl = mono_runtime_create_delegate_trampoline (this->object.vtable->klass);
}