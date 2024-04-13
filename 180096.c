ves_icall_ModuleBuilder_WriteToFile (MonoReflectionModuleBuilder *mb, HANDLE file)
{
	MONO_ARCH_SAVE_REGS;

	mono_image_create_pefile (mb, file);
}