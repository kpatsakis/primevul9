uchar *MsgGetProp(msg_t *pMsg, struct templateEntry *pTpe,
                 propid_t propID, size_t *pPropLen,
		 unsigned short *pbMustBeFreed)
{
	uchar *pRes; /* result pointer */
	int bufLen = -1; /* length of string or -1, if not known */
	uchar *pBufStart;
	uchar *pBuf;
	int iLen;
	short iOffs;

	BEGINfunc
	assert(pMsg != NULL);
	assert(pbMustBeFreed != NULL);

#ifdef	FEATURE_REGEXP
	/* Variables necessary for regular expression matching */
	size_t nmatch = 10;
	regmatch_t pmatch[10];
#endif

	*pbMustBeFreed = 0;

	switch(propID) {
		case PROP_MSG:
			pRes = getMSG(pMsg);
			bufLen = getMSGLen(pMsg);
			break;
		case PROP_TIMESTAMP:
			pRes = (uchar*)getTimeReported(pMsg, pTpe->data.field.eDateFormat);
			break;
		case PROP_HOSTNAME:
			pRes = (uchar*)getHOSTNAME(pMsg);
			bufLen = getHOSTNAMELen(pMsg);
			break;
		case PROP_SYSLOGTAG:
			getTAG(pMsg, &pRes, &bufLen);
			break;
		case PROP_RAWMSG:
			getRawMsg(pMsg, &pRes, &bufLen);
			break;
		/* enable this, if someone actually uses UxTradMsg, delete after some  time has
		 * passed and nobody complained -- rgerhards, 2009-06-16
		case PROP_UXTRADMSG:
			pRes = getUxTradMsg(pMsg);
			break;
		*/
		case PROP_INPUTNAME:
			getInputName(pMsg, &pRes, &bufLen);
			break;
		case PROP_FROMHOST:
			pRes = getRcvFrom(pMsg);
			break;
		case PROP_FROMHOST_IP:
			pRes = getRcvFromIP(pMsg);
			break;
		case PROP_PRI:
			pRes = (uchar*)getPRI(pMsg);
			break;
		case PROP_PRI_TEXT:
			pBuf = MALLOC(20 * sizeof(uchar));
			if(pBuf == NULL) {
				RET_OUT_OF_MEMORY;
			} else {
				*pbMustBeFreed = 1;
				pRes = (uchar*)textpri((char*)pBuf, 20, getPRIi(pMsg));
			}
			break;
		case PROP_IUT:
			pRes = UCHAR_CONSTANT("1"); /* always 1 for syslog messages (a MonitorWare thing;)) */
			bufLen = 1;
			break;
		case PROP_SYSLOGFACILITY:
			pRes = (uchar*)getFacility(pMsg);
			break;
		case PROP_SYSLOGFACILITY_TEXT:
			pRes = (uchar*)getFacilityStr(pMsg);
			break;
		case PROP_SYSLOGSEVERITY:
			pRes = (uchar*)getSeverity(pMsg);
			break;
		case PROP_SYSLOGSEVERITY_TEXT:
			pRes = (uchar*)getSeverityStr(pMsg);
			break;
		case PROP_TIMEGENERATED:
			pRes = (uchar*)getTimeGenerated(pMsg, pTpe->data.field.eDateFormat);
			break;
		case PROP_PROGRAMNAME:
			pRes = getProgramName(pMsg, LOCK_MUTEX);
			break;
		case PROP_PROTOCOL_VERSION:
			pRes = (uchar*)getProtocolVersionString(pMsg);
			break;
		case PROP_STRUCTURED_DATA:
			pRes = (uchar*)getStructuredData(pMsg);
			break;
		case PROP_APP_NAME:
			pRes = (uchar*)getAPPNAME(pMsg, LOCK_MUTEX);
			break;
		case PROP_PROCID:
			pRes = (uchar*)getPROCID(pMsg, LOCK_MUTEX);
			break;
		case PROP_MSGID:
			pRes = (uchar*)getMSGID(pMsg);
			break;
		case PROP_SYS_NOW:
			if((pRes = getNOW(NOW_NOW)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_YEAR:
			if((pRes = getNOW(NOW_YEAR)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_MONTH:
			if((pRes = getNOW(NOW_MONTH)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_DAY:
			if((pRes = getNOW(NOW_DAY)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_HOUR:
			if((pRes = getNOW(NOW_HOUR)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_HHOUR:
			if((pRes = getNOW(NOW_HHOUR)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_QHOUR:
			if((pRes = getNOW(NOW_QHOUR)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_MINUTE:
			if((pRes = getNOW(NOW_MINUTE)) == NULL) {
				RET_OUT_OF_MEMORY;
			} else
				*pbMustBeFreed = 1;	/* all of these functions allocate dyn. memory */
			break;
		case PROP_SYS_MYHOSTNAME:
			pRes = glbl.GetLocalHostName();
			break;
		default:
			/* there is no point in continuing, we may even otherwise render the
			 * error message unreadable. rgerhards, 2007-07-10
			 */
			dbgprintf("invalid property id: '%d'\n", propID);
			*pbMustBeFreed = 0;
			*pPropLen = sizeof("**INVALID PROPERTY NAME**") - 1;
			return UCHAR_CONSTANT("**INVALID PROPERTY NAME**");
	}

	/* If we did not receive a template pointer, we are already done... */
	if(pTpe == NULL) {
		return pRes;
	}
	
	/* Now check if we need to make "temporary" transformations (these
	 * are transformations that do not go back into the message -
	 * memory must be allocated for them!).
	 */
	
	/* substring extraction */
	/* first we check if we need to extract by field number
	 * rgerhards, 2005-12-22
	 */
	if(pTpe->data.field.has_fields == 1) {
		size_t iCurrFld;
		uchar *pFld;
		uchar *pFldEnd;
		/* first, skip to the field in question. The field separator
		 * is always one character and is stored in the template entry.
		 */
		iCurrFld = 1;
		pFld = pRes;
		while(*pFld && iCurrFld < pTpe->data.field.iToPos) {
			/* skip fields until the requested field or end of string is found */
			while(*pFld && (uchar) *pFld != pTpe->data.field.field_delim)
				++pFld; /* skip to field terminator */
			if(*pFld == pTpe->data.field.field_delim) {
				++pFld; /* eat it */
				if (pTpe->data.field.field_expand != 0) {
					while (*pFld == pTpe->data.field.field_delim) {
						++pFld;
					}
				}
				++iCurrFld;
			}
		}
		dbgprintf("field requested %d, field found %d\n", pTpe->data.field.iToPos, (int) iCurrFld);
		
		if(iCurrFld == pTpe->data.field.iToPos) {
			/* field found, now extract it */
			/* first of all, we need to find the end */
			pFldEnd = pFld;
			while(*pFldEnd && *pFldEnd != pTpe->data.field.field_delim)
				++pFldEnd;
			--pFldEnd; /* we are already at the delimiter - so we need to
			            * step back a little not to copy it as part of the field. */
			/* we got our end pointer, now do the copy */
			/* TODO: code copied from below, this is a candidate for a separate function */
			iLen = pFldEnd - pFld + 1; /* the +1 is for an actual char, NOT \0! */
			pBufStart = pBuf = MALLOC((iLen + 1) * sizeof(char));
			if(pBuf == NULL) {
				if(*pbMustBeFreed == 1)
					free(pRes);
				RET_OUT_OF_MEMORY;
			}
			/* now copy */
			memcpy(pBuf, pFld, iLen);
			bufLen = iLen;
			pBuf[iLen] = '\0'; /* terminate it */
			if(*pbMustBeFreed == 1)
				free(pRes);
			pRes = pBufStart;
			*pbMustBeFreed = 1;
			if(*(pFldEnd+1) != '\0')
				++pFldEnd; /* OK, skip again over delimiter char */
		} else {
			/* field not found, return error */
			if(*pbMustBeFreed == 1)
				free(pRes);
			*pbMustBeFreed = 0;
			*pPropLen = sizeof("**FIELD NOT FOUND**") - 1;
			return UCHAR_CONSTANT("**FIELD NOT FOUND**");
		}
	} else if(pTpe->data.field.iFromPos != 0 || pTpe->data.field.iToPos != 0) {
		/* we need to obtain a private copy */
		int iFrom, iTo;
		uchar *pSb;
		iFrom = pTpe->data.field.iFromPos;
		iTo = pTpe->data.field.iToPos;
		/* need to zero-base to and from (they are 1-based!) */
		if(iFrom > 0)
			--iFrom;
		if(iTo > 0)
			--iTo;
		if(bufLen == -1)
			bufLen = ustrlen(pRes);
		if(iFrom == 0 && iTo >=  bufLen) { 
			/* in this case, the requested string is a superset of what we already have,
			 * so there is no need to do any processing. This is a frequent case for size-limited
			 * fields like TAG in the default forwarding template (so it is a useful optimization
			 * to check for this condition ;)). -- rgerhards, 2009-07-09
			 */
			; /*DO NOTHING*/
		} else {
			iLen = iTo - iFrom + 1; /* the +1 is for an actual char, NOT \0! */
			pBufStart = pBuf = MALLOC((iLen + 1) * sizeof(char));
			if(pBuf == NULL) {
				if(*pbMustBeFreed == 1)
					free(pRes);
				RET_OUT_OF_MEMORY;
			}
			pSb = pRes;
			if(iFrom) {
			/* skip to the start of the substring (can't do pointer arithmetic
			 * because the whole string might be smaller!!)
			 */
				while(*pSb && iFrom) {
					--iFrom;
					++pSb;
				}
			}
			/* OK, we are at the begin - now let's copy... */
			bufLen = iLen;
			while(*pSb && iLen) {
				*pBuf++ = *pSb;
				++pSb;
				--iLen;
			}
			*pBuf = '\0';
			bufLen -= iLen; /* subtract remaining length if the string was smaller! */
			if(*pbMustBeFreed == 1)
				free(pRes);
			pRes = pBufStart;
			*pbMustBeFreed = 1;
		}
#ifdef FEATURE_REGEXP
	} else {
		/* Check for regular expressions */
		if (pTpe->data.field.has_regex != 0) {
			if (pTpe->data.field.has_regex == 2) {
				/* Could not compile regex before! */
				if (*pbMustBeFreed == 1) {
					free(pRes);
					*pbMustBeFreed = 0;
				}
				*pPropLen = sizeof("**NO MATCH** **BAD REGULAR EXPRESSION**") - 1;
				return UCHAR_CONSTANT("**NO MATCH** **BAD REGULAR EXPRESSION**");
			}

			dbgprintf("string to match for regex is: %s\n", pRes);

			if(objUse(regexp, LM_REGEXP_FILENAME) == RS_RET_OK) {
				short iTry = 0;
				uchar bFound = 0;
				iOffs = 0;
				/* first see if we find a match, iterating through the series of
				 * potential matches over the string.
				 */
				while(!bFound) {
					int iREstat;
					iREstat = regexp.regexec(&pTpe->data.field.re, (char*)(pRes + iOffs), nmatch, pmatch, 0);
					dbgprintf("regexec return is %d\n", iREstat);
					if(iREstat == 0) {
						if(pmatch[0].rm_so == -1) {
							dbgprintf("oops ... start offset of successful regexec is -1\n");
							break;
						}
						if(iTry == pTpe->data.field.iMatchToUse) {
							bFound = 1;
						} else {
							dbgprintf("regex found at offset %d, new offset %d, tries %d\n",
								  iOffs, (int) (iOffs + pmatch[0].rm_eo), iTry);
							iOffs += pmatch[0].rm_eo;
							++iTry;
						}
					} else {
						break;
					}
				}
				dbgprintf("regex: end search, found %d\n", bFound);
				if(!bFound) {
					/* we got no match! */
					if(pTpe->data.field.nomatchAction != TPL_REGEX_NOMATCH_USE_WHOLE_FIELD) {
						if (*pbMustBeFreed == 1) {
							free(pRes);
							*pbMustBeFreed = 0;
						}
						if(pTpe->data.field.nomatchAction == TPL_REGEX_NOMATCH_USE_DFLTSTR) {
							bufLen = sizeof("**NO MATCH**") - 1;
							pRes = UCHAR_CONSTANT("**NO MATCH**");
						} else if(pTpe->data.field.nomatchAction == TPL_REGEX_NOMATCH_USE_ZERO) {
							bufLen = 1;
							pRes = UCHAR_CONSTANT("0");
						} else {
							bufLen = 0;
							pRes = UCHAR_CONSTANT("");
						}
					}
				} else {
					/* Match- but did it match the one we wanted? */
					/* we got no match! */
					if(pmatch[pTpe->data.field.iSubMatchToUse].rm_so == -1) {
						if(pTpe->data.field.nomatchAction != TPL_REGEX_NOMATCH_USE_WHOLE_FIELD) {
							if (*pbMustBeFreed == 1) {
								free(pRes);
								*pbMustBeFreed = 0;
							}
							if(pTpe->data.field.nomatchAction == TPL_REGEX_NOMATCH_USE_DFLTSTR) {
								bufLen = sizeof("**NO MATCH**") - 1;
								pRes = UCHAR_CONSTANT("**NO MATCH**");
							} else if(pTpe->data.field.nomatchAction == TPL_REGEX_NOMATCH_USE_ZERO) {
								bufLen = 1;
								pRes = UCHAR_CONSTANT("0");
							} else {
								bufLen = 0;
								pRes = UCHAR_CONSTANT("");
							}
						}
					}
					/* OK, we have a usable match - we now need to malloc pB */
					int iLenBuf;
					uchar *pB;

					iLenBuf = pmatch[pTpe->data.field.iSubMatchToUse].rm_eo
						  - pmatch[pTpe->data.field.iSubMatchToUse].rm_so;
					pB = MALLOC((iLenBuf + 1) * sizeof(uchar));

					if (pB == NULL) {
						if (*pbMustBeFreed == 1)
							free(pRes);
						RET_OUT_OF_MEMORY;
					}

					/* Lets copy the matched substring to the buffer */
					memcpy(pB, pRes + iOffs +  pmatch[pTpe->data.field.iSubMatchToUse].rm_so, iLenBuf);
					bufLen = iLenBuf;
					pB[iLenBuf] = '\0';/* terminate string, did not happen before */

					if (*pbMustBeFreed == 1)
						free(pRes);
					pRes = pB;
					*pbMustBeFreed = 1;
				}
			} else {
				/* we could not load regular expression support. This is quite unexpected at
				 * this stage of processing (after all, the config parser found it), but so
				 * it is. We return an error in that case. -- rgerhards, 2008-03-07
				 */
				dbgprintf("could not get regexp object pointer, so regexp can not be evaluated\n");
				if (*pbMustBeFreed == 1) {
					free(pRes);
					*pbMustBeFreed = 0;
				}
				*pPropLen = sizeof("***REGEXP NOT AVAILABLE***") - 1;
				return UCHAR_CONSTANT("***REGEXP NOT AVAILABLE***");
			}
		}
#endif /* #ifdef FEATURE_REGEXP */
	}

	/* now check if we need to do our "SP if first char is non-space" hack logic */
	if(*pRes && pTpe->data.field.options.bSPIffNo1stSP) {
		/* here, we always destruct the buffer and return a new one */
		uchar cFirst = *pRes; /* save first char */
		if(*pbMustBeFreed == 1)
			free(pRes);
		pRes = (cFirst == ' ') ? UCHAR_CONSTANT("") : UCHAR_CONSTANT(" ");
		bufLen = (cFirst == ' ') ? 0 : 1;
		*pbMustBeFreed = 0;
	}

	if(*pRes) {
		/* case conversations (should go after substring, because so we are able to
		 * work on the smallest possible buffer).
		 */
		if(pTpe->data.field.eCaseConv != tplCaseConvNo) {
			/* we need to obtain a private copy */
			if(bufLen == -1)
				bufLen = ustrlen(pRes);
			uchar *pBStart;
			uchar *pB;
			uchar *pSrc;
			pBStart = pB = MALLOC((bufLen + 1) * sizeof(char));
			if(pB == NULL) {
				if(*pbMustBeFreed == 1)
					free(pRes);
				RET_OUT_OF_MEMORY;
			}
			pSrc = pRes;
			while(*pSrc) {
				*pB++ = (pTpe->data.field.eCaseConv == tplCaseConvUpper) ?
					(uchar)toupper((int)*pSrc) : (uchar)tolower((int)*pSrc);
				/* currently only these two exist */
				++pSrc;
			}
			*pB = '\0';
			if(*pbMustBeFreed == 1)
				free(pRes);
			pRes = pBStart;
			*pbMustBeFreed = 1;
		}

		/* now do control character dropping/escaping/replacement
		 * Only one of these can be used. If multiple options are given, the
		 * result is random (though currently there obviously is an order of
		 * preferrence, see code below. But this is NOT guaranteed.
		 * RGerhards, 2006-11-17
		 * We must copy the strings if we modify them, because they may either 
		 * point to static memory or may point into the message object, in which
		 * case we would actually modify the original property (which of course
		 * is wrong).
		 * This was found and fixed by varmojefkoj on 2007-09-11
		 */
		if(pTpe->data.field.options.bDropCC) {
			int iLenBuf = 0;
			uchar *pSrc = pRes;
			uchar *pDstStart;
			uchar *pDst;
			uchar bDropped = 0;
			
			while(*pSrc) {
				if(!iscntrl((int) *pSrc++))
					iLenBuf++;
				else
					bDropped = 1;
			}

			if(bDropped) {
				pDst = pDstStart = MALLOC(iLenBuf + 1);
				if(pDst == NULL) {
					if(*pbMustBeFreed == 1)
						free(pRes);
					RET_OUT_OF_MEMORY;
				}
				for(pSrc = pRes; *pSrc; pSrc++) {
					if(!iscntrl((int) *pSrc))
						*pDst++ = *pSrc;
				}
				*pDst = '\0';
				if(*pbMustBeFreed == 1)
					free(pRes);
				pRes = pDstStart;
				bufLen = iLenBuf;
				*pbMustBeFreed = 1;
			}
		} else if(pTpe->data.field.options.bSpaceCC) {
			uchar *pSrc;
			uchar *pDstStart;
			uchar *pDst;
			
			if(*pbMustBeFreed == 1) {
				/* in this case, we already work on dynamic
				 * memory, so there is no need to copy it - we can
				 * modify it in-place without any harm. This is a
				 * performance optiomization.
				 */
				for(pDst = pRes; *pDst; pDst++) {
					if(iscntrl((int) *pDst))
						*pDst = ' ';
				}
			} else {
				if(bufLen == -1)
					bufLen = ustrlen(pRes);
				pDst = pDstStart = MALLOC(bufLen + 1);
				if(pDst == NULL) {
					if(*pbMustBeFreed == 1)
						free(pRes);
					RET_OUT_OF_MEMORY;
				}
				for(pSrc = pRes; *pSrc; pSrc++) {
					if(iscntrl((int) *pSrc))
						*pDst++ = ' ';
					else
						*pDst++ = *pSrc;
				}
				*pDst = '\0';
				pRes = pDstStart;
				*pbMustBeFreed = 1;
			}
		} else if(pTpe->data.field.options.bEscapeCC) {
			/* we must first count how many control charactes are
			 * present, because we need this to compute the new string
			 * buffer length. While doing so, we also compute the string
			 * length.
			 */
			int iNumCC = 0;
			int iLenBuf = 0;
			uchar *pB;

			for(pB = pRes ; *pB ; ++pB) {
				++iLenBuf;
				if(iscntrl((int) *pB))
					++iNumCC;
			}

			if(iNumCC > 0) { /* if 0, there is nothing to escape, so we are done */
				/* OK, let's do the escaping... */
				uchar *pBStart;
				uchar szCCEsc[8]; /* buffer for escape sequence */
				int i;

				iLenBuf += iNumCC * 4;
				pBStart = pB = MALLOC((iLenBuf + 1) * sizeof(uchar));
				if(pB == NULL) {
					if(*pbMustBeFreed == 1)
						free(pRes);
					RET_OUT_OF_MEMORY;
				}
				while(*pRes) {
					if(iscntrl((int) *pRes)) {
						snprintf((char*)szCCEsc, sizeof(szCCEsc), "#%3.3d", *pRes);
						for(i = 0 ; i < 4 ; ++i)
							*pB++ = szCCEsc[i];
					} else {
						*pB++ = *pRes;
					}
					++pRes;
				}
				*pB = '\0';
				if(*pbMustBeFreed == 1)
					free(pRes);
				pRes = pBStart;
				bufLen = -1;
				*pbMustBeFreed = 1;
			}
		}
	}

	/* Take care of spurious characters to make the property safe
	 * for a path definition
	 */
	if(pTpe->data.field.options.bSecPathDrop || pTpe->data.field.options.bSecPathReplace) {
		if(pTpe->data.field.options.bSecPathDrop) {
			int iLenBuf = 0;
			uchar *pSrc = pRes;
			uchar *pDstStart;
			uchar *pDst;
			uchar bDropped = 0;
			
			while(*pSrc) {
				if(*pSrc++ != '/')
					iLenBuf++;
				else
					bDropped = 1;
			}
			
			if(bDropped) {
				pDst = pDstStart = MALLOC(iLenBuf + 1);
				if(pDst == NULL) {
					if(*pbMustBeFreed == 1)
						free(pRes);
					RET_OUT_OF_MEMORY;
				}
				for(pSrc = pRes; *pSrc; pSrc++) {
					if(*pSrc != '/')
						*pDst++ = *pSrc;
				}
				*pDst = '\0';
				if(*pbMustBeFreed == 1)
					free(pRes);
				pRes = pDstStart;
				bufLen = -1; /* TODO: can we do better? */
				*pbMustBeFreed = 1;
			}
		} else {
			uchar *pSrc;
			uchar *pDstStart;
			uchar *pDst;
			
			if(*pbMustBeFreed == 1) {
				/* here, again, we can modify the string as we already obtained
				 * a private buffer. As we do not change the size of that buffer,
				 * in-place modification is possible. This is a performance
				 * enhancement.
				 */
				for(pDst = pRes; *pDst; pDst++) {
					if(*pDst == '/')
						*pDst++ = '_';
				}
			} else {
				if(bufLen == -1)
					bufLen = ustrlen(pRes);
				pDst = pDstStart = MALLOC(bufLen + 1);
				if(pDst == NULL) {
					if(*pbMustBeFreed == 1)
						free(pRes);
					RET_OUT_OF_MEMORY;
				}
				for(pSrc = pRes; *pSrc; pSrc++) {
					if(*pSrc == '/')
						*pDst++ = '_';
					else
						*pDst++ = *pSrc;
				}
				*pDst = '\0';
				/* we must NOT check if it needs to be freed, because we have done
				 * this in the if above. So if we come to hear, the pSrc string needs
				 * not to be freed (and we do not need to care about it).
				 */
				pRes = pDstStart;
				*pbMustBeFreed = 1;
			}
		}
		
		/* check for "." and ".." (note the parenthesis in the if condition!) */
		if((*pRes == '.') && (*(pRes + 1) == '\0' || (*(pRes + 1) == '.' && *(pRes + 2) == '\0'))) {
			uchar *pTmp = pRes;

			if(*(pRes + 1) == '\0')
				pRes = UCHAR_CONSTANT("_");
			else
				pRes = UCHAR_CONSTANT("_.");;
			if(*pbMustBeFreed == 1)
				free(pTmp);
			*pbMustBeFreed = 0;
		} else if(*pRes == '\0') {
			if(*pbMustBeFreed == 1)
				free(pRes);
			pRes = UCHAR_CONSTANT("_");
			bufLen = 1;
			*pbMustBeFreed = 0;
		}
	}

	/* Now drop last LF if present (pls note that this must not be done
	 * if bEscapeCC was set)!
	 */
	if(pTpe->data.field.options.bDropLastLF && !pTpe->data.field.options.bEscapeCC) {
		int iLn;
		uchar *pB;
		if(bufLen == -1)
			bufLen = ustrlen(pRes);
		iLn = bufLen;
		if(iLn > 0 && *(pRes + iLn - 1) == '\n') {
			/* we have a LF! */
			/* check if we need to obtain a private copy */
			if(*pbMustBeFreed == 0) {
				/* ok, original copy, need a private one */
				pB = MALLOC((iLn + 1) * sizeof(uchar));
				if(pB == NULL) {
					RET_OUT_OF_MEMORY;
				}
				memcpy(pB, pRes, iLn - 1);
				pRes = pB;
				*pbMustBeFreed = 1;
			}
			*(pRes + iLn - 1) = '\0'; /* drop LF ;) */
			--bufLen;
		}
	}

	/* finally, we need to check if the property should be formatted in CSV
	 * format (we use RFC 4180, and always use double quotes). As of this writing,
	 * this should be the last action carried out on the property, but in the
	 * future there may be reasons to change that. -- rgerhards, 2009-04-02
	 */
	if(pTpe->data.field.options.bCSV) {
		/* we need to obtain a private copy, as we need to at least add the double quotes */
		int iBufLen;
		uchar *pBStart;
		uchar *pDst;
		uchar *pSrc;
		if(bufLen == -1)
			bufLen = ustrlen(pRes);
		iBufLen = bufLen;
		/* the malloc may be optimized, we currently use the worst case... */
		pBStart = pDst = MALLOC((2 * iBufLen + 3) * sizeof(uchar));
		if(pDst == NULL) {
			if(*pbMustBeFreed == 1)
				free(pRes);
			RET_OUT_OF_MEMORY;
		}
		pSrc = pRes;
		*pDst++ = '"'; /* starting quote */
		while(*pSrc) {
			if(*pSrc == '"')
				*pDst++ = '"'; /* need to add double double quote (see RFC4180) */
			*pDst++ = *pSrc++;
		}
		*pDst++ = '"';	/* ending quote */
		*pDst = '\0';
		if(*pbMustBeFreed == 1)
			free(pRes);
		pRes = pBStart;
		bufLen = -1;
		*pbMustBeFreed = 1;
	}

	if(bufLen == -1)
		bufLen = ustrlen(pRes);
	*pPropLen = bufLen;

	ENDfunc
	return(pRes);
}