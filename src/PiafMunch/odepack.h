/*
* PiafMunch (v.2) -- Implementing and Solving the Munch model of phloem sap flow in higher plants
*
* Copyright (C) 2004-2019 INRA
*
* Author: A. Lacointe, UMR PIAF, Clermont-Ferrand, France
*
* File: odepack.h
*
* This file is part of PiafMunch. PiafMunch is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License version 3.0 as published by
* the Free Software Foundation and appearing in the file LICENSE.GPL included in the
* packaging of this file. Please  review the following information to ensure the GNU
* General Public License version 3.0  requirements will be met:
* http://www.gnu.org/copyleft/gpl.html.
*
* PiafMunch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with PiafMunch.
* If not, see <http://www.gnu.org/licenses/>.
*
-----------------------------------------------------------------------------------------------------------------------------------*/

#ifndef NSOLVE_H
#define NLSOLVE_H

#include <stdio.h>
#include "PiafMunch/PM_arrays.h"
#include <memory>

#include <sundials/sundials_types.h>    /* defs. of realtype, sunindextype      */
#include <sundials/sundials_math.h>
#include <sundials/sundials_pcg.h>
#include <sundials/sundials_sparse.h>
#include <sundials/sundials_spbcgs.h>
#include <sundials/sundials_spfgmr.h>
#include <sundials/sundials_spgmr.h>
#include <sundials/sundials_sptfqmr.h>
#include <nvector/nvector_serial.h>     /* access to serial N_Vector            */
#include <sunmatrix/sunmatrix_sparse.h> /* access to sparse SUNMatrix           */
#include <sunlinsol/sunlinsol_klu.h>    /* access to KLU sparse direct solver   */
#include <sunlinsol/sunlinsol_band.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunlinsol/sunlinsol_pcg.h>
#include <sunlinsol/sunlinsol_spbcgs.h>
#include <sunlinsol/sunlinsol_spfgmr.h>
#include <sunlinsol/sunlinsol_spgmr.h>
#include <sunlinsol/sunlinsol_sptfqmr.h>
#include <sunnonlinsol/sunnonlinsol_newton.h>

#include <cvode/cvode.h>                /* prototypes for CVODE fcts., consts.  */
#include <cvode/cvode_bandpre.h>
#include <cvode/cvode_diag.h>
#include <cvode/cvode_direct.h>
#include <cvode/cvode_spils.h>
#include <cvode/cvode_impl.h>

#include <arkode/arkode.h>                /* prototypes for arkODE fcts., consts.  */
#include <arkode/arkode_erkstep.h>
#include <arkode/arkode_butcher.h>

#define DENSE 0
#define BAND  1
#define DIAG  2
#define SPFGMR 3
#define SPGMR 4
#define SPBCGS 5
#define PCG 6
#define SPTFQMR 7
#define KLU 8

//void(*aux)(double,double*, shared_ptr<PhloemFlux>),
												  
int cvode_direct(void(*f)(double,double*,double*), Fortran_vector& y, Fortran_vector &T, void(*aux)(double,double*), Fortran_vector& atol, Fortran_vector& rtol,
			  int solver = DENSE, int nbVar_dot = 0, Fortran_vector** Var_primitive = NULL, Fortran_vector** Var_dot = NULL, bool verbose = true, bool STALD = true,
			  void(*rootfind)(double, double*, double*) = NULL, int nrootfns = 0, int mu = 1, int ml = 1);

int cvode_spils(void(*f)(double,double*,double*), Fortran_vector &y, Fortran_vector &T, void(*aux)(double,double*), Fortran_vector& atol, Fortran_vector& rtol,
			int solver = SPGMR, int GSType = MODIFIED_GS, int prectype = PREC_NONE, int nbVar_dot = 0, Fortran_vector** Var_primitive = NULL, Fortran_vector** Var_dot = NULL,
			bool verbose = true, bool STALD = true, void(*rootfind)(double,double*,double*) = NULL, int nrootfns = 0, int mu = 1, int ml = 1, int maxl = 5);

int arkode(void(*f)(double, double*, double*), Fortran_vector& y, Fortran_vector &T, void(*aux)(double, double*), Fortran_vector& atol, Fortran_vector& rtol,
	int nbVar_dot=0, Fortran_vector** Var_primitive=NULL, Fortran_vector** Var_dot=NULL, bool verbose=true, void(*rootfind)(double, double*, double*)=NULL, int nrootfns=0);

/*
int cvode_spils(void(*f)(double,double*,double*), Fortran_vector &y, Fortran_vector &T, void(*aux)(double,double*), Fortran_vector& atol, Fortran_vector& rtol,
			int solver = SPGMR, int GSType = MODIFIED_GS, int prectype = PREC_NONE, int nbVar_dot = 0, Fortran_vector** Var_primitive = NULL, Fortran_vector** Var_dot = NULL,
			bool verbose = true, bool STALD = true, void(*rootfind)(double,double*,double*) = NULL, int nrootfns = 0, int mu = 1, int ml = 1, int maxl = 5);

Les solveurs :

 cvode_direct(f, y, T, aux, atol, rtol [, solver [, nbVar_dot [, Var_primitive [, Var_dot [, verbose [, STALD [, rootfind, nrootfns [, mu, ml]]]]]]]] )
 cvode_spils(f, y, T, aux, atol, rtol [, solver [, GSType [, prectype [, nbVar_dot [, Var_primitive [, Var_dot [, verbose [, STALD [, rootfind, nrootfns [, mu, ml [, maxl]]]]]]]]]]] )
 arkode(f, y, T, aux, atol, rtol [, nbVar_dot [, Var_primitive [, Var_dot [, verbose [, rootfind, nrootfns]]]]] )

 impliquent 6 arguments obligatoires, plus 6 � 12 facultatifs :

 arguments obligatoires :

 f(double t, double* y, double* ydot) : fonction exprimant le syst�me � r�soudre :  ydot[] = dy[]/dt  en fonction de t et de y.
 y  : Fortran_vector des variables d'�tat d�crivant le syst�me ; doit �tre initialis� �  y0 = y(t=t0)  avant l'appel du solveur.
 T  : Fortran_vector sp�cifiant les valeurs de t pour lesquelles on d�sire la solution, y compris le temps initial t0 = T[1].
 aux(double t, double* y) : d�finit une t�che auxiliaire (NULL si aucune) � ex�cuter � chacun des instants  t  de T  demand�s
		et �galement aux �ventuels instants t solutions de rootfind (cf. ci-apr�s).
 atol, rtol : (Fortran_Vector) pr�cision  absolue, relative -> erreur sur y[i] ~ ewt[i]= (rtol[i]* |y[i]|) + atol[i];
		si xtol (= atol ou rtol) est de size 1, alors pour chaque composante (i= 1 � neq), xtol[i] = xtol[1].
		Actuellement, seule la forme scalaire de rtol est g�r�e ; l'option CV_WF de cvode (p. ...) n'est pas encore g�r�e.

arguments facultatifs pour le calcul des d�riv�es de variables accessibles mais ne figurant pas dans le vecteur y des var. 'r�servoirs' v�rifiant dy/dt = f(t, y, y') :

 NbVar_dot : nb de variables (Fortran_vectors) dont on souhaite la d�riv�e � chaque instant.
 Fortran_vector** Var_primitive : tableau (de taille NbVar_dot) des adresses des NbVar_dot variables (Fortran_vectors) dont on souhaite la d�riv�e � chaque instant.
 Fortran_vector** Var_dot : tableau (de taille NbVar_dot) des adresses des NbVar_dot  d�riv�es (Fortran_vectors) dont on souhaite la valeur � chaque instant.

arguments facultatifs annexes :

 solver : (int) -- un des 2 ou 3 disponibles, suivant la fonction :	{DENSE (par d�faut), BAND, DIAG ou KLU}  pour cvode_direct  ;  {SPGMR (par d�faut), SPFGMR, SPBCGS, PCG ou SPTFQMR}  pour cvode_spils.
 GSType  : (sp�c. cvode_spils) int (ignor� si solver <> SPGMR ou SPFGMR) type d'orthogonalis. Gram-Schmidt: CLASSICAL_GS ou (par d�faut) MODIFIED_GS.
 prectype: (sp�c. cvode_spils) int : une des 4 constantes {PREC_NONE (par d�faut), PREC_LEFT, PREC_RIGHT, PREC_BOTH} : le type de pr�cond. de la mat.
 verbose: bool�en (true par d�faut) : si true =>  affiche le d�roulement (pas par pas) ;
 STALD : (sp�c. cvode_xxxx) : bool�en (true par d�faut)  : si true => active l'algorithme de d�tection/corr. d'instab. li� aux ordres >2.
 rootfind : (sp�c. cvode_xxxx) : fonction (NULL si aucune) d�finissant un syst�me de nrootfns �q. � r�soudre en cours
           d'int�gration : g[i](t, y) = 0   [i = 1..nroots]. D�cl. suivant le prototype: rootfind(double t, double* y, double* g).
 nrootfns : (sp�c. cvode_xxxx) : int : nombre d'�quations  g[i](t, y) = 0  (ci-dessus) �ventuellement � r�soudre ; ignor� si rootfind = NULL.
 mu, ml : (sp�c. cvode_xxxx) : int : pris en compte seulement si solver = BAND ou si prectype <> PREC_NONE : nb de super-diag. resp. sup�rieures et inf�rieures.
 maxl : (sp�c. cvode_spils) : int (= 5 par d�faut) : la taille max. du sous-espace de Krylov.
*/

#endif
