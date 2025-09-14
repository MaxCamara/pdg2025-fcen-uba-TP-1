//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:14:44 gtaubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: <Your Name>
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  if(filename!=(char*)0) {

    // Check these conditions

    // 1) the SceneGraph should have a single child
    if(wrl.getNumberOfChildren() != 1) return success;
    Node* child = wrl[0];

    // 2) the child should be a Shape node
    if(!(child->isShape())) return success;
    Shape* shape = (Shape*) child;

    // 3) the geometry of the Shape node should be an IndexedFaceSet node
    Node* geometry = shape->getGeometry();
    if(!(geometry->isIndexedFaceSet())) return success;
    IndexedFaceSet* ifs = (IndexedFaceSet*) geometry;

    // - construct an instance of the Faces class from the IndexedFaceSet
    // - remember to delete it when you are done with it (if necessary)
    //   before returning

    vector<float> coord = ifs->getCoord();
    vector<int> coordIndex = ifs->getCoordIndex();

    int nV = coord.size()/3;

    Faces* faces = new Faces(nV, coordIndex);

    // 4) the IndexedFaceSet should be a triangle mesh
    if(!(ifs->isTriangleMesh())){
        delete faces;
        return success;
    }

    // 5) the IndexedFaceSet should have normals per face
    if(ifs->getNormalPerVertex()){
        delete faces;
        return success;
    }

    // if (all the conditions are satisfied) {

    FILE* fp = fopen(filename,"w");
    if(	fp!=(FILE*)0) {

      // if set, use ifs->getName()
      // otherwise use filename,
      // but first remove directory and extension

      string name = ifs->getName();
      if(name == ""){
          string rawFilename = string(filename);
          int extIndex = rawFilename.find_last_of('.');
          int dirIndex = rawFilename.find_last_of('/');
          rawFilename = (rawFilename.erase(extIndex)).erase(0,dirIndex+1);
          fprintf(fp,"solid %s\n",rawFilename.c_str());
      } else {
          fprintf(fp,"solid %s\n",name.c_str());
      }

      //fprintf(fp,"solid %s\n",filename);

      int nF = ifs->getNumberOfFaces();
      for(int iF=0; iF<nF; iF++){
          vector<float> normals = ifs->getNormal();
          fprintf(fp, "facet normal %f %f %f\n", normals[iF*3], normals[iF*3+1], normals[iF*3+2]);
          fprintf(fp, "  outer loop\n");
          int faceSize = faces->getFaceSize(iF);
          int iC = faces->getFaceFirstCorner(iF);
          int i=0;
          do{
              int iV = coordIndex[iC];
              fprintf(fp, "    vertex %f %f %f\n", coord[3*iV], coord[3*iV+1], coord[3*iV+2]);
              iC = faces->getNextCorner(iC);
              i++;
          } while(i<faceSize);
          fprintf(fp, "  endloop\n");
          fprintf(fp, "endfacet\n");
      }

      // TODO ...
      // for each face {
      //   ...
      // }
      
      fclose(fp);
      success = true;
    }

    // } endif (all the conditions are satisfied)

    delete(faces);
  }
  return success;
}
