ves_icall_System_Environment_BroadcastSettingChange (void)
{
#ifdef PLATFORM_WIN32
	SendMessageTimeout (HWND_BROADCAST, WM_SETTINGCHANGE, NULL, L"Environment", SMTO_ABORTIFHUNG, 2000, 0);
#endif
}