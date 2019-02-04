// -----------------------------------------------------------------------------
// Copyright (C) 2017  Ludovic LE FRIOUX
//
// This file is part of PaInleSS.
//
// PaInleSS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// -----------------------------------------------------------------------------

#pragma once
#ifndef SHARING_RADAR_SHARING_H
#define SHARING_RADAR_SHARING_H

#include "../sharing/SharingStrategy.h"
#include "../solvers/SolverInterface.h"
#include <vector>
#include <unordered_map>
#include <sstream>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime> 

/// Simple sharing is a all to all sharing.
class RadarSatSharing : public SharingStrategy
{
public:
   /// Constructor.
   RadarSatSharing();

   /// This method all the shared clauses from the producers to the consumers.
   void doSharing(int idSharer, const vector<SolverInterface *> & from,
                  const vector<SolverInterface *> & to);

   /// Return the sharing statistics of this sharng strategy.
   SharingStatistics getStatistics();

    /// Hashtable for repeateble clauses
    struct VectorHash {
      size_t operator()(const std::vector<int>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
      }
    };
    typedef std::unordered_map<vector<int>, int, VectorHash> hashtable;
    hashtable hashtable1;

    /// Count for sharing with repeats
    int sharingcount = 0;

protected:
   /// Used to manipulate clauses.
   vector<ClauseExchange *> tmp;
   
   /// Sharing statistics.
   SharingStatistics stats;
};

#endif /* SHARING_HORDESAT_SHARING_H */