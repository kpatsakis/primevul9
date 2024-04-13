
    Tfloat _cubic_atXYZ(const float fx, const float fy, const float fz, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0,width() - 1),
        nfy = cimg::cut(fy,0,height() - 1),
        nfz = cimg::cut(fz,0,depth() - 1);
      const int x = (int)nfx, y = (int)nfy, z = (int)nfz;
      const float dx = nfx - x, dy = nfy - y, dz = nfz - z;
      const int
        px = x - 1<0?0:x - 1, nx = dx>0?x + 1:x, ax = x + 2>=width()?width() - 1:x + 2,
        py = y - 1<0?0:y - 1, ny = dy>0?y + 1:y, ay = y + 2>=height()?height() - 1:y + 2,
        pz = z - 1<0?0:z - 1, nz = dz>0?z + 1:z, az = z + 2>=depth()?depth() - 1:z + 2;
      const Tfloat
        Ippp = (Tfloat)(*this)(px,py,pz,c), Icpp = (Tfloat)(*this)(x,py,pz,c),
        Inpp = (Tfloat)(*this)(nx,py,pz,c), Iapp = (Tfloat)(*this)(ax,py,pz,c),
        Ipp = Icpp + 0.5f*(dx*(-Ippp + Inpp) + dx*dx*(2*Ippp - 5*Icpp + 4*Inpp - Iapp) +
                           dx*dx*dx*(-Ippp + 3*Icpp - 3*Inpp + Iapp)),
        Ipcp = (Tfloat)(*this)(px,y,pz,c),  Iccp = (Tfloat)(*this)(x, y,pz,c),
        Incp = (Tfloat)(*this)(nx,y,pz,c),  Iacp = (Tfloat)(*this)(ax,y,pz,c),
        Icp = Iccp + 0.5f*(dx*(-Ipcp + Incp) + dx*dx*(2*Ipcp - 5*Iccp + 4*Incp - Iacp) +
                           dx*dx*dx*(-Ipcp + 3*Iccp - 3*Incp + Iacp)),
        Ipnp = (Tfloat)(*this)(px,ny,pz,c), Icnp = (Tfloat)(*this)(x,ny,pz,c),
        Innp = (Tfloat)(*this)(nx,ny,pz,c), Ianp = (Tfloat)(*this)(ax,ny,pz,c),
        Inp = Icnp + 0.5f*(dx*(-Ipnp + Innp) + dx*dx*(2*Ipnp - 5*Icnp + 4*Innp - Ianp) +
                           dx*dx*dx*(-Ipnp + 3*Icnp - 3*Innp + Ianp)),
        Ipap = (Tfloat)(*this)(px,ay,pz,c), Icap = (Tfloat)(*this)(x,ay,pz,c),
        Inap = (Tfloat)(*this)(nx,ay,pz,c), Iaap = (Tfloat)(*this)(ax,ay,pz,c),
        Iap = Icap + 0.5f*(dx*(-Ipap + Inap) + dx*dx*(2*Ipap - 5*Icap + 4*Inap - Iaap) +
                           dx*dx*dx*(-Ipap + 3*Icap - 3*Inap + Iaap)),
        Ip = Icp + 0.5f*(dy*(-Ipp + Inp) + dy*dy*(2*Ipp - 5*Icp + 4*Inp - Iap) +
                         dy*dy*dy*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ippc = (Tfloat)(*this)(px,py,z,c), Icpc = (Tfloat)(*this)(x,py,z,c),
        Inpc = (Tfloat)(*this)(nx,py,z,c), Iapc = (Tfloat)(*this)(ax,py,z,c),
        Ipc = Icpc + 0.5f*(dx*(-Ippc + Inpc) + dx*dx*(2*Ippc - 5*Icpc + 4*Inpc - Iapc) +
                           dx*dx*dx*(-Ippc + 3*Icpc - 3*Inpc + Iapc)),
        Ipcc = (Tfloat)(*this)(px,y,z,c),  Iccc = (Tfloat)(*this)(x, y,z,c),
        Incc = (Tfloat)(*this)(nx,y,z,c),  Iacc = (Tfloat)(*this)(ax,y,z,c),
        Icc = Iccc + 0.5f*(dx*(-Ipcc + Incc) + dx*dx*(2*Ipcc - 5*Iccc + 4*Incc - Iacc) +
                           dx*dx*dx*(-Ipcc + 3*Iccc - 3*Incc + Iacc)),
        Ipnc = (Tfloat)(*this)(px,ny,z,c), Icnc = (Tfloat)(*this)(x,ny,z,c),
        Innc = (Tfloat)(*this)(nx,ny,z,c), Ianc = (Tfloat)(*this)(ax,ny,z,c),
        Inc = Icnc + 0.5f*(dx*(-Ipnc + Innc) + dx*dx*(2*Ipnc - 5*Icnc + 4*Innc - Ianc) +
                           dx*dx*dx*(-Ipnc + 3*Icnc - 3*Innc + Ianc)),
        Ipac = (Tfloat)(*this)(px,ay,z,c), Icac = (Tfloat)(*this)(x,ay,z,c),
        Inac = (Tfloat)(*this)(nx,ay,z,c), Iaac = (Tfloat)(*this)(ax,ay,z,c),
        Iac = Icac + 0.5f*(dx*(-Ipac + Inac) + dx*dx*(2*Ipac - 5*Icac + 4*Inac - Iaac) +
                           dx*dx*dx*(-Ipac + 3*Icac - 3*Inac + Iaac)),
        Ic = Icc + 0.5f*(dy*(-Ipc + Inc) + dy*dy*(2*Ipc - 5*Icc + 4*Inc - Iac) +
                         dy*dy*dy*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ippn = (Tfloat)(*this)(px,py,nz,c), Icpn = (Tfloat)(*this)(x,py,nz,c),
        Inpn = (Tfloat)(*this)(nx,py,nz,c), Iapn = (Tfloat)(*this)(ax,py,nz,c),
        Ipn = Icpn + 0.5f*(dx*(-Ippn + Inpn) + dx*dx*(2*Ippn - 5*Icpn + 4*Inpn - Iapn) +
                           dx*dx*dx*(-Ippn + 3*Icpn - 3*Inpn + Iapn)),
        Ipcn = (Tfloat)(*this)(px,y,nz,c),  Iccn = (Tfloat)(*this)(x, y,nz,c),
        Incn = (Tfloat)(*this)(nx,y,nz,c),  Iacn = (Tfloat)(*this)(ax,y,nz,c),
        Icn = Iccn + 0.5f*(dx*(-Ipcn + Incn) + dx*dx*(2*Ipcn - 5*Iccn + 4*Incn - Iacn) +
                           dx*dx*dx*(-Ipcn + 3*Iccn - 3*Incn + Iacn)),
        Ipnn = (Tfloat)(*this)(px,ny,nz,c), Icnn = (Tfloat)(*this)(x,ny,nz,c),
        Innn = (Tfloat)(*this)(nx,ny,nz,c), Iann = (Tfloat)(*this)(ax,ny,nz,c),
        Inn = Icnn + 0.5f*(dx*(-Ipnn + Innn) + dx*dx*(2*Ipnn - 5*Icnn + 4*Innn - Iann) +
                           dx*dx*dx*(-Ipnn + 3*Icnn - 3*Innn + Iann)),
        Ipan = (Tfloat)(*this)(px,ay,nz,c), Ican = (Tfloat)(*this)(x,ay,nz,c),
        Inan = (Tfloat)(*this)(nx,ay,nz,c), Iaan = (Tfloat)(*this)(ax,ay,nz,c),
        Ian = Ican + 0.5f*(dx*(-Ipan + Inan) + dx*dx*(2*Ipan - 5*Ican + 4*Inan - Iaan) +
                           dx*dx*dx*(-Ipan + 3*Ican - 3*Inan + Iaan)),
        In = Icn + 0.5f*(dy*(-Ipn + Inn) + dy*dy*(2*Ipn - 5*Icn + 4*Inn - Ian) +
                         dy*dy*dy*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ippa = (Tfloat)(*this)(px,py,az,c), Icpa = (Tfloat)(*this)(x,py,az,c),
        Inpa = (Tfloat)(*this)(nx,py,az,c), Iapa = (Tfloat)(*this)(ax,py,az,c),
        Ipa = Icpa + 0.5f*(dx*(-Ippa + Inpa) + dx*dx*(2*Ippa - 5*Icpa + 4*Inpa - Iapa) +
                           dx*dx*dx*(-Ippa + 3*Icpa - 3*Inpa + Iapa)),
        Ipca = (Tfloat)(*this)(px,y,az,c),  Icca = (Tfloat)(*this)(x, y,az,c),
        Inca = (Tfloat)(*this)(nx,y,az,c),  Iaca = (Tfloat)(*this)(ax,y,az,c),
        Ica = Icca + 0.5f*(dx*(-Ipca + Inca) + dx*dx*(2*Ipca - 5*Icca + 4*Inca - Iaca) +
                           dx*dx*dx*(-Ipca + 3*Icca - 3*Inca + Iaca)),
        Ipna = (Tfloat)(*this)(px,ny,az,c), Icna = (Tfloat)(*this)(x,ny,az,c),
        Inna = (Tfloat)(*this)(nx,ny,az,c), Iana = (Tfloat)(*this)(ax,ny,az,c),
        Ina = Icna + 0.5f*(dx*(-Ipna + Inna) + dx*dx*(2*Ipna - 5*Icna + 4*Inna - Iana) +
                           dx*dx*dx*(-Ipna + 3*Icna - 3*Inna + Iana)),
        Ipaa = (Tfloat)(*this)(px,ay,az,c), Icaa = (Tfloat)(*this)(x,ay,az,c),
        Inaa = (Tfloat)(*this)(nx,ay,az,c), Iaaa = (Tfloat)(*this)(ax,ay,az,c),
        Iaa = Icaa + 0.5f*(dx*(-Ipaa + Inaa) + dx*dx*(2*Ipaa - 5*Icaa + 4*Inaa - Iaaa) +
                           dx*dx*dx*(-Ipaa + 3*Icaa - 3*Inaa + Iaaa)),
        Ia = Ica + 0.5f*(dy*(-Ipa + Ina) + dy*dy*(2*Ipa - 5*Ica + 4*Ina - Iaa) +
                         dy*dy*dy*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dz*(-Ip + In) + dz*dz*(2*Ip - 5*Ic + 4*In - Ia) + dz*dz*dz*(-Ip + 3*Ic - 3*In + Ia));