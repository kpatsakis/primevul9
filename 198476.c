static void parse_sec_attr(sc_card_t *card,
                           sc_file_t *file, const u8 *buf, size_t len)
{
        unsigned int op;
        
        /* list directory is not covered by ACLs - so always add an entry */
        sc_file_add_acl_entry (file, SC_AC_OP_LIST_FILES,
                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
        /* FIXME: check for what LOCK is used */
        sc_file_add_acl_entry (file, SC_AC_OP_LOCK,
                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
        for (; len >= 6; len -= 6, buf += 6) {
                /* FIXME: temporary hacks */
                if (!memcmp(buf, "\xa4\x00\x00\x00\xff\xff", 6)) /* select */
                        sc_file_add_acl_entry (file, SC_AC_OP_SELECT,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                else if (!memcmp(buf, "\xb0\x00\x00\x00\xff\xff", 6)) /*read*/
                        sc_file_add_acl_entry (file, SC_AC_OP_READ,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                else if (!memcmp(buf, "\xd6\x00\x00\x00\xff\xff", 6)) /*upd*/
                        sc_file_add_acl_entry (file, SC_AC_OP_UPDATE,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                else if (!memcmp(buf, "\x60\x00\x00\x00\xff\xff", 6)) {/*adm */
                        sc_file_add_acl_entry (file, SC_AC_OP_WRITE,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                        sc_file_add_acl_entry (file, SC_AC_OP_CREATE,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                        sc_file_add_acl_entry (file, SC_AC_OP_INVALIDATE,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                        sc_file_add_acl_entry (file, SC_AC_OP_REHABILITATE,
                                               SC_AC_NONE, SC_AC_KEY_REF_NONE);
                }
                else {
                        /* the first byte tells use the command or the
                           command group.  We have to mask bit 0
                           because this one distinguish between AND/OR
                           combination of PINs*/
                        op = map_operations (buf[0]);
                        if (op == (unsigned int)-1)
                        {
                                sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
                                       "Unknown security command byte %02x\n",
                                       buf[0]);
                                continue;
                        }
                        if (!buf[1])
                                sc_file_add_acl_entry (file, op,
                                                       SC_AC_NONE,
                                                       SC_AC_KEY_REF_NONE);
                        else
                                sc_file_add_acl_entry (file, op,
                                                       SC_AC_CHV, buf[1]);

                        if (!buf[2] && !buf[3])
                                sc_file_add_acl_entry (file, op,
                                                       SC_AC_NONE,
                                                       SC_AC_KEY_REF_NONE);
                        else
                                sc_file_add_acl_entry (file, op,
                                                       SC_AC_TERM,
                                                       (buf[2]<<8)|buf[3]);
                }
        }
}