VDir::VDir(int bManageDir /* = 1 */)
{
    nDefault = 0;
    bManageDirectory = bManageDir;
    memset(dirTableA, 0, sizeof(dirTableA));
    memset(dirTableW, 0, sizeof(dirTableW));
}