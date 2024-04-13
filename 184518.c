librariesLoaded(void)
{
#if !CALL_BUNDLED_FUNCTIONS_DIRECTLY
	if ((globalCreateVM  == NULL)||(globalGetVMs == NULL)) {
		return FALSE;
	}
#endif
	return TRUE;
}