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

// MiniSat includes
#include "minisat/core/Dimacs.h"
#include "minisat/simp/SimpSolver.h"
#include "minisat/utils/System.h"

#include "../clauses/ClauseManager.h"
#include "../solvers/MiniSat.h"
#include "../utils/Parameters.h"

using namespace Minisat;

// Macros for minisat literal representation conversion
#define MINI_LIT(lit) lit > 0 ? mkLit(lit - 1, false) : mkLit((-lit) - 1, true)

#define INT_LIT(lit) sign(lit) ? -(var(lit) + 1) : (var(lit) + 1)


static void makeMiniVec(ClauseExchange * cls, vec<Lit> & mcls)
{
   for (size_t i = 0; i < cls->size; i++) {
      mcls.push(MINI_LIT(cls->lits[i]));
   }
}


void miniLearnCallback(const vec<Lit> & cls, void * issuer)
{
	MiniSat* mp = (MiniSat*)issuer;

	if (cls.size() > mp->sizeLimit)
		return;

	ClauseExchange * ncls = ClauseManager::allocClause(cls.size());

	if (cls.size() > 1) {
		// fake glue value
		int madeUpGlue = min(3, cls.size());

      ncls->lbd = madeUpGlue;
	}

	for (int i = 0; i < cls.size(); i++) {
		ncls->lits[i] = INT_LIT(cls[i]);
	}

   ncls->from = mp->id;

   mp->clausesToExport.addClause(ncls);
}

MiniSat::MiniSat(int id) : SolverInterface(id, MINISAT)
{
	sizeLimit = Parameters::getIntParam("lbd-limit", 2);

	solver = new SimpSolver();

	solver->learnedClsCallback = miniLearnCallback;
	solver->issuer             = this;
}

MiniSat::~MiniSat()
{
	delete solver;
}

bool
MiniSat::loadFormula(const char* filename)
{
    gzFile in = gzopen(filename, "rb");

    parse_DIMACS(in, *solver);

    gzclose(in);

    return true;
}

//Get the number of variables of the formula
int
MiniSat::getVariablesCount()
{
	return solver->nVars();
}

// Get a variable suitable for search splitting
int
MiniSat::getDivisionVariable()
{
   return (rand() % getVariablesCount()) + 1;
}

// Set initial phase for a given variable
void
MiniSat::setPhase(const int var, const bool phase)
{
	solver->setPolarity(var - 1, phase ? l_True : l_False);
}

// Bump activity for a given variable
void
MiniSat::bumpVariableActivity(const int var, const int times)
{
   for(int i = 0; i < times; i++) {
      solver->varBumpActivity(var - 1);
   }
}

// Interrupt the SAT solving, so it can be started again with new assumptions
void
MiniSat::setSolverInterrupt()
{
   stopSolver = true;

	solver->interrupt();
}

void
MiniSat::unsetSolverInterrupt()
{
   stopSolver = false;

	solver->clearInterrupt();
}

// Diversify the solver
void
MiniSat::diversify(int id)
{
	solver->random_seed = (double)id;
}

// Solve the formula with a given set of assumptions
// return 10 for SAT, 20 for UNSAT, 0 for UNKNOWN
SatResult
MiniSat::solve(const vector<int> & cube)
{
   vector<ClauseExchange *> tmp;

   while (stopSolver == false) {
      tmp.clear();
      clausesToAdd.getClauses(tmp);

      for (size_t ind = 0; ind < tmp.size(); ind++) {
         vec<Lit> mcls;
         makeMiniVec(tmp[ind], mcls);

         ClauseManager::releaseClause(tmp[ind]);

         if (solver->addClause(mcls) == false) {
            printf("unsat when adding cls\n");
            return UNSAT;
         }
      }

      tmp.clear();

      clausesToImport.getClauses(tmp);

      for (size_t ind = 0; ind < tmp.size(); ind++) {
         vec<Lit> mcls;
         makeMiniVec(tmp[ind], mcls);

         ClauseManager::releaseClause(tmp[ind]);

         solver->addLearnedClause(mcls);
      }

      vec<Lit> miniAssumptions;
      for (size_t ind = 0; ind < cube.size(); ind++) {
         miniAssumptions.push(MINI_LIT(cube[ind]));
      }

      lbool res = solver->solveLimited(miniAssumptions);

      if (res == l_True)
         return SAT;

      if (res == l_False)
         return UNSAT;
   }

   return UNKNOWN;
}

void
MiniSat::addClause(ClauseExchange * clause)
{
   clausesToAdd.addClause(clause);

   setSolverInterrupt();
}

void
MiniSat::addLearnedClause(ClauseExchange * clause)
{
   if (clause->size == 1) {
      clausesToAdd.addClause(clause);
   } else {
      clausesToImport.addClause(clause);
   }

   if (clausesToImport.size() > CLS_COUNT_INTERRUPT_LIMIT) {
      setSolverInterrupt();
   }
}

void
MiniSat::addClauses(const vector<ClauseExchange *> & clauses)
{
   clausesToAdd.addClauses(clauses);

   setSolverInterrupt();
}

void
MiniSat::addInitialClauses(const vector<ClauseExchange *> & clauses)
{
   for (size_t ind = 0; ind < clauses.size(); ind++) {
      vec<Lit> mcls;

      for (size_t i = 0; i < clauses[ind]->size; i++) {
         int lit = clauses[ind]->lits[i];
         int var = abs(lit);

         while (solver->nVars() < var) {
            solver->newVar();
         }

         mcls.push(MINI_LIT(lit));
      }

      if (solver->addClause(mcls) == false) {
         printf("unsat when adding initial cls\n");
      }
   }
}

void
MiniSat::addLearnedClauses(const vector<ClauseExchange *> & clauses)
{
   for (size_t i = 0; i < clauses.size(); i++) {
      if (clauses[i]->size == 1) {
         clausesToAdd.addClause(clauses[i]);
      } else {
         clausesToImport.addClause(clauses[i]);
      }
   }

   if (clausesToImport.size() > CLS_COUNT_INTERRUPT_LIMIT ||
       clausesToAdd.size() > 0) {
      solver->interrupt();
   }
}

void
MiniSat::getLearnedClauses(vector<ClauseExchange *> & clauses)
{
   clausesToExport.getClauses(clauses);
}

void
MiniSat::increaseClauseProduction()
{
   sizeLimit++;
}

void
MiniSat::decreaseClauseProduction()
{
   if (sizeLimit > 2) {
      sizeLimit--;
   }
}

SolvingStatistics
MiniSat::getStatistics()
{
   SolvingStatistics stats;

   stats.conflicts    = solver->conflicts;
   stats.propagations = solver->propagations;
   stats.restarts     = solver->starts;
   stats.decisions    = solver->decisions;
   stats.memPeak      = memUsedPeak();

   return stats;
}

std::vector<int>
MiniSat::getModel()
{
   std::vector<int> model;

   for (int i = 0; i < solver->nVars(); i++) {
      if (solver->model[i] != l_Undef) {
         int lit = solver->model[i] == l_True ? i + 1 : -(i + 1);
         model.push_back(lit);
      }
   }

   return model;
}
