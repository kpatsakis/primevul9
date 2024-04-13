E_P16 (uschar *p14, uschar *p16)
{
uschar sp8[8] = { 0x4b, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 };
smbhash (p16, sp8, p14, 1);
smbhash (p16 + 8, sp8, p14 + 7, 1);
}