static inline void nfs4_exclusive_attrset(struct nfs4_opendata *opendata, struct iattr *sattr)
{
	if ((opendata->o_res.attrset[1] & FATTR4_WORD1_TIME_ACCESS) &&
	    !(sattr->ia_valid & ATTR_ATIME_SET))
		sattr->ia_valid |= ATTR_ATIME;

	if ((opendata->o_res.attrset[1] & FATTR4_WORD1_TIME_MODIFY) &&
	    !(sattr->ia_valid & ATTR_MTIME_SET))
		sattr->ia_valid |= ATTR_MTIME;
}