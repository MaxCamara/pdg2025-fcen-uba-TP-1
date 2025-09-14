//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:09:56 gtaubin>
//------------------------------------------------------------------------
//
// Faces.cpp
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

#include <math.h>
#include "Faces.hpp"

#include <iostream> //Para el manejo de excepciones
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
  try{
      if(nV<=0) throw 1;
      _faceIndex = {};
      for(int i=0; i<coordIndex.size(); i++){
          if(coordIndex[i] < -1 || coordIndex[i] >= nV) throw 2;
          if(i == 0 || coordIndex[i-1] == -1) _faceIndex.push_back(i);
      }
  }
  catch(int errNo){
      if(errNo == 1) cout << "Error: Cantidad incorrecta de vértices.";
      if(errNo == 2) cout << "Error: Índice de vértice incorrecto.";
  }
  _nV = nV;
  _coordIndex = coordIndex;
  _faceIndex.push_back(coordIndex.size()); //Agrego un índice que apunta después del último -1, para mejorar la eficiencia de ciertas operaciones.
}

int Faces::getNumberOfVertices() const {
  return _nV;
}

int Faces::getNumberOfFaces() const {
    return _faceIndex.size()-1; //Le resto 1 ya que la última posición no apunta a una cara real
}

int Faces::getNumberOfCorners() const {
    return _coordIndex.size();
}

int Faces::getFaceSize(const int iF) const {
  try{
      if(iF < 0 || iF >= getNumberOfFaces()){
          throw 1;
      }
  }
  catch(...){
      cout << "Error: Índice de cara incorrecto.";
  }
  int firstIndexFace = getFaceFirstCorner(iF);
  int indexFaceSeparator = getFaceFirstCorner(iF+1) - 1; //El índice del separador de la cara es la posición anterior al índice de la cara siguiente
  return indexFaceSeparator - firstIndexFace;
}

int Faces::getFaceFirstCorner(const int iF) const {
  try{
      if(iF < 0 || iF >= getNumberOfFaces()){
          throw 1;
      }
  }
  catch(...){
      cout << "Error: Índice de cara incorrecto.";
  }
  return _faceIndex[iF];
}

int Faces::getFaceVertex(const int iF, const int j) const {
  try{
      if(iF < 0 || iF >= getNumberOfFaces()){
          throw 1;
      }
      if(j < 0 || j >= getFaceSize(iF)){
          throw 2;
      }
  }
  catch(int errNo){
      if(errNo == 1){
          cout << "Error: Índice de cara incorrecto.";
      }
      if(errNo == 2){
          cout << "Error: Número de esquina fuera del tamaño de la cara.";
      }
  }
  int vertexIndex = getFaceFirstCorner(iF) + j;
  return _coordIndex[vertexIndex];
}

int Faces::getCornerFace(const int iC) const {
  try{
      if(iC < 0 || iC >= getNumberOfCorners()){
          throw 1;
      }
  }
  catch(...){
      cout << "Error: Índice de esquina incorrecto.";
  }
  int result = -1;
  if(_coordIndex[iC] != -1){
      int candidateFace = _faceIndex[getNumberOfFaces()-2]; //El primer candidato es la última cara
      while(getFaceFirstCorner(candidateFace) > iC){
          candidateFace--;                             //Recorro las caras desde la última hasta encontrar la primera que empiece antes
      }
      result = candidateFace;
  }
  return result;   //Obs: Este método se podría hacer mas eficiente haciendo una búsqueda binaria sobre _faceIndex en lugar de un barrido lineal
}

int Faces::getNextCorner(const int iC) const {
  try{
      if(iC < 0 || iC >= getNumberOfCorners()){
          throw 1;
      }
  }
  catch(...){
      cout << "Error: Índice de esquina incorrecto.";
  }
  int result = -1;
  if(_coordIndex[iC] != -1){
      int cornerFace = getCornerFace(iC);
      if(_coordIndex[iC+1] != -1){
          result = iC+1;
      } else {
          result = cornerFace;  //Si la posición siguiente en coordIndex es el separador, la siguiente esquina es la primer esquina de la cara
      }
  }
  return result;
}

