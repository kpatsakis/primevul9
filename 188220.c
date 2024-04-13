/* {{{  */
void php_get_windows_cpu(char *buf, int bufsize)
{
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	switch (SysInfo.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_INTEL :
			snprintf(buf, bufsize, "i%d", SysInfo.dwProcessorType);
			break;
		case PROCESSOR_ARCHITECTURE_MIPS :
			snprintf(buf, bufsize, "MIPS R%d000", SysInfo.wProcessorLevel);
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA :
			snprintf(buf, bufsize, "Alpha %d", SysInfo.wProcessorLevel);
			break;
		case PROCESSOR_ARCHITECTURE_PPC :
			snprintf(buf, bufsize, "PPC 6%02d", SysInfo.wProcessorLevel);
			break;
		case PROCESSOR_ARCHITECTURE_IA64 :
			snprintf(buf, bufsize,  "IA64");
			break;
#if defined(PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
		case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64 :
			snprintf(buf, bufsize, "IA32");
			break;
#endif
#if defined(PROCESSOR_ARCHITECTURE_AMD64)
		case PROCESSOR_ARCHITECTURE_AMD64 :
			snprintf(buf, bufsize, "AMD64");
			break;
#endif
		case PROCESSOR_ARCHITECTURE_UNKNOWN :
		default:
			snprintf(buf, bufsize, "Unknown");
			break;
	}