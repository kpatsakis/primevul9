static inline int __smb2_reconnect(const struct nls_table *nlsc,
				   struct cifs_tcon *tcon)
{
	return SMB2_tcon(0, tcon->ses, tcon->treeName, tcon, nlsc);
}