ves_icall_System_IO_DriveInfo_GetDriveType (MonoString *root_path_name)
{
	MONO_ARCH_SAVE_REGS;

	return GetDriveType (mono_string_chars (root_path_name));
}