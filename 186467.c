PHP_LIBXML_API BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return xmlDllMain(hinstDLL, fdwReason, lpvReserved);
}