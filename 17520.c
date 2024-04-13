bool DL_Dxf::processDXFGroup(DL_CreationInterface* creationInterface,
                             int groupCode, const std::string& groupValue) {

    //printf("%d\n", groupCode);
    //printf("%s\n", groupValue.c_str());

    // Init values on first call
    if (firstCall) {
        settingValue[0] = '\0';
        firstCall=false;
    }

    // Indicates comment or dxflib version:
    if (groupCode==999) {
        if (!groupValue.empty()) {
            if (groupValue.substr(0, 6)=="dxflib") {
                libVersion = getLibVersion(groupValue.substr(7));
            }
            
            addComment(creationInterface, groupValue);
        }
    }

    // Indicates start of new entity or variable:
    else if (groupCode==0 || groupCode==9) {
        // If new entity is encountered, the last one is complete.
        // Prepare default attributes for next entity:
        std::string layer = getStringValue(8, "0");

        int width;
        // Compatibility with qcad1:
        if (hasValue(39) && !hasValue(370)) {
            width = getIntValue(39, -1);
        }
        // since autocad 2002:
        else if (hasValue(370)) {
            width = getIntValue(370, -1);
        }
        // default to BYLAYER:
        else {
            width = -1;
        }

        int color;
        color = getIntValue(62, 256);
        int color24;
        color24 = getIntValue(420, -1);
        int handle;
        handle = getInt16Value(5, -1);

        std::string linetype = getStringValue(6, "BYLAYER");

        attrib = DL_Attributes(layer,                   // layer
                               color,                   // color
                               color24,                 // 24 bit color
                               width,                   // width
                               linetype,                // linetype
                               handle);                 // handle
        attrib.setInPaperSpace((bool)getIntValue(67, 0));
        attrib.setLinetypeScale(getRealValue(48, 1.0));
        creationInterface->setAttributes(attrib);

        int elevationGroupCode=30;
        if (currentObjectType==DL_ENTITY_LWPOLYLINE ) {
            // see lwpolyline group codes reference
            elevationGroupCode=38;
        }
        else {
            // see polyline group codes reference
            elevationGroupCode=30;
        }

        creationInterface->setExtrusion(getRealValue(210, 0.0),
                                        getRealValue(220, 0.0),
                                        getRealValue(230, 1.0),
                                        getRealValue(elevationGroupCode, 0.0));

        // Add the previously parsed entity via creationInterface
        switch (currentObjectType) {
        case DL_SETTING:
            addSetting(creationInterface);
            break;

        case DL_LAYER:
            addLayer(creationInterface);
            break;

        case DL_LINETYPE:
            addLinetype(creationInterface);
            break;

        case DL_BLOCK:
            addBlock(creationInterface);
            break;

        case DL_ENDBLK:
            endBlock(creationInterface);
            break;

        case DL_STYLE:
            addTextStyle(creationInterface);
            break;

        case DL_ENTITY_POINT:
            addPoint(creationInterface);
            break;

        case DL_ENTITY_LINE:
            addLine(creationInterface);
            break;

        case DL_ENTITY_XLINE:
            addXLine(creationInterface);
            break;

        case DL_ENTITY_RAY:
            addRay(creationInterface);
            break;

        case DL_ENTITY_POLYLINE:
        case DL_ENTITY_LWPOLYLINE:
            addPolyline(creationInterface);
            break;

        case DL_ENTITY_VERTEX:
            addVertex(creationInterface);
            break;

        case DL_ENTITY_SPLINE:
            addSpline(creationInterface);
            break;

        case DL_ENTITY_ARC:
            addArc(creationInterface);
            break;

        case DL_ENTITY_CIRCLE:
            addCircle(creationInterface);
            break;

        case DL_ENTITY_ELLIPSE:
            addEllipse(creationInterface);
            break;

        case DL_ENTITY_INSERT:
            addInsert(creationInterface);
            break;

        case DL_ENTITY_MTEXT:
            addMText(creationInterface);
            break;

        case DL_ENTITY_TEXT:
            addText(creationInterface);
            break;

        case DL_ENTITY_ARCALIGNEDTEXT:
            addArcAlignedText(creationInterface);
            break;

        case DL_ENTITY_ATTRIB:
            addAttribute(creationInterface);
            break;

        case DL_ENTITY_DIMENSION: {
                int type = (getIntValue(70, 0)&0x07);

                switch (type) {
                case 0:
                    addDimLinear(creationInterface);
                    break;

                case 1:
                    addDimAligned(creationInterface);
                    break;

                case 2:
                    addDimAngular(creationInterface);
                    break;

                case 3:
                    addDimDiametric(creationInterface);
                    break;

                case 4:
                    addDimRadial(creationInterface);
                    break;

                case 5:
                    addDimAngular3P(creationInterface);
                    break;
                
                case 6:
                    addDimOrdinate(creationInterface);
                    break;

                default:
                    break;
                }
            }
            break;

        case DL_ENTITY_LEADER:
            addLeader(creationInterface);
            break;

        case DL_ENTITY_HATCH:
            //addHatch(creationInterface);
            handleHatchData(creationInterface);
            break;

        case DL_ENTITY_IMAGE:
            addImage(creationInterface);
            break;

        case DL_ENTITY_IMAGEDEF:
            addImageDef(creationInterface);
            break;

        case DL_ENTITY_TRACE:
            addTrace(creationInterface);
            break;
        
        case DL_ENTITY_3DFACE:
            add3dFace(creationInterface);
            break;

        case DL_ENTITY_SOLID:
            addSolid(creationInterface);
            break;

        case DL_ENTITY_SEQEND:
            endSequence(creationInterface);
            break;

        default:
            break;
        }

        creationInterface->endSection();

        // reset all values (they are not persistent and only this
        //  way we can set defaults for omitted values)
//        for (int i=0; i<DL_DXF_MAXGROUPCODE; ++i) {
//            values[i][0] = '\0';
//        }
        values.clear();
        settingValue[0] = '\0';
        settingKey = "";
        firstHatchLoop = true;
        //firstHatchEdge = true;
        hatchEdge = DL_HatchEdgeData();
        //xRecordHandle = "";
        xRecordValues = false;

        // Last DXF entity or setting has been handled
        // Now determine what the next entity or setting type is

        int prevEntity = currentObjectType;

        // Read DXF variable:
        if (groupValue[0]=='$') {
            currentObjectType = DL_SETTING;
            settingKey = groupValue;
        }

        // Read Layers:
        else if (groupValue=="LAYER") {
            currentObjectType = DL_LAYER;
        }

        // Read Linetypes:
        else if (groupValue=="LTYPE") {
            currentObjectType = DL_LINETYPE;
        }

        // Read Blocks:
        else if (groupValue=="BLOCK") {
            currentObjectType = DL_BLOCK;
        } else if (groupValue=="ENDBLK") {
            currentObjectType = DL_ENDBLK;
        }

        // Read text styles:
        else if (groupValue=="STYLE") {
            currentObjectType = DL_STYLE;
        }

        // Read entities:
        else if (groupValue=="POINT") {
            currentObjectType = DL_ENTITY_POINT;
        } else if (groupValue=="LINE") {
            currentObjectType = DL_ENTITY_LINE;
        } else if (groupValue=="XLINE") {
            currentObjectType = DL_ENTITY_XLINE;
        } else if (groupValue=="RAY") {
            currentObjectType = DL_ENTITY_RAY;
        } else if (groupValue=="POLYLINE") {
            currentObjectType = DL_ENTITY_POLYLINE;
        } else if (groupValue=="LWPOLYLINE") {
            currentObjectType = DL_ENTITY_LWPOLYLINE;
        } else if (groupValue=="VERTEX") {
            currentObjectType = DL_ENTITY_VERTEX;
        } else if (groupValue=="SPLINE") {
            currentObjectType = DL_ENTITY_SPLINE;
        } else if (groupValue=="ARC") {
            currentObjectType = DL_ENTITY_ARC;
        } else if (groupValue=="ELLIPSE") {
            currentObjectType = DL_ENTITY_ELLIPSE;
        } else if (groupValue=="CIRCLE") {
            currentObjectType = DL_ENTITY_CIRCLE;
        } else if (groupValue=="INSERT") {
            currentObjectType = DL_ENTITY_INSERT;
        } else if (groupValue=="TEXT") {
            currentObjectType = DL_ENTITY_TEXT;
        } else if (groupValue=="MTEXT") {
            currentObjectType = DL_ENTITY_MTEXT;
        } else if (groupValue=="ARCALIGNEDTEXT") {
            currentObjectType = DL_ENTITY_ARCALIGNEDTEXT;
        } else if (groupValue=="ATTRIB") {
            currentObjectType = DL_ENTITY_ATTRIB;
        } else if (groupValue=="DIMENSION") {
            currentObjectType = DL_ENTITY_DIMENSION;
        } else if (groupValue=="LEADER") {
            currentObjectType = DL_ENTITY_LEADER;
        } else if (groupValue=="HATCH") {
            currentObjectType = DL_ENTITY_HATCH;
        } else if (groupValue=="IMAGE") {
            currentObjectType = DL_ENTITY_IMAGE;
        } else if (groupValue=="IMAGEDEF") {
            currentObjectType = DL_ENTITY_IMAGEDEF;
        } else if (groupValue=="TRACE") {
           currentObjectType = DL_ENTITY_TRACE;
        } else if (groupValue=="SOLID") {
           currentObjectType = DL_ENTITY_SOLID;
        } else if (groupValue=="3DFACE") {
           currentObjectType = DL_ENTITY_3DFACE;
        } else if (groupValue=="SEQEND") {
            currentObjectType = DL_ENTITY_SEQEND;
        } else if (groupValue=="XRECORD") {
            currentObjectType = DL_XRECORD;
        } else if (groupValue=="DICTIONARY") {
            currentObjectType = DL_DICTIONARY;
        } else {
            currentObjectType = DL_UNKNOWN;
        }

        // end of old style POLYLINE entity
        if (prevEntity==DL_ENTITY_VERTEX && currentObjectType!=DL_ENTITY_VERTEX) {
            endEntity(creationInterface);
        }

        // TODO: end of SPLINE entity
        //if (prevEntity==DL_ENTITY_CONTROLPOINT && currentEntity!=DL_ENTITY_CONTROLPOINT) {
        //    endEntity(creationInterface);
        //}

        return true;

    } else {
        // Group code does not indicate start of new entity or setting,
        // so this group must be continuation of data for the current
        // one.
        if (groupCode<DL_DXF_MAXGROUPCODE) {

            bool handled = false;

            switch (currentObjectType) {
            case DL_ENTITY_MTEXT:
                handled = handleMTextData(creationInterface);
                break;

            case DL_ENTITY_LWPOLYLINE:
                handled = handleLWPolylineData(creationInterface);
                break;

            case DL_ENTITY_SPLINE:
                handled = handleSplineData(creationInterface);
                break;

            case DL_ENTITY_LEADER:
                handled = handleLeaderData(creationInterface);
                break;

            case DL_ENTITY_HATCH:
                handled = handleHatchData(creationInterface);
                break;

            case DL_XRECORD:
                handled = handleXRecordData(creationInterface);
                break;

            case DL_DICTIONARY:
                handled = handleDictionaryData(creationInterface);
                break;

            case DL_LINETYPE:
                handled = handleLinetypeData(creationInterface);
                break;

            default:
                break;
            }

            // Always try to handle XData, unless we're in an XData record:
            if (currentObjectType!=DL_XRECORD) {
                handled = handleXData(creationInterface);
            }

            if (!handled) {
                // Normal group / value pair:
                values[groupCode] = groupValue;
            }
        }

        return false;
    }
    return false;
}