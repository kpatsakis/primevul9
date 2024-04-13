    template<typename tf, typename t>
    static CImg<T> dijkstra(const tf& distance, const unsigned int nb_nodes,
                            const unsigned int starting_node, const unsigned int ending_node,
                            CImg<t>& previous_node) {
      if (starting_node>=nb_nodes)
        throw CImgArgumentException("CImg<%s>::dijkstra(): Specified indice of starting node %u is higher "
                                    "than number of nodes %u.",
                                    pixel_type(),starting_node,nb_nodes);
      CImg<T> dist(1,nb_nodes,1,1,cimg::type<T>::max());
      dist(starting_node) = 0;
      previous_node.assign(1,nb_nodes,1,1,(t)-1);
      previous_node(starting_node) = (t)starting_node;
      CImg<uintT> Q(nb_nodes);
      cimg_forX(Q,u) Q(u) = (unsigned int)u;
      cimg::swap(Q(starting_node),Q(0));
      unsigned int sizeQ = nb_nodes;
      while (sizeQ) {
        // Update neighbors from minimal vertex
        const unsigned int umin = Q(0);
        if (umin==ending_node) sizeQ = 0;
        else {
          const T dmin = dist(umin);
          const T infty = cimg::type<T>::max();
          for (unsigned int q = 1; q<sizeQ; ++q) {
            const unsigned int v = Q(q);
            const T d = (T)distance(v,umin);
            if (d<infty) {
              const T alt = dmin + d;
              if (alt<dist(v)) {
                dist(v) = alt;
                previous_node(v) = (t)umin;
                const T distpos = dist(Q(q));
                for (unsigned int pos = q, par = 0; pos && distpos<dist(Q(par=(pos + 1)/2 - 1)); pos=par)
                  cimg::swap(Q(pos),Q(par));
              }
            }
          }
          // Remove minimal vertex from queue
          Q(0) = Q(--sizeQ);
          const T distpos = dist(Q(0));
          for (unsigned int pos = 0, left = 0, right = 0;
               ((right=2*(pos + 1),(left=right - 1))<sizeQ && distpos>dist(Q(left))) ||
                 (right<sizeQ && distpos>dist(Q(right)));) {
            if (right<sizeQ) {
              if (dist(Q(left))<dist(Q(right))) { cimg::swap(Q(pos),Q(left)); pos = left; }
              else { cimg::swap(Q(pos),Q(right)); pos = right; }
            } else { cimg::swap(Q(pos),Q(left)); pos = left; }
          }
        }
      }
      return dist;