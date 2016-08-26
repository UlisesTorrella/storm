/**
  @file 

  @ingroup cudd

  @brief Quantification functions for ADDs.

  @author Fabio Somenzi

  @copyright@parblock
  Copyright (c) 1995-2015, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
  @endparblock

*/

#include "util.h"
#include "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

/** \cond */

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int addCheckPositiveCube (DdManager *manager, DdNode *cube);

/** \endcond */


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**
  @brief Existentially Abstracts all the variables in cube from f.

  @details Abstracts all the variables in cube from f by summing
  over all possible values taken by the variables.

  @return the abstracted %ADD.

  @sideeffect None

  @see Cudd_addUnivAbstract Cudd_bddExistAbstract
  Cudd_addOrAbstract

*/
DdNode *
Cudd_addExistAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddExistAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }

    return(res);

} /* end of Cudd_addExistAbstract */


/**
  @brief Universally Abstracts all the variables in cube from f.

  @details Abstracts all the variables in cube from f by taking
  the product over all possible values taken by the variable.

  @return the abstracted %ADD if successful; NULL otherwise.

  @sideeffect None

  @see Cudd_addExistAbstract Cudd_bddUnivAbstract
  Cudd_addOrAbstract

*/
DdNode *
Cudd_addUnivAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode		*res;

    if (addCheckPositiveCube(manager, cube) == 0) {
	(void) fprintf(manager->err,"Error:  Can only abstract cubes");
	return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddUnivAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }

    return(res);

} /* end of Cudd_addUnivAbstract */


/**
  @brief Disjunctively abstracts all the variables in cube from the
  0-1 %ADD f.

  @details Abstracts all the variables in cube from the 0-1 %ADD f
  by taking the disjunction over all possible values taken by the
  variables.

  @return the abstracted %ADD if successful; NULL otherwise.

  @sideeffect None

  @see Cudd_addUnivAbstract Cudd_addExistAbstract

*/
DdNode *
Cudd_addOrAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddOrAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    if (manager->errorCode == CUDD_TIMEOUT_EXPIRED && manager->timeoutHandler) {
        manager->timeoutHandler(manager, manager->tohArg);
    }
    return(res);

} /* end of Cudd_addOrAbstract */

/**Function********************************************************************
 
 Synopsis    [Abstracts all the variables in cube from the
 ADD f by taking the minimum.]
 
 Description [Abstracts all the variables in cube from the ADD f
 by taking the minimum over all possible values taken by the
 variables.  Returns the abstracted ADD if successful; NULL
 otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addUnivAbstract Cudd_addExistAbstract]
 
 Note: Added by Dave Parker 14/6/99
 
 ******************************************************************************/
DdNode *
Cudd_addMinAbstract(
                    DdManager * manager,
                    DdNode * f,
                    DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMinAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMinAbstract */


/**Function********************************************************************
 
 Synopsis    [Abstracts all the variables in cube from the
 ADD f by taking the minimum.]
 
 Description [Abstracts all the variables in cube from the ADD f
 by taking the minimum over all possible values taken by the
 variables.  Returns the abstracted ADD if successful; NULL
 otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addUnivAbstract Cudd_addExistAbstract]
 
 Note: Added by Christian Dehnert 24/08/2016
 
 ******************************************************************************/
DdNode *
Cudd_addMinExcept0Abstract(
                    DdManager * manager,
                    DdNode * f,
                    DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMinExcept0AbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMinExcept0Abstract */

/**Function********************************************************************
 
 Synopsis    [Abstracts all the variables in cube from the
 ADD f by taking the maximum.]
 
 Description [Abstracts all the variables in cube from the ADD f
 by taking the maximum over all possible values taken by the
 variables.  Returns the abstracted ADD if successful; NULL
 otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addUnivAbstract Cudd_addExistAbstract]
 
 Note: Added by Dave Parker 14/6/99
 
 ******************************************************************************/
DdNode *
Cudd_addMaxAbstract(
                    DdManager * manager,
                    DdNode * f,
                    DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMaxAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMaxAbstract */

/**Function********************************************************************
 
 Synopsis    [Just like Cudd_addMinAbstract, but instead of abstracting the
 variables in the given cube, picks a unique representative that realizes th
 minimal function value.]
 
 Description [Returns the resulting ADD if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addMaxAbstractRepresentative]
 
 Note: Added by Christian Dehnert 8/5/14
 
 ******************************************************************************/
DdNode *
Cudd_addMinAbstractRepresentative(
                                  DdManager * manager,
                                  DdNode * f,
                                  DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMinAbstractRepresentativeRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMinRepresentative */

/**Function********************************************************************
 
 Synopsis    [Just like Cudd_addMaxAbstract, but instead of abstracting the
 variables in the given cube, picks a unique representative that realizes th
 maximal function value.]
 
 Description [Returns the resulting ADD if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addMinAbstractRepresentative]
 
 Note: Added by Christian Dehnert 8/5/14
 
 ******************************************************************************/
DdNode *
Cudd_addMaxAbstractRepresentative(
                                  DdManager * manager,
                                  DdNode * f,
                                  DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMaxAbstractRepresentativeRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMaxRepresentative */

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**
  @brief Performs the recursive step of Cudd_addExistAbstract.

  @details Returns the %ADD obtained by abstracting the variables of
  cube from f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddExistAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;

    statLine(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point. */	
    if (f == zero || cuddIsConstant(cube)) {  
        return(f);
    }

    /* Abstract a variable that does not appear in f => multiply by 2. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddExistAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager,Cudd_addPlus,res1,res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
        return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addExistAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddExistAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddExistAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addPlus, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addExistAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddExistAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddExistAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addExistAbstract, f, cube, res);
        return(res);
    }	    

} /* end of cuddAddExistAbstractRecur */


/**
  @brief Performs the recursive step of Cudd_addUnivAbstract.

  @return the %ADD obtained by abstracting the variables of cube from
  f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddUnivAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *one, *zero;

    statLine(manager);
    one = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point.
    ** zero and one are the only constatnts c such that c*c=c.
    */
    if (f == zero || f == one || cube == one) {  
	return(f);
    }

    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddUnivAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager, Cudd_addTimes, res1, res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
	return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addUnivAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddUnivAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddUnivAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addTimes, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addUnivAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddUnivAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddUnivAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addUnivAbstract, f, cube, res);
        return(res);
    }

} /* end of cuddAddUnivAbstractRecur */


/**
  @brief Performs the recursive step of Cudd_addOrAbstract.

  @return the %ADD obtained by abstracting the variables of cube from
  f, if successful; NULL otherwise.

  @sideeffect None

*/
DdNode *
cuddAddOrAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *one;

    statLine(manager);
    one = DD_ONE(manager);

    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(f) || cube == one) {  
	return(f);
    }

    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res = cuddAddOrAbstractRecur(manager, f, cuddT(cube));
	return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addOrAbstract, f, cube)) != NULL) {
	return(res);
    }

    checkWhetherToGiveUp(manager);

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddOrAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	if (res1 != one) {
	    res2 = cuddAddOrAbstractRecur(manager, E, cuddT(cube));
	    if (res2 == NULL) {
		Cudd_RecursiveDeref(manager,res1);
		return(NULL);
	    }
	    cuddRef(res2);
	    res = cuddAddApplyRecur(manager, Cudd_addOr, res1, res2);
	    if (res == NULL) {
		Cudd_RecursiveDeref(manager,res1);
		Cudd_RecursiveDeref(manager,res2);
		return(NULL);
	    }
	    cuddRef(res);
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	} else {
	    res = res1;
	}
	cuddCacheInsert2(manager, Cudd_addOrAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddOrAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddOrAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addOrAbstract, f, cube, res);
        return(res);
    }

} /* end of cuddAddOrAbstractRecur */

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMinAbstract.]
 
 Description [Performs the recursive step of Cudd_addMinAbstract.
 Returns the ADD obtained by abstracting the variables of cube from f,
 if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMinAbstractRecur(
                        DdManager * manager,
                        DdNode * f,
                        DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;
    
    zero = DD_ZERO(manager);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (f == zero || cuddIsConstant(cube)) {
        return(f);
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMinAbstractRecur(manager, f, cuddT(cube));
       	return(res);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMinAbstract, f, cube)) != NULL) {
        return(res);
    }
    
    
    T = cuddT(f);
    E = cuddE(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMinAbstractRecur(manager, T, cuddT(cube));
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMinAbstractRecur(manager, E, cuddT(cube));
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = cuddAddApplyRecur(manager, Cudd_addMinimum, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1);
        Cudd_RecursiveDeref(manager,res2);
        cuddCacheInsert2(manager, Cudd_addMinAbstract, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMinAbstractRecur(manager, T, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMinAbstractRecur(manager, E, cube);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = (res1 == res2) ? res1 :
        cuddUniqueInter(manager, (int) f->index, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMinAbstract, f, cube, res);
        return(res);
    }
    
} /* end of cuddAddMinAbstractRecur */

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMinAbstract.]
 
 Description [Performs the recursive step of Cudd_addMinAbstract.
 Returns the ADD obtained by abstracting the variables of cube from f,
 if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 added 24/08/2016 by Christian Dehnert
 
 ******************************************************************************/
DdNode *
cuddAddMinExcept0AbstractRecur(
                        DdManager * manager,
                        DdNode * f,
                        DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;
    
    zero = DD_ZERO(manager);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (f == zero || cuddIsConstant(cube)) {
        return(f);
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMinAbstractRecur(manager, f, cuddT(cube));
       	return(res);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMinAbstract, f, cube)) != NULL) {
        return(res);
    }
    
    
    T = cuddT(f);
    E = cuddE(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMinAbstractRecur(manager, T, cuddT(cube));
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMinAbstractRecur(manager, E, cuddT(cube));
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = cuddAddApplyRecur(manager, Cudd_addMinimumExcept0, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1);
        Cudd_RecursiveDeref(manager,res2);
        cuddCacheInsert2(manager, Cudd_addMinAbstract, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMinAbstractRecur(manager, T, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMinAbstractRecur(manager, E, cube);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = (res1 == res2) ? res1 :
        cuddUniqueInter(manager, (int) f->index, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMinAbstract, f, cube, res);
        return(res);
    }
    
} /* end of cuddAddMinAbstractRecur */

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMaxAbstract.]
 
 Description [Performs the recursive step of Cudd_addMaxAbstract.
 Returns the ADD obtained by abstracting the variables of cube from f,
 if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMaxAbstractRecur(
                        DdManager * manager,
                        DdNode * f,
                        DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;
    
    zero = DD_ZERO(manager);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (f == zero || cuddIsConstant(cube)) {
        return(f);
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMaxAbstractRecur(manager, f, cuddT(cube));
       	return(res);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMaxAbstract, f, cube)) != NULL) {
        return(res);
    }
    
    
    T = cuddT(f);
    E = cuddE(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMaxAbstractRecur(manager, T, cuddT(cube));
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMaxAbstractRecur(manager, E, cuddT(cube));
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = cuddAddApplyRecur(manager, Cudd_addMaximum, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1);
        Cudd_RecursiveDeref(manager,res2);
        cuddCacheInsert2(manager, Cudd_addMaxAbstract, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMaxAbstractRecur(manager, T, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMaxAbstractRecur(manager, E, cube);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = (res1 == res2) ? res1 :
        cuddUniqueInter(manager, (int) f->index, res1, res2);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMaxAbstract, f, cube, res);
        return(res);
    }	    
    
} /* end of cuddAddMaxAbstractRecur */

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMinAbstractRepresentative.]
 
 Description [Performs the recursive step of Cudd_addMinAbstractRepresentative.
 Returns the ADD obtained by picking a representative over the variables in
 the given cube for all other valuations. Returns the resulting ADD if successful;
 NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMinAbstractRepresentativeRecur(
                                      DdManager * manager,
                                      DdNode * f,
                                      DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero, *one, *logicalZero, *res1Inf, *res2Inf, *left, *right, *tmp, *tmp2;
    
    zero = DD_ZERO(manager);
    one = DD_ONE(manager);
    logicalZero = Cudd_Not(one);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(cube)) {
        return one;
    }
    if (cuddIsConstant(f)) {
        res = cuddAddMinAbstractRepresentativeRecur(manager, f, cuddT(cube));
        if (res == NULL) {
            return(NULL);
        }
        cuddRef(res);
            
        // We build in the negation ourselves.
        res1 = cuddUniqueInter(manager, (int) cube->index, one, Cudd_Not(res));
        if (res1 == NULL) {
            Cudd_IterDerefBdd(manager,res);
            return(NULL);
        }
        res1 = Cudd_Not(res1);
        cuddDeref(res);
        return(res1);
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMinAbstractRepresentativeRecur(manager, f, cuddT(cube));
        if (res == NULL) {
            return(NULL);
        }
        
        // Fill in the missing variables to make representative unique.
        cuddRef(res);
        // We build in the negation ourselves.
        res1 = cuddUniqueInter(manager, (int) cube->index, one, Cudd_Not(res));
        if (res1 == NULL) {
            Cudd_IterDerefBdd(manager,res);
            return(NULL);
        }
        res1 = Cudd_Not(res1);
        cuddDeref(res);
       	return(res1);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMinAbstractRepresentative, f, cube)) != NULL) {
        return(res);
    }
    
    
    E = cuddE(f);
    T = cuddT(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMinAbstractRepresentativeRecur(manager, E, cuddT(cube));
        if (res1 == NULL) {
            return(NULL);
        }
        cuddRef(res1);
        
        res2 = cuddAddMinAbstractRepresentativeRecur(manager, T, cuddT(cube));
        if (res2 == NULL) {
            Cudd_IterDerefBdd(manager, res1);
            return(NULL);
        }
        cuddRef(res2);
        
        left = cuddAddMinAbstractRecur(manager, E, cuddT(cube));
        if (left == NULL) {
            Cudd_IterDerefBdd(manager, res1);
            Cudd_IterDerefBdd(manager, res2);
            return(NULL);
        }
        cuddRef(left);
        right = cuddAddMinAbstractRecur(manager, T, cuddT(cube));
        if (right == NULL) {
            Cudd_IterDerefBdd(manager, res1);
            Cudd_IterDerefBdd(manager, res2);
            Cudd_RecursiveDeref(manager, left);
            return(NULL);
        }
        cuddRef(right);
        
        tmp = cuddAddToBddApplyRecur(manager, Cudd_addToBddLessThanEquals, left, right);
        if (tmp == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            Cudd_IterDerefBdd(manager,res2);
            Cudd_RecursiveDeref(manager,left);
            Cudd_RecursiveDeref(manager,right);
            return(NULL);
        }
        cuddRef(tmp);
        
        Cudd_RecursiveDeref(manager, left);
        Cudd_RecursiveDeref(manager, right);
        
        res1Inf = cuddBddIteRecur(manager, tmp, res1, logicalZero);
        if (res1Inf == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            Cudd_IterDerefBdd(manager,res2);
            Cudd_IterDerefBdd(manager,tmp);
            return(NULL);
        }
        cuddRef(res1Inf);
        Cudd_IterDerefBdd(manager,res1);
        
        res2Inf = cuddBddIteRecur(manager, Cudd_Not(tmp), res2, logicalZero);
        Cudd_IterDerefBdd(manager,tmp);
        if (res2Inf == NULL) {
            Cudd_IterDerefBdd(manager,res2);
            Cudd_IterDerefBdd(manager,res1Inf);
            return(NULL);
        }
        cuddRef(res2Inf);
        Cudd_IterDerefBdd(manager,res2);

        int compl = (res1Inf == res2Inf) ? 1 : Cudd_IsComplement(res2Inf);
        res = (res1Inf == res2Inf) ? cuddUniqueInter(manager, (int) f->index, one, Cudd_Not(res1Inf)) : cuddUniqueInter(manager, (int) f->index, Cudd_Regular(res2Inf), compl ? Cudd_Not(res1Inf) : res1Inf);
        if (res == NULL) {
            Cudd_IterDerefBdd(manager,res1Inf);
            Cudd_IterDerefBdd(manager,res2Inf);
            return(NULL);
        }
        if (compl) {
            res = Cudd_Not(res);
        }
        cuddRef(res);
        cuddDeref(res1Inf);
        cuddDeref(res2Inf);
        cuddCacheInsert2(manager, Cudd_addMinAbstractRepresentative, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMinAbstractRepresentativeRecur(manager, E, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMinAbstractRepresentativeRecur(manager, T, cube);
        if (res2 == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        
        int compl = (res1 == res2) ? 1 : Cudd_IsComplement(res2);
        res = (res1 == res2) ? cuddUniqueInter(manager, (int) f->index, one, Cudd_Not(res1)) : cuddUniqueInter(manager, (int) f->index, Cudd_Regular(res2), Cudd_Not(res1));
        if (res == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            Cudd_IterDerefBdd(manager,res2);
            return(NULL);
        }
        if (compl) {
            res = Cudd_Not(res);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMinAbstractRepresentative, f, cube, res);
        return(res);
    }
    
} /* end of cuddAddMinAbstractRepresentativeRecur */

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMaxAbstractRepresentative.]
 
 Description [Performs the recursive step of Cudd_addMaxAbstractRepresentative.
 Returns the ADD obtained by picking a representative over the variables in
 the given cube for all other valuations. Returns the resulting ADD if successful;
 NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMaxAbstractRepresentativeRecur(
                                      DdManager * manager,
                                      DdNode * f,
                                      DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero, *one, *logicalZero, *res1Inf, *res2Inf, *left, *right, *tmp, *tmp2;
    
    zero = DD_ZERO(manager);
    one = DD_ONE(manager);
    logicalZero = Cudd_Not(one);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(cube)) {
        return one;
    }
    if (cuddIsConstant(f)) {
        res = cuddAddMaxAbstractRepresentativeRecur(manager, f, cuddT(cube));
        if (res == NULL) {
            return(NULL);
        }
        cuddRef(res);
        
        // We build in the negation ourselves.
        res1 = cuddUniqueInter(manager, (int) cube->index, one, Cudd_Not(res));
        if (res1 == NULL) {
            Cudd_IterDerefBdd(manager,res);
            return(NULL);
        }
        res1 = Cudd_Not(res1);
        cuddDeref(res);
        return(res1);
            
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMaxAbstractRepresentativeRecur(manager, f, cuddT(cube));
        
        if (res == NULL) {
            return(NULL);
        }
        
        // Fill in the missing variables to make representative unique.
        cuddRef(res);
        res1 = cuddUniqueInter(manager, (int) cube->index, one, Cudd_Not(res));
        if (res1 == NULL) {
            Cudd_IterDerefBdd(manager, res);
            return(NULL);
        }
        res1 = Cudd_Not(res1);
        Cudd_IterDerefBdd(manager,res);
       	return(res1);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMaxAbstractRepresentative, f, cube)) != NULL) {
        return(res);
    }
    
    
    E = cuddE(f);
    T = cuddT(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMaxAbstractRepresentativeRecur(manager, E, cuddT(cube));
        if (res1 == NULL) {
            return(NULL);
        }
        cuddRef(res1);
        
        res2 = cuddAddMaxAbstractRepresentativeRecur(manager, T, cuddT(cube));
        if (res2 == NULL) {
            Cudd_IterDerefBdd(manager, res1);
            return(NULL);
        }
        cuddRef(res2);
        
        left = cuddAddMaxAbstractRecur(manager, E, cuddT(cube));
        if (left == NULL) {
            Cudd_IterDerefBdd(manager, res1);
            Cudd_IterDerefBdd(manager, res2);
            return(NULL);
        }
        cuddRef(left);
        right = cuddAddMaxAbstractRecur(manager, T, cuddT(cube));
        if (right == NULL) {
            Cudd_IterDerefBdd(manager, res1);
            Cudd_IterDerefBdd(manager, res2);
            Cudd_RecursiveDeref(manager, left);
            return(NULL);
        }
        cuddRef(right);
        
        tmp = cuddAddToBddApplyRecur(manager, Cudd_addToBddGreaterThanEquals, left, right);
        if (tmp == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            Cudd_IterDerefBdd(manager,res2);
            Cudd_RecursiveDeref(manager,left);
            Cudd_RecursiveDeref(manager,right);
            return(NULL);
        }
        cuddRef(tmp);
        
        Cudd_RecursiveDeref(manager, left);
        Cudd_RecursiveDeref(manager, right);
        
        cuddRef(zero);
        res1Inf = cuddBddIteRecur(manager, tmp, res1, logicalZero);
        if (res1Inf == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            Cudd_IterDerefBdd(manager,res2);
            Cudd_IterDerefBdd(manager,tmp);
            cuddDeref(zero);
            return(NULL);
        }
        cuddRef(res1Inf);
        Cudd_IterDerefBdd(manager,res1);
        
        cuddRef(zero);
        res2Inf = cuddBddIteRecur(manager, Cudd_Not(tmp), res2, logicalZero);
        if (res2Inf == NULL) {
            Cudd_IterDerefBdd(manager,res2);
            Cudd_IterDerefBdd(manager,res1Inf);
            Cudd_IterDerefBdd(manager,tmp);
            return(NULL);
        }
        cuddRef(res2Inf);
        Cudd_IterDerefBdd(manager,res2);
        Cudd_IterDerefBdd(manager,tmp);
        
        int compl = (res1Inf == res2Inf) ? 1 : Cudd_IsComplement(res2Inf);
        res = (res1Inf == res2Inf) ? cuddUniqueInter(manager, (int) f->index, one, Cudd_Not(res1Inf)) : cuddUniqueInter(manager, (int) f->index, Cudd_Regular(res2Inf), compl ? Cudd_Not(res1Inf) : res1Inf);
        if (res == NULL) {
            Cudd_IterDerefBdd(manager,res1Inf);
            Cudd_IterDerefBdd(manager,res2Inf);
            return(NULL);
        }
        if (compl) {
            res = Cudd_Not(res);
        }
        cuddRef(res);
        Cudd_IterDerefBdd(manager,res1Inf);
        Cudd_IterDerefBdd(manager,res2Inf);
        cuddCacheInsert2(manager, Cudd_addMaxAbstractRepresentative, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMaxAbstractRepresentativeRecur(manager, E, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMaxAbstractRepresentativeRecur(manager, T, cube);
        if (res2 == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        
        int compl = (res1 == res2) ? 1 : Cudd_IsComplement(res2);
        res = (res1 == res2) ? cuddUniqueInter(manager, (int) f->index, one, Cudd_Not(res1)) : cuddUniqueInter(manager, (int) f->index, Cudd_Regular(res2), compl ? Cudd_Not(res1) : res1);
        if (res == NULL) {
            Cudd_IterDerefBdd(manager,res1);
            Cudd_IterDerefBdd(manager,res2);
            return(NULL);
        }
        if (compl) {
            res = Cudd_Not(res);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMaxAbstractRepresentative, f, cube, res);
        return(res);
    }
} /* end of cuddAddMaxAbstractRepresentativeRecur */

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**
  @brief Checks whether cube is an %ADD representing the product
  of positive literals.

  @return 1 in case of success; 0 otherwise.

  @sideeffect None

*/
static int
addCheckPositiveCube(
  DdManager * manager,
  DdNode * cube)
{
    if (Cudd_IsComplement(cube)) return(0);
    if (cube == DD_ONE(manager)) return(1);
    if (cuddIsConstant(cube)) return(0);
    if (cuddE(cube) == DD_ZERO(manager)) {
        return(addCheckPositiveCube(manager, cuddT(cube)));
    }
    return(0);

} /* end of addCheckPositiveCube */
