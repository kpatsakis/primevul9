bool DL_Dxf::checkVariable(const char* var, DL_Codes::version version) {
    if (version>=DL_VERSION_2000) {
        return true;
    } else if (version==DL_VERSION_R12) {
        // these are all the variables recognized by dxf r12:
        if (!strcmp(var, "$ACADVER")) {
            return true;
        }
        if (!strcmp(var, "$ACADVER")) {
            return true;
        }
        if (!strcmp(var, "$ANGBASE")) {
            return true;
        }
        if (!strcmp(var, "$ANGDIR")) {
            return true;
        }
        if (!strcmp(var, "$ATTDIA")) {
            return true;
        }
        if (!strcmp(var, "$ATTMODE")) {
            return true;
        }
        if (!strcmp(var, "$ATTREQ")) {
            return true;
        }
        if (!strcmp(var, "$AUNITS")) {
            return true;
        }
        if (!strcmp(var, "$AUPREC")) {
            return true;
        }
        if (!strcmp(var, "$AXISMODE")) {
            return true;
        }
        if (!strcmp(var, "$AXISUNIT")) {
            return true;
        }
        if (!strcmp(var, "$BLIPMODE")) {
            return true;
        }
        if (!strcmp(var, "$CECOLOR")) {
            return true;
        }
        if (!strcmp(var, "$CELTYPE")) {
            return true;
        }
        if (!strcmp(var, "$CHAMFERA")) {
            return true;
        }
        if (!strcmp(var, "$CHAMFERB")) {
            return true;
        }
        if (!strcmp(var, "$CLAYER")) {
            return true;
        }
        if (!strcmp(var, "$COORDS")) {
            return true;
        }
        if (!strcmp(var, "$DIMALT")) {
            return true;
        }
        if (!strcmp(var, "$DIMALTD")) {
            return true;
        }
        if (!strcmp(var, "$DIMALTF")) {
            return true;
        }
        if (!strcmp(var, "$DIMAPOST")) {
            return true;
        }
        if (!strcmp(var, "$DIMASO")) {
            return true;
        }
        if (!strcmp(var, "$DIMASZ")) {
            return true;
        }
        if (!strcmp(var, "$DIMBLK")) {
            return true;
        }
        if (!strcmp(var, "$DIMBLK1")) {
            return true;
        }
        if (!strcmp(var, "$DIMBLK2")) {
            return true;
        }
        if (!strcmp(var, "$DIMCEN")) {
            return true;
        }
        if (!strcmp(var, "$DIMCLRD")) {
            return true;
        }
        if (!strcmp(var, "$DIMCLRE")) {
            return true;
        }
        if (!strcmp(var, "$DIMCLRT")) {
            return true;
        }
        if (!strcmp(var, "$DIMDLE")) {
            return true;
        }
        if (!strcmp(var, "$DIMDLI")) {
            return true;
        }
        if (!strcmp(var, "$DIMEXE")) {
            return true;
        }
        if (!strcmp(var, "$DIMEXO")) {
            return true;
        }
        if (!strcmp(var, "$DIMGAP")) {
            return true;
        }
        if (!strcmp(var, "$DIMLFAC")) {
            return true;
        }
        if (!strcmp(var, "$DIMLIM")) {
            return true;
        }
        if (!strcmp(var, "$DIMPOST")) {
            return true;
        }
        if (!strcmp(var, "$DIMRND")) {
            return true;
        }
        if (!strcmp(var, "$DIMSAH")) {
            return true;
        }
        if (!strcmp(var, "$DIMSCALE")) {
            return true;
        }
        if (!strcmp(var, "$DIMSE1")) {
            return true;
        }
        if (!strcmp(var, "$DIMSE2")) {
            return true;
        }
        if (!strcmp(var, "$DIMSHO")) {
            return true;
        }
        if (!strcmp(var, "$DIMSOXD")) {
            return true;
        }
        if (!strcmp(var, "$DIMSTYLE")) {
            return true;
        }
        if (!strcmp(var, "$DIMTAD")) {
            return true;
        }
        if (!strcmp(var, "$DIMTFAC")) {
            return true;
        }
        if (!strcmp(var, "$DIMTIH")) {
            return true;
        }
        if (!strcmp(var, "$DIMTIX")) {
            return true;
        }
        if (!strcmp(var, "$DIMTM")) {
            return true;
        }
        if (!strcmp(var, "$DIMTOFL")) {
            return true;
        }
        if (!strcmp(var, "$DIMTOH")) {
            return true;
        }
        if (!strcmp(var, "$DIMTOL")) {
            return true;
        }
        if (!strcmp(var, "$DIMTP")) {
            return true;
        }
        if (!strcmp(var, "$DIMTSZ")) {
            return true;
        }
        if (!strcmp(var, "$DIMTVP")) {
            return true;
        }
        if (!strcmp(var, "$DIMTXT")) {
            return true;
        }
        if (!strcmp(var, "$DIMZIN")) {
            return true;
        }
        if (!strcmp(var, "$DWGCODEPAGE")) {
            return true;
        }
        if (!strcmp(var, "$DRAGMODE")) {
            return true;
        }
        if (!strcmp(var, "$ELEVATION")) {
            return true;
        }
        if (!strcmp(var, "$EXTMAX")) {
            return true;
        }
        if (!strcmp(var, "$EXTMIN")) {
            return true;
        }
        if (!strcmp(var, "$FILLETRAD")) {
            return true;
        }
        if (!strcmp(var, "$FILLMODE")) {
            return true;
        }
        if (!strcmp(var, "$HANDLING")) {
            return true;
        }
        if (!strcmp(var, "$HANDSEED")) {
            return true;
        }
        if (!strcmp(var, "$INSBASE")) {
            return true;
        }
        if (!strcmp(var, "$LIMCHECK")) {
            return true;
        }
        if (!strcmp(var, "$LIMMAX")) {
            return true;
        }
        if (!strcmp(var, "$LIMMIN")) {
            return true;
        }
        if (!strcmp(var, "$LTSCALE")) {
            return true;
        }
        if (!strcmp(var, "$LUNITS")) {
            return true;
        }
        if (!strcmp(var, "$LUPREC")) {
            return true;
        }
        if (!strcmp(var, "$MAXACTVP")) {
            return true;
        }
        if (!strcmp(var, "$MENU")) {
            return true;
        }
        if (!strcmp(var, "$MIRRTEXT")) {
            return true;
        }
        if (!strcmp(var, "$ORTHOMODE")) {
            return true;
        }
        if (!strcmp(var, "$OSMODE")) {
            return true;
        }
        if (!strcmp(var, "$PDMODE")) {
            return true;
        }
        if (!strcmp(var, "$PDSIZE")) {
            return true;
        }
        if (!strcmp(var, "$PELEVATION")) {
            return true;
        }
        if (!strcmp(var, "$PEXTMAX")) {
            return true;
        }
        if (!strcmp(var, "$PEXTMIN")) {
            return true;
        }
        if (!strcmp(var, "$PLIMCHECK")) {
            return true;
        }
        if (!strcmp(var, "$PLIMMAX")) {
            return true;
        }
        if (!strcmp(var, "$PLIMMIN")) {
            return true;
        }
        if (!strcmp(var, "$PLINEGEN")) {
            return true;
        }
        if (!strcmp(var, "$PLINEWID")) {
            return true;
        }
        if (!strcmp(var, "$PSLTSCALE")) {
            return true;
        }
        if (!strcmp(var, "$PUCSNAME")) {
            return true;
        }
        if (!strcmp(var, "$PUCSORG")) {
            return true;
        }
        if (!strcmp(var, "$PUCSXDIR")) {
            return true;
        }
        if (!strcmp(var, "$PUCSYDIR")) {
            return true;
        }
        if (!strcmp(var, "$QTEXTMODE")) {
            return true;
        }
        if (!strcmp(var, "$REGENMODE")) {
            return true;
        }
        if (!strcmp(var, "$SHADEDGE")) {
            return true;
        }
        if (!strcmp(var, "$SHADEDIF")) {
            return true;
        }
        if (!strcmp(var, "$SKETCHINC")) {
            return true;
        }
        if (!strcmp(var, "$SKPOLY")) {
            return true;
        }
        if (!strcmp(var, "$SPLFRAME")) {
            return true;
        }
        if (!strcmp(var, "$SPLINESEGS")) {
            return true;
        }
        if (!strcmp(var, "$SPLINETYPE")) {
            return true;
        }
        if (!strcmp(var, "$SURFTAB1")) {
            return true;
        }
        if (!strcmp(var, "$SURFTAB2")) {
            return true;
        }
        if (!strcmp(var, "$SURFTYPE")) {
            return true;
        }
        if (!strcmp(var, "$SURFU")) {
            return true;
        }
        if (!strcmp(var, "$SURFV")) {
            return true;
        }
        if (!strcmp(var, "$TDCREATE")) {
            return true;
        }
        if (!strcmp(var, "$TDINDWG")) {
            return true;
        }
        if (!strcmp(var, "$TDUPDATE")) {
            return true;
        }
        if (!strcmp(var, "$TDUSRTIMER")) {
            return true;
        }
        if (!strcmp(var, "$TEXTSIZE")) {
            return true;
        }
        if (!strcmp(var, "$TEXTSTYLE")) {
            return true;
        }
        if (!strcmp(var, "$THICKNESS")) {
            return true;
        }
        if (!strcmp(var, "$TILEMODE")) {
            return true;
        }
        if (!strcmp(var, "$TRACEWID")) {
            return true;
        }
        if (!strcmp(var, "$UCSNAME")) {
            return true;
        }
        if (!strcmp(var, "$UCSORG")) {
            return true;
        }
        if (!strcmp(var, "$UCSXDIR")) {
            return true;
        }
        if (!strcmp(var, "$UCSYDIR")) {
            return true;
        }
        if (!strcmp(var, "$UNITMODE")) {
            return true;
        }
        if (!strcmp(var, "$USERI1")) {
            return true;
        }
        if (!strcmp(var, "$USERR1")) {
            return true;
        }
        if (!strcmp(var, "$USRTIMER")) {
            return true;
        }
        if (!strcmp(var, "$VISRETAIN")) {
            return true;
        }
        if (!strcmp(var, "$WORLDVIEW")) {
            return true;
        }
        if (!strcmp(var, "$FASTZOOM")) {
            return true;
        }
        if (!strcmp(var, "$GRIDMODE")) {
            return true;
        }
        if (!strcmp(var, "$GRIDUNIT")) {
            return true;
        }
        if (!strcmp(var, "$SNAPANG")) {
            return true;
        }
        if (!strcmp(var, "$SNAPBASE")) {
            return true;
        }
        if (!strcmp(var, "$SNAPISOPAIR")) {
            return true;
        }
        if (!strcmp(var, "$SNAPMODE")) {
            return true;
        }
        if (!strcmp(var, "$SNAPSTYLE")) {
            return true;
        }
        if (!strcmp(var, "$SNAPUNIT")) {
            return true;
        }
        if (!strcmp(var, "$VIEWCTR")) {
            return true;
        }
        if (!strcmp(var, "$VIEWDIR")) {
            return true;
        }
        if (!strcmp(var, "$VIEWSIZE")) {
            return true;
        }
        return false;
    }

    return false;
}