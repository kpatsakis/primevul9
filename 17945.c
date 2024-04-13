ProcXkbGetKbdByName(ClientPtr client)
{
    DeviceIntPtr 		dev;
    DeviceIntPtr                tmpd;
    xkbGetKbdByNameReply 	rep;
    xkbGetMapReply		mrep;
    xkbGetCompatMapReply	crep;
    xkbGetIndicatorMapReply	irep;
    xkbGetNamesReply		nrep;
    xkbGetGeometryReply		grep;
    XkbComponentNamesRec	names;
    XkbDescPtr			xkb, new;
    unsigned char *		str;
    char 			mapFile[PATH_MAX];
    unsigned			len;
    unsigned			fwant,fneed,reported;
    int				status;
    Bool			geom_changed;
    XkbSrvLedInfoPtr            old_sli;
    XkbSrvLedInfoPtr            sli;
    Mask access_mode = DixGetAttrAccess | DixManageAccess;

    REQUEST(xkbGetKbdByNameReq);
    REQUEST_AT_LEAST_SIZE(xkbGetKbdByNameReq);

    if (!(client->xkbClientFlags&_XkbClientInitialized))
	return BadAccess;

    CHK_KBD_DEVICE(dev, stuff->deviceSpec, client, access_mode);

    xkb = dev->key->xkbInfo->desc;
    status= Success;
    str= (unsigned char *)&stuff[1];
    names.keymap= GetComponentSpec(&str,True,&status);
    names.keycodes= GetComponentSpec(&str,True,&status);
    names.types= GetComponentSpec(&str,True,&status);
    names.compat= GetComponentSpec(&str,True,&status);
    names.symbols= GetComponentSpec(&str,True,&status);
    names.geometry= GetComponentSpec(&str,True,&status);
    if (status!=Success)
	return status;
    len= str-((unsigned char *)stuff);
    if ((XkbPaddedSize(len)/4)!=stuff->length)
	return BadLength;

    CHK_MASK_LEGAL(0x01,stuff->want,XkbGBN_AllComponentsMask);
    CHK_MASK_LEGAL(0x02,stuff->need,XkbGBN_AllComponentsMask);
    
    if (stuff->load)
	 fwant= XkbGBN_AllComponentsMask;
    else fwant= stuff->want|stuff->need;
    if ((!names.compat)&&
        (fwant&(XkbGBN_CompatMapMask|XkbGBN_IndicatorMapMask))) {
        names.compat= _XkbDupString("%");
    }
    if ((!names.types)&&(fwant&(XkbGBN_TypesMask))) {
        names.types= _XkbDupString("%");
    }
    if ((!names.symbols)&&(fwant&XkbGBN_SymbolsMask)) {
        names.symbols= _XkbDupString("%");
    }
    geom_changed= ((names.geometry!=NULL)&&(strcmp(names.geometry,"%")!=0));
    if ((!names.geometry)&&(fwant&XkbGBN_GeometryMask)) {
        names.geometry= _XkbDupString("%");
        geom_changed= False;
    }

    bzero(mapFile,PATH_MAX);
    rep.type= X_Reply;
    rep.deviceID = dev->id;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    rep.minKeyCode = xkb->min_key_code;
    rep.maxKeyCode = xkb->max_key_code;
    rep.loaded=	False;
    fwant= XkbConvertGetByNameComponents(True,stuff->want)|XkmVirtualModsMask;
    fneed= XkbConvertGetByNameComponents(True,stuff->need);
    rep.reported= XkbConvertGetByNameComponents(False,fwant|fneed);
    if (stuff->load) {
	fneed|= XkmKeymapRequired;
	fwant|= XkmKeymapLegal;
    }
    if ((fwant|fneed)&XkmSymbolsMask) {
	fneed|= XkmKeyNamesIndex|XkmTypesIndex;
	fwant|= XkmIndicatorsIndex;
    }

    /* We pass dev in here so we can get the old names out if needed. */
    rep.found = XkbDDXLoadKeymapByNames(dev,&names,fwant,fneed,&new,
                                        mapFile,PATH_MAX);
    rep.newKeyboard= False;
    rep.pad1= rep.pad2= rep.pad3= rep.pad4= 0;

    stuff->want|= stuff->need;
    if (new==NULL)
	rep.reported= 0;
    else {
	if (stuff->load)
	    rep.loaded= True;
	if (stuff->load || 
		((rep.reported&XkbGBN_SymbolsMask) && (new->compat))) {
	    XkbChangesRec changes;
	    bzero(&changes,sizeof(changes));
	    XkbUpdateDescActions(new,
			new->min_key_code,XkbNumKeys(new),
			&changes);
	}

	if (new->map==NULL)
	    rep.reported&= ~(XkbGBN_SymbolsMask|XkbGBN_TypesMask);
	else if (rep.reported&(XkbGBN_SymbolsMask|XkbGBN_TypesMask)) {
	    mrep.type= X_Reply;
	    mrep.deviceID = dev->id;
	    mrep.sequenceNumber= client->sequence;
	    mrep.length = ((SIZEOF(xkbGetMapReply)-SIZEOF(xGenericReply))>>2);
	    mrep.minKeyCode = new->min_key_code;
	    mrep.maxKeyCode = new->max_key_code;
	    mrep.present = 0;
	    mrep.totalSyms = mrep.totalActs =
		mrep.totalKeyBehaviors= mrep.totalKeyExplicit= 
		mrep.totalModMapKeys= 0;
	    if (rep.reported&(XkbGBN_TypesMask|XkbGBN_ClientSymbolsMask)) {
		mrep.present|= XkbKeyTypesMask;
		mrep.firstType = 0;
		mrep.nTypes = mrep.totalTypes= new->map->num_types;
	    }
	    else {
		mrep.firstType = mrep.nTypes= 0;
		mrep.totalTypes= 0;
	    }
	    if (rep.reported&XkbGBN_ClientSymbolsMask) {
		mrep.present|= (XkbKeySymsMask|XkbModifierMapMask);
		mrep.firstKeySym = mrep.firstModMapKey= new->min_key_code;
		mrep.nKeySyms = mrep.nModMapKeys= XkbNumKeys(new);
	    }
	    else {
		mrep.firstKeySym= mrep.firstModMapKey= 0;
		mrep.nKeySyms= mrep.nModMapKeys= 0;
	    }
	    if (rep.reported&XkbGBN_ServerSymbolsMask) {
		mrep.present|= XkbAllServerInfoMask;
		mrep.virtualMods= ~0;
		mrep.firstKeyAct = mrep.firstKeyBehavior = 
			mrep.firstKeyExplicit = new->min_key_code;
		mrep.nKeyActs = mrep.nKeyBehaviors = 
			mrep.nKeyExplicit = XkbNumKeys(new);
	    }
	    else {
		mrep.virtualMods= 0;
		mrep.firstKeyAct= mrep.firstKeyBehavior= 
			mrep.firstKeyExplicit = 0;
		mrep.nKeyActs= mrep.nKeyBehaviors= mrep.nKeyExplicit= 0;
	    }
	    XkbComputeGetMapReplySize(new,&mrep);
	    rep.length+= SIZEOF(xGenericReply)/4+mrep.length;
	}
	if (new->compat==NULL)
	    rep.reported&= ~XkbGBN_CompatMapMask;
	else if (rep.reported&XkbGBN_CompatMapMask) {
	    crep.type= X_Reply;
	    crep.deviceID= dev->id;
	    crep.sequenceNumber= client->sequence;
	    crep.length= 0;
	    crep.groups= XkbAllGroupsMask;
	    crep.firstSI= 0;
	    crep.nSI= crep.nTotalSI= new->compat->num_si;
	    XkbComputeGetCompatMapReplySize(new->compat,&crep);
	    rep.length+= SIZEOF(xGenericReply)/4+crep.length;
	}
	if (new->indicators==NULL)
	    rep.reported&= ~XkbGBN_IndicatorMapMask;
	else if (rep.reported&XkbGBN_IndicatorMapMask) {
	    irep.type= X_Reply;
	    irep.deviceID= dev->id;
	    irep.sequenceNumber= client->sequence;
	    irep.length= 0;
	    irep.which= XkbAllIndicatorsMask;
	    XkbComputeGetIndicatorMapReplySize(new->indicators,&irep);
	    rep.length+= SIZEOF(xGenericReply)/4+irep.length;
	}
	if (new->names==NULL)
	    rep.reported&= ~(XkbGBN_OtherNamesMask|XkbGBN_KeyNamesMask);
	else if (rep.reported&(XkbGBN_OtherNamesMask|XkbGBN_KeyNamesMask)) {
	    nrep.type= X_Reply;
	    nrep.deviceID= dev->id;
	    nrep.sequenceNumber= client->sequence;
	    nrep.length= 0;
	    nrep.minKeyCode= new->min_key_code;
	    nrep.maxKeyCode= new->max_key_code;
	    if (rep.reported&XkbGBN_OtherNamesMask) {
		nrep.which= XkbAllNamesMask;
		if (new->map!=NULL)
		     nrep.nTypes= new->map->num_types;
		else nrep.nTypes= 0;
		nrep.nKTLevels= 0;
		nrep.groupNames= XkbAllGroupsMask;
		nrep.virtualMods= XkbAllVirtualModsMask;
		nrep.indicators= XkbAllIndicatorsMask;
		nrep.nRadioGroups= new->names->num_rg;
	    }
	    else {
		nrep.which= 0;
		nrep.nTypes= 0;
		nrep.nKTLevels= 0;
		nrep.groupNames= 0;
		nrep.virtualMods= 0;
		nrep.indicators= 0;
		nrep.nRadioGroups= 0;
	    }
	    if (rep.reported&XkbGBN_KeyNamesMask) {
		nrep.which|= XkbKeyNamesMask;
		nrep.firstKey= new->min_key_code;
		nrep.nKeys= XkbNumKeys(new);
		nrep.nKeyAliases= new->names->num_key_aliases;
		if (nrep.nKeyAliases)
		    nrep.which|= XkbKeyAliasesMask;
	    }
	    else {
		nrep.which&= ~(XkbKeyNamesMask|XkbKeyAliasesMask);
		nrep.firstKey= nrep.nKeys= 0;
		nrep.nKeyAliases= 0;
	    }
	    XkbComputeGetNamesReplySize(new,&nrep);
	    rep.length+= SIZEOF(xGenericReply)/4+nrep.length;
	}
	if (new->geom==NULL)
	    rep.reported&= ~XkbGBN_GeometryMask;
	else if (rep.reported&XkbGBN_GeometryMask) {
	    grep.type= X_Reply;
	    grep.deviceID= dev->id;
	    grep.sequenceNumber= client->sequence;
	    grep.length= 0;
	    grep.found= True;
	    grep.pad= 0;
	    grep.widthMM= grep.heightMM= 0;
	    grep.nProperties= grep.nColors= grep.nShapes= 0;
	    grep.nSections= grep.nDoodads= 0;
	    grep.baseColorNdx= grep.labelColorNdx= 0;
	    XkbComputeGetGeometryReplySize(new->geom,&grep,None);
	    rep.length+= SIZEOF(xGenericReply)/4+grep.length;
	}
    }

    reported= rep.reported;
    if ( client->swapped ) {
	register int n;
	swaps(&rep.sequenceNumber,n);
	swapl(&rep.length,n);
	swaps(&rep.found,n);
	swaps(&rep.reported,n);
    }
    WriteToClient(client,SIZEOF(xkbGetKbdByNameReply), (char *)&rep);
    if (reported&(XkbGBN_SymbolsMask|XkbGBN_TypesMask))
	XkbSendMap(client,new,&mrep);
    if (reported&XkbGBN_CompatMapMask)
	XkbSendCompatMap(client,new->compat,&crep);
    if (reported&XkbGBN_IndicatorMapMask)
	XkbSendIndicatorMap(client,new->indicators,&irep);
    if (reported&(XkbGBN_KeyNamesMask|XkbGBN_OtherNamesMask))
	XkbSendNames(client,new,&nrep);
    if (reported&XkbGBN_GeometryMask)
	XkbSendGeometry(client,new->geom,&grep,False);
    if (rep.loaded) {
	XkbDescPtr		old_xkb;
	xkbNewKeyboardNotify 	nkn;
	int 			i,nG,nTG;
	old_xkb= xkb;
	xkb= new;
	dev->key->xkbInfo->desc= xkb;
	new= old_xkb; /* so it'll get freed automatically */

	*xkb->ctrls= *old_xkb->ctrls;
	for (nG=nTG=0,i=xkb->min_key_code;i<=xkb->max_key_code;i++) {
	    nG= XkbKeyNumGroups(xkb,i);
	    if (nG>=XkbNumKbdGroups) {
		nTG= XkbNumKbdGroups;
		break;
	    }
	    if (nG>nTG) {
		nTG= nG;
	    }
	}
	xkb->ctrls->num_groups= nTG;

        for (tmpd = inputInfo.devices; tmpd; tmpd = tmpd->next) {
            if (tmpd == dev ||
                (dev->id == inputInfo.keyboard->id && tmpd->key &&
                 tmpd->coreEvents)) {

                memcpy(tmpd->key->modifierMap, xkb->map->modmap,
                       xkb->max_key_code + 1);
                if (tmpd != dev)
                    XkbCopyKeymap(dev->key->xkbInfo->desc,
                                  tmpd->key->xkbInfo->desc, True);
                XkbUpdateCoreDescription(tmpd, True);

                if (tmpd->kbdfeed && tmpd->kbdfeed->xkb_sli) {
                    old_sli = tmpd->kbdfeed->xkb_sli;
                    tmpd->kbdfeed->xkb_sli = NULL;
                    sli = XkbAllocSrvLedInfo(tmpd, tmpd->kbdfeed, NULL, 0);
                    if (sli) {
                        sli->explicitState = old_sli->explicitState;
                        sli->effectiveState = old_sli->effectiveState;
                    }
                    tmpd->kbdfeed->xkb_sli = sli;
                    XkbFreeSrvLedInfo(old_sli);
                }
            }
        }

        /* this should be either a MN or an NKN, depending on whether or not
         * the keycode range changed? */
	nkn.deviceID= nkn.oldDeviceID= dev->id;
	nkn.minKeyCode= new->min_key_code;
	nkn.maxKeyCode= new->max_key_code;
	nkn.oldMinKeyCode= xkb->min_key_code;
	nkn.oldMaxKeyCode= xkb->max_key_code;
	nkn.requestMajor= XkbReqCode;
	nkn.requestMinor= X_kbGetKbdByName;
	nkn.changed= XkbNKN_KeycodesMask;
	if (geom_changed)
	    nkn.changed|= XkbNKN_GeometryMask;
	XkbSendNewKeyboardNotify(dev,&nkn);
    }
    if ((new!=NULL)&&(new!=xkb)) {
	XkbFreeKeyboard(new,XkbAllComponentsMask,True);
	new= NULL;
    }
    if (names.keymap)	{ _XkbFree(names.keymap); names.keymap= NULL; }
    if (names.keycodes)	{ _XkbFree(names.keycodes); names.keycodes= NULL; }
    if (names.types)	{ _XkbFree(names.types); names.types= NULL; }
    if (names.compat)	{ _XkbFree(names.compat); names.compat= NULL; }
    if (names.symbols)	{ _XkbFree(names.symbols); names.symbols= NULL; }
    if (names.geometry)	{ _XkbFree(names.geometry); names.geometry= NULL; }
    return client->noClientException;
}