ves_icall_ModuleBuilder_build_metadata (MonoReflectionModuleBuilder *mb)
{
	MONO_ARCH_SAVE_REGS;

	mono_image_build_metadata (mb);
}