static void foo (short zz_datatype) {
    switch (zz_datatype) {
    case 0x0001: /* ZIP64 extended information extra field */
    case 0x0007: /* AV Info */
    case 0x0008: /* Reserved for future Unicode file name data (PFS) */
    case 0x0009: /* OS/2 */
    case 0x000a: /* NTFS */
    case 0x000c: /* OpenVMS */
    case 0x000d: /* Unix */
    case 0x000e: /* Reserved for file stream and fork descriptors */
    case 0x000f: /* Patch Descriptor */
    case 0x0014: /* PKCS#7 Store for X.509 Certificates */
    case 0x0015: /* X.509 Certificate ID and Signature for file */
    case 0x0016: /* X.509 Certificate ID for Central Directory */
    case 0x0017: /* Strong Encryption Header */
    case 0x0018: /* Record Management Controls */
    case 0x0019: /* PKCS#7 Encryption Recipient Certificate List */
    case 0x0065: /* IBM S/390, AS/400 attributes - uncompressed */
    case 0x0066: /* Reserved for IBM S/390, AS/400 attr - compressed */
    case 0x07c8: /* Macintosh */
    case 0x2605: /* ZipIt Macintosh */
    case 0x2705: /* ZipIt Macintosh 1.3.5+ */
    case 0x2805: /* ZipIt Macintosh 1.3.5+ */
    case 0x334d: /* Info-ZIP Macintosh */
    case 0x4341: /* Acorn/SparkFS  */
    case 0x4453: /* Windows NT security descriptor (binary ACL) */
    case 0x4704: /* VM/CMS */
    case 0x470f: /* MVS */
    case 0x4b46: /* FWKCS MD5 (see below) */
    case 0x4c41: /* OS/2 access control list (text ACL) */
    case 0x4d49: /* Info-ZIP OpenVMS */
    case 0x4f4c: /* Xceed original location extra field */
    case 0x5356: /* AOS/VS (ACL) */
    case 0x5455: /* extended timestamp */
    case 0x554e: /* Xceed unicode extra field */
    case 0x5855: /* Info-ZIP Unix (original, also OS/2, NT, etc) */
    case 0x6542: /* BeOS/BeBox */
    case 0x756e: /* ASi Unix */
    case 0x7855: /* Info-ZIP Unix (new) */
    case 0xfd4a: /* SMS/QDOS */
    }
}