ves_icall_System_Reflection_Assembly_get_EntryPoint (MonoReflectionAssembly *assembly) 
{
	guint32 token = mono_image_get_entry_point (assembly->assembly->image);

	MONO_ARCH_SAVE_REGS;

	if (!token)
		return NULL;
	return mono_method_get_object (mono_object_domain (assembly), mono_get_method (assembly->assembly->image, token, NULL), NULL);
}