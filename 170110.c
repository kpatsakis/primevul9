void VDir::Init(VDir* pDir, VMem *p)
{
    int index;

    pMem = p;
    if (pDir) {
	for (index = 0; index < driveCount; ++index) {
	    SetDirW(pDir->GetDirW(index), index);
	}
	nDefault = pDir->GetDefault();
    }
    else {
	int bSave = bManageDirectory;
	DWORD driveBits = GetLogicalDrives();

	bManageDirectory = 0;
        WCHAR szBuffer[MAX_PATH*driveCount];
        if (GetLogicalDriveStringsW(sizeof(szBuffer), szBuffer)) {
            WCHAR* pEnv = GetEnvironmentStringsW();
            WCHAR* ptr = szBuffer;
            for (index = 0; index < driveCount; ++index) {
                if (driveBits & (1<<index)) {
                    ptr += SetDirW(ptr, index) + 1;
                    FromEnvW(pEnv, index);
                }
            }
            FreeEnvironmentStringsW(pEnv);
        }
        SetDefaultW(L".");
	bManageDirectory = bSave;
  }
}