#include "glhshapes.h"
#include "glhlib_2_1_win/source/TCylinder.h"

#include <stack>

using namespace std;



//INTERNAL VARIABLES, CLASSES, STRUCTS
//sint	ErrorState;			//Record error if one occurs. I will no longer use this. I will instead use a stack with max depth of 100
sint			TotalErrorStateStack;
stack<sint>	ErrorStateStack;


sint CheckForParameterErrors_Geometry(tbool check1, sreal width, tbool check2, sreal height, tbool check3, sreal length,
    GLH_INDEXFORMAT indexFormat)
{
    if(check1)
    {
        if(width<=0.0)
        {
            if(TotalErrorStateStack<100)
            {
                ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
                TotalErrorStateStack++;
            }
            return -1;
        }
    }
    if(check2)
    {
        if(height<=0.0)
        {
            if(TotalErrorStateStack<100)
            {
                ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
                TotalErrorStateStack++;
            }
            return -1;
        }
    }
    if(check3)
    {
        if(length<=0.0)
        {
            if(TotalErrorStateStack<100)
            {
                ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
                TotalErrorStateStack++;
            }
            return -1;
        }
    }
    if((indexFormat!=GLH_INDEXFORMAT_16BIT)&&(indexFormat!=GLH_INDEXFORMAT_32BIT))
    {
        if(TotalErrorStateStack<100)
        {
            ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
            TotalErrorStateStack++;
        }
        return -1;
    }

    return 1;
}

void DeleteCommonObject(glhCommonObjectf2 *common)
{
    if(common->pIndex16Bit)
    {
        delete [] common->pIndex16Bit;
        common->pIndex16Bit=NULL;
    }
    if(common->pIndex32Bit)
    {
        delete [] common->pIndex32Bit;
        common->pIndex32Bit=NULL;
    }
    if(common->pVertex)
    {
        delete [] common->pVertex;
        common->pVertex=NULL;
    }
}

sint glhCreateCylinderf2(glhCylinderObjectf2 *pcylinder)
{
    sint i, returnVal;

    if(pcylinder==NULL)
    {
        if(TotalErrorStateStack<100)
        {
            ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
            TotalErrorStateStack++;
        }
        return -1;
    }

    returnVal=CheckForParameterErrors_Geometry(TRUE, pcylinder->RadiusA, TRUE, pcylinder->RadiusB, TRUE, pcylinder->Height,
        pcylinder->IndexFormat);
    if(returnVal==-1)
    {
        return -1;
    }

    //Style must be 0 or 1, if texcoords are wanted
    for(i=0; i<NUMBEROFTEXTUREUNITS; i++)
    {
        if((pcylinder->TexCoordStyle[i]!=0)&&(pcylinder->TexCoordStyle[i]!=1))
        {
            if(TotalErrorStateStack<100)
            {
                ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
                TotalErrorStateStack++;
            }
            return -1;
        }
    }

    if(pcylinder->Stacks<=0)
    {
        if(TotalErrorStateStack<100)
        {
            ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
            TotalErrorStateStack++;
        }
        return -1;
    }
    if(pcylinder->Slices<=2)
    {
        if(TotalErrorStateStack<100)
        {
            ErrorStateStack.push(GLH_ERROR_INVALID_PARAM);
            TotalErrorStateStack++;
        }
        return -1;
    }

    TCylinder cylinder;

    returnVal=cylinder.StartupByTwoRadiusStacksAndSlices2(pcylinder->IndexFormat, pcylinder->VertexFormat,
        (pcylinder->IsThereATop) ? TRUE : FALSE, (pcylinder->IsThereABottom) ? TRUE : FALSE,
        pcylinder->RadiusA, pcylinder->RadiusB, pcylinder->Height, pcylinder->Stacks, pcylinder->Slices,
        pcylinder->TexCoordStyle[0], pcylinder->ScaleFactorS[0], pcylinder->ScaleFactorT[0]);
    if(returnVal==-1)
    {
        if(TotalErrorStateStack<100)
        {
            ErrorStateStack.push(GLH_ERROR_OUT_OF_MEMORY);
            TotalErrorStateStack++;
        }
        cylinder.DeallocateAllMemory();
        return -1;
    }


    //Write stuff to pcylinder
    pcylinder->TotalIndex=cylinder.UsedIndex;
    pcylinder->pIndex16Bit=cylinder.pIndex16Bit;
    cylinder.pIndex16Bit=NULL;
    pcylinder->pIndex32Bit=cylinder.pIndex32Bit;
    cylinder.pIndex32Bit=NULL;
    pcylinder->VertexCount=cylinder.VertexCount;

    switch(cylinder.VertexFormat)
    {
    case GLH_VERTEXFORMAT_V:
        pcylinder->pVertex=(sreal *)cylinder.pVertex_V;
        cylinder.pVertex_V=NULL;
        break;
    case GLH_VERTEXFORMAT_VT:
        pcylinder->pVertex=(sreal *)cylinder.pVertex_VT;
        cylinder.pVertex_VT=NULL;
        break;
    case GLH_VERTEXFORMAT_VNT:
        pcylinder->pVertex=(sreal *)cylinder.pVertex_VNT;
        cylinder.pVertex_VNT=NULL;
        break;
    case GLH_VERTEXFORMAT_VNT3:
        pcylinder->pVertex=(sreal *)cylinder.pVertex_VNT3;
        cylinder.pVertex_VNT3=NULL;
        break;
    case GLH_VERTEXFORMAT_VNTT3T3:
        pcylinder->pVertex=(sreal *)cylinder.pVertex_VNTT3T3;
        cylinder.pVertex_VNTT3T3=NULL;
        break;
    case GLH_VERTEXFORMAT_VNTT3:
        pcylinder->pVertex=(sreal *)cylinder.pVertex_VNTT3;
        cylinder.pVertex_VNTT3=NULL;
        break;
    }


    pcylinder->Stacks=cylinder.Stacks;
    pcylinder->Slices=cylinder.Slices;

    //Copy the rest of it in a few shots
    pcylinder->Start_DrawRangeElements=cylinder.Start_DrawRangeElements;
    pcylinder->End_DrawRangeElements=cylinder.End_DrawRangeElements;

    pcylinder->Start_DrawRangeElements_BodyOfCylinder=cylinder.Start_DrawRangeElements_BodyOfCylinder;
    pcylinder->End_DrawRangeElements_BodyOfCylinder=cylinder.End_DrawRangeElements_BodyOfCylinder;
    pcylinder->Start_DrawRangeElements_TopOfCylinder=cylinder.Start_DrawRangeElements_TopOfCylinder;
    pcylinder->End_DrawRangeElements_TopOfCylinder=cylinder.End_DrawRangeElements_TopOfCylinder;
    pcylinder->Start_DrawRangeElements_BottomOfCylinder=cylinder.Start_DrawRangeElements_BottomOfCylinder;
    pcylinder->End_DrawRangeElements_BottomOfCylinder=cylinder.End_DrawRangeElements_BottomOfCylinder;

    pcylinder->StartIndexIn_pIndex_BodyOfCylinder=cylinder.StartIndexIn_pIndex_BodyOfCylinder;
    pcylinder->EndIndexIn_pIndex_BodyOfCylinder=cylinder.EndIndexIn_pIndex_BodyOfCylinder;
    pcylinder->StartIndexIn_pIndex_TopOfCylinder=cylinder.StartIndexIn_pIndex_TopOfCylinder;
    pcylinder->EndIndexIn_pIndex_TopOfCylinder=cylinder.EndIndexIn_pIndex_TopOfCylinder;
    pcylinder->StartIndexIn_pIndex_BottomOfCylinder=cylinder.StartIndexIn_pIndex_BottomOfCylinder;
    pcylinder->EndIndexIn_pIndex_BottomOfCylinder=cylinder.EndIndexIn_pIndex_BottomOfCylinder;

    pcylinder->TotalIndex_BodyOfCylinder=cylinder.TotalIndex_BodyOfCylinder;
    pcylinder->TotalIndex_TopOfCylinder=cylinder.TotalIndex_TopOfCylinder;
    pcylinder->TotalIndex_BottomOfCylinder=cylinder.TotalIndex_BottomOfCylinder;

    pcylinder->pIndex16Bit_BodyOfCylinder=cylinder.pIndex16Bit_BodyOfCylinder;
    pcylinder->pIndex16Bit_TopOfCylinder=cylinder.pIndex16Bit_TopOfCylinder;
    pcylinder->pIndex16Bit_BottomOfCylinder=cylinder.pIndex16Bit_BottomOfCylinder;

    pcylinder->pIndex32Bit_BodyOfCylinder=cylinder.pIndex32Bit_BodyOfCylinder;
    pcylinder->pIndex32Bit_TopOfCylinder=cylinder.pIndex32Bit_TopOfCylinder;
    pcylinder->pIndex32Bit_BottomOfCylinder=cylinder.pIndex32Bit_BottomOfCylinder;

    pcylinder->StartIndexIn_pVertex_BodyOfCylinder=cylinder.StartIndexIn_pVertex_BodyOfCylinder;
    pcylinder->EndIndexIn_pVertex_BodyOfCylinder=cylinder.EndIndexIn_pVertex_BodyOfCylinder;
    pcylinder->StartIndexIn_pVertex_TopOfCylinder=cylinder.StartIndexIn_pVertex_TopOfCylinder;
    pcylinder->EndIndexIn_pVertex_TopOfCylinder=cylinder.EndIndexIn_pVertex_TopOfCylinder;
    pcylinder->StartIndexIn_pVertex_BottomOfCylinder=cylinder.StartIndexIn_pVertex_BottomOfCylinder;
    pcylinder->EndIndexIn_pVertex_BottomOfCylinder=cylinder.EndIndexIn_pVertex_BottomOfCylinder;

    pcylinder->TotalVertex_BodyOfCylinder=cylinder.TotalVertex_BodyOfCylinder;
    pcylinder->TotalVertex_TopOfCylinder=cylinder.TotalVertex_TopOfCylinder;
    pcylinder->TotalVertex_BottomOfCylinder=cylinder.TotalVertex_BottomOfCylinder;

    pcylinder->pVertex_BodyOfCylinder=cylinder.pVertex_BodyOfCylinder;
    pcylinder->pVertex_TopOfCylinder=cylinder.pVertex_TopOfCylinder;
    pcylinder->pVertex_BottomOfCylinder=cylinder.pVertex_BottomOfCylinder;

    cylinder.DeallocateAllMemory();
    return 1;
}

sint glhDeleteCylinderf2(glhCylinderObjectf2 *pcylinder)
{
    glhCommonObjectf2 *common=reinterpret_cast<glhCommonObjectf2 *>(pcylinder);
    DeleteCommonObject(common);
    return 1;
}
