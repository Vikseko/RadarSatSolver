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

#include "../sharing/RadarSatSharing.h"
#include "../utils/Parameters.h"
#include "../clauses/ClauseManager.h"
#include "../solvers/SolverFactory.h"

RadarSatSharing::RadarSatSharing()
{
}

void
RadarSatSharing::doSharing(int idSharer, const vector<SolverInterface *> & from,
                         const vector<SolverInterface *> & to)
{
   sharingcount ++;

   if(sharingcount <= 50) {
      for (int i = 0; i < from.size(); i++) {
         tmp.clear();

         from[i]->getLearnedClauses(tmp);

         stats.receivedClauses += tmp.size();
         stats.sharedClauses   += tmp.size();
         
         for (size_t k = 0; k < tmp.size(); k++) {
            std::vector<int> vecclause;
            tmp[k]->repeat = true;

            for(int s = 0; s < tmp[k]->size; s++)
            {
               vecclause.push_back(tmp[k]->lits[s]);
            }

            auto it = hashtable1.find(vecclause);

            if(it != hashtable1.end())
            {
               it->second++;
               if(it->second==2)
               tmp[k]->repeat = false;
            }
            else
            {
               hashtable1[vecclause] = 1;
            }

            /*for(auto clause : hashtable1)
            {
               if(vecclause == clause.first && clause.second>1)
               {
                    tmp[k]->repeat = false;
               }
            }*/
         }

         for (size_t j = 0; j < to.size(); j++) {
            for (size_t k = 0; k < tmp.size(); k++) {
                  ClauseManager::increaseClause(tmp[k], 1);
            }
            to[j]->addLearnedClauses(tmp);
         }
            
         for (size_t k = 0; k < tmp.size(); k++) {
            ClauseManager::releaseClause(tmp[k]);
         }
      }
   }
   else
   {
      if(sharingcount==51){
         hashtable1.clear();
      }
      for (int i = 0; i < from.size(); i++) {
         tmp.clear();

         from[i]->getLearnedClauses(tmp);

         stats.receivedClauses += tmp.size();
         stats.sharedClauses   += tmp.size();

         for (size_t j = 0; j < to.size(); j++) {
            if (i != j) {
               for (size_t k = 0; k < tmp.size(); k++) {
                  tmp[k]->repeat = true;
                  ClauseManager::increaseClause(tmp[k], 1);
               }
               to[j]->addLearnedClauses(tmp);
            }
         }
            
         for (size_t k = 0; k < tmp.size(); k++) {
            ClauseManager::releaseClause(tmp[k]);
         }
      }
   }
}

SharingStatistics
RadarSatSharing::getStatistics()
{
   return stats;
}