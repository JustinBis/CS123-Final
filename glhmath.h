#ifndef GLHMATH_H
#define GLHMATH_H

#include "glhlib_2_1_win/source/glhlib.h"
#include <math.h>

// From memory management
template<class T>
sint AllocateExact_1(T *(&pointerToArray), sint *totalArraySize, sint amount)
{
    sint i;
    if(amount<=0)
        return 2;										//Misuse

    T *temporaryPointer=new T[*totalArraySize+amount];
    if(temporaryPointer==0)
        return 0;
    for(i=0; i<*totalArraySize; i++)
        temporaryPointer[i]=pointerToArray[i];
    if(pointerToArray)
        delete [] pointerToArray;
    pointerToArray=temporaryPointer;
    *totalArraySize+=amount;
    return 1;
}

inline sreal ComputeAngleBetweenTwoVectorFLOAT_2x(const sreal *pvector1, const sreal *pvector2)
{
    return acosf((pvector1[0]*pvector2[0]+pvector1[1]*pvector2[1]+pvector1[2]*pvector2[2])/(sqrtf(pvector1[0]*pvector1[0]+pvector1[1]*pvector1[1]+pvector1[2]*pvector1[2])*sqrtf(pvector2[0]*pvector2[0]+pvector2[1]*pvector2[1]+pvector2[2]*pvector2[2])));
}

inline void ComputeNormalOfPlaneFLOAT_2(sreal *normal, const sreal *pvector1, const sreal *pvector2)
{
    normal[0]=(pvector1[1]*pvector2[2])-(pvector1[2]*pvector2[1]);
    normal[1]=(pvector1[2]*pvector2[0])-(pvector1[0]*pvector2[2]);
    normal[2]=(pvector1[0]*pvector2[1])-(pvector1[1]*pvector2[0]);
}

inline void ComputeNormalOfPlaneFLOAT_3(sreal *normal, const sreal *terminal1, const sreal *terminal2, const sreal *tail)
{
    normal[0]=((terminal1[1]-tail[1])*(terminal2[2]-tail[2]))-((terminal1[2]-tail[2])*(terminal2[1]-tail[1]));
    normal[1]=((terminal1[2]-tail[2])*(terminal2[0]-tail[0]))-((terminal1[0]-tail[0])*(terminal2[2]-tail[2]));
    normal[2]=((terminal1[0]-tail[0])*(terminal2[1]-tail[1]))-((terminal1[1]-tail[1])*(terminal2[0]-tail[0]));
}

inline void NormalizeVectorFLOAT_2(sreal *pvector)
{
    sreal normalizingConstant;
    normalizingConstant=1.0/sqrtf(pvector[0]*pvector[0]+pvector[1]*pvector[1]+pvector[2]*pvector[2]);
    pvector[0]*=normalizingConstant;
    pvector[1]*=normalizingConstant;
    pvector[2]*=normalizingConstant;
}

inline void ComputeOrthonormalBasis_v2_FLOAT(sreal *tangent, sreal *binormal, sreal *normal, const sreal *v0, const sreal *v1, const sreal *v2,
                                const sreal *t0, const sreal *t1, const sreal *t2, const sreal *faceNormal)
{
    sreal cp[3];
    sreal e0[3], e1[3];
    e0[0]=v1[0]-v0[0];		//x-x
    e0[1]=t1[0]-t0[0];		//s-s
    e0[2]=t1[1]-t0[1];		//t-t
    e1[0]=v2[0]-v0[0];		//x-x
    e1[1]=t2[0]-t0[0];		//s-s
    e1[2]=t2[1]-t0[1];		//t-t

    //Compute the cross product cp = e0 CROSS e1
    cp[0]=(e0[1]*e1[2])-(e0[2]*e1[1]);
    cp[1]=(e0[2]*e1[0])-(e0[0]*e1[2]);
    cp[2]=(e0[0]*e1[1])-(e0[1]*e1[0]);
    //The cross product of the two edge vectors yields
    //a normal vector to the plane in which they lie.
    //This vector defines a plane equation.
    //Ax + Bu + Cv + D = 0
    //Assume Cv + D = 0
    // x = -Bu/A
    //
    //Assume Bu + D = 0
    //dudx = -B/A (basis[0])
    //dvdy = -C/A (basis[3])

    if(fabsf(cp[0])>1.0e-6)
    {
        tangent[0]=-cp[1]/cp[0];
        binormal[0]=-cp[2]/cp[0];
    }

    e0[0]=v1[1]-v0[1];
    e1[0]=v2[1]-v0[1];

    //Compute the cross product cp = e0 CROSS e1
    //cp[0]=(e0[1]*e1[2])-(e0[2]*e1[1]);	//Will not change
    cp[1]=(e0[2]*e1[0])-(e0[0]*e1[2]);
    cp[2]=(e0[0]*e1[1])-(e0[1]*e1[0]);

    if(fabsf(cp[0])>1.0e-6)
    {
        tangent[1]=-cp[1]/cp[0];
        binormal[1]=-cp[2]/cp[0];
    }

    e0[0]=v1[2]-v0[2];
    e1[0]=v2[2]-v0[2];

    //Compute the cross product cp = e0 CROSS e1
    //cp[0]=(e0[1]*e1[2])-(e0[2]*e1[1]);	//Will not change
    cp[1]=(e0[2]*e1[0])-(e0[0]*e1[2]);
    cp[2]=(e0[0]*e1[1])-(e0[1]*e1[0]);

    if(fabsf(cp[0])>1.0e-6)
    {
        tangent[2]=-cp[1]/cp[0];
        binormal[2]=-cp[2]/cp[0];
    }

    //Tangent... in first column of the matrix
    sreal oonorm=1.0/sqrtf(tangent[0]*tangent[0]+tangent[1]*tangent[1]+tangent[2]*tangent[2]);
    tangent[0]*=oonorm;
    tangent[1]*=oonorm;
    tangent[2]*=oonorm;

    //Binormal... in the second column of the matrix
    oonorm=1.0/sqrtf(binormal[0]*binormal[0]+binormal[1]*binormal[1]+binormal[2]*binormal[2]);
    binormal[0]*=oonorm;
    binormal[1]*=oonorm;
    binormal[2]*=oonorm;

    //Normal... in the third column of the matrix
    //Compute the cross product TxB
    normal[0]=tangent[1]*binormal[2]-tangent[2]*binormal[1];
    normal[1]=tangent[2]*binormal[0]-tangent[0]*binormal[2];
    normal[2]=tangent[0]*binormal[1]-tangent[1]*binormal[0];

    oonorm=1.0/sqrtf(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
    normal[0]*=oonorm;			//I had a bug here, it was normal[6]
    normal[1]*=oonorm;			//I had a bug here, it was normal[7]
    normal[2]*=oonorm;			//I had a bug here, it was normal[8]

    //Gram-Schmidt orthogonalization process for B
    //Compute the cross product B=NxT to obtain an orthogonal basis
    //(Without this it's possible that complete orthogonality is not acheived)
    binormal[0]=normal[1]*tangent[2]-normal[2]*tangent[1];
    binormal[1]=normal[2]*tangent[0]-normal[0]*tangent[2];
    binormal[2]=normal[0]*tangent[1]-normal[1]*tangent[0];

    if(normal[0]*faceNormal[0]+normal[1]*faceNormal[1]+normal[2]*faceNormal[2]<0.0)
    {
        normal[0]=-normal[0];
        normal[1]=-normal[1];
        normal[2]=-normal[2];
    }
}


#endif // GLHMATH_H
