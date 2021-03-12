#include <PSL_OrthogonalGridUtilities.hpp>
#include "PSL_FreeHelpers.hpp"
#include <iostream>
#include <limits>
#include <cmath>

namespace PlatoSubproblemLibrary
{

std::vector<int> OrthogonalGridUtilities::computeNumElementsInEachDirection(const Vector& aMaxUVWCoords, const Vector& aMinUVWCoords, const double& aTargetEdgeLength) const
{
    double tULength = aMaxUVWCoords(0) - aMinUVWCoords(0);
    double tVLength = aMaxUVWCoords(1) - aMinUVWCoords(1);
    double tWLength = aMaxUVWCoords(2) - aMinUVWCoords(2);

    if(aTargetEdgeLength <= 0.0)
        throw(std::domain_error("OrthogonalGridUtilities: target edge length must be greater than zero"));

    std::vector<int> tNumElements;

    tNumElements.push_back(aTargetEdgeLength > tULength ? 1 : (int) tULength/aTargetEdgeLength);
    tNumElements.push_back(aTargetEdgeLength > tVLength ? 1 : (int) tVLength/aTargetEdgeLength);
    tNumElements.push_back(aTargetEdgeLength > tWLength ? 1 : (int) tWLength/aTargetEdgeLength);

    return tNumElements;
}

int OrthogonalGridUtilities::getSerializedIndex(const int& i, const int& j, const int& k) const
{
    if(i < 0 || j < 0 || k < 0 || i > mNumElementsInEachDirection[0] || j > mNumElementsInEachDirection[1] || k > mNumElementsInEachDirection[2])
    {
        throw(std::out_of_range("OrthogonalGridUtilities: Index in each direction must be between zero and number of grid points"));
    }

    return i + j*(mNumElementsInEachDirection[0]+1) + k*(mNumElementsInEachDirection[0]+1)*(mNumElementsInEachDirection[1]+1);
}

int OrthogonalGridUtilities::getSerializedIndex(const std::vector<int>& aIndex) const
{
    if(aIndex.size() != 3)
    {
        throw(std::domain_error("OrthogonalGridUtilities: Index must have 3 entries"));
    }

    return getSerializedIndex(aIndex.at(0), aIndex.at(1), aIndex.at(2));
}    

std::vector<std::vector<int>> OrthogonalGridUtilities::getSupportIndices(const int& i, const int& j, const int& k) const
{
    auto tDimensions = getGridDimensions();
    std::vector<std::vector<int>> tIndices;

    std::vector<int> tFirstIndexSupports;
    std::vector<int> tSecondIndexSupports;
    std::vector<int> tThirdIndexSupports;

        tFirstIndexSupports.push_back(i);
        if(i > 0)
            tFirstIndexSupports.push_back(i-1);
        if(i < tDimensions[0] - 1)
            tFirstIndexSupports.push_back(i+1);

        tSecondIndexSupports.push_back(j);
        if(j > 0)
            tSecondIndexSupports.push_back(j-1);
        if(j < tDimensions[1] - 1)
            tSecondIndexSupports.push_back(j+1);

        if(k > 0)
            tThirdIndexSupports.push_back(k-1);

        for(auto tFirst : tFirstIndexSupports)
        {
            for(auto tSecond : tSecondIndexSupports)
            {
                for(auto tThird : tThirdIndexSupports)
                {
                    if(tFirst == i || tSecond == j)
                        tIndices.push_back({tFirst,tSecond,tThird});
                }
            }
        }
                
    return tIndices;
}

std::vector<std::vector<int>> OrthogonalGridUtilities::getSupportIndices(const std::vector<int>& aIndex) const
{
    if(aIndex.size() != 3)
    {
        throw(std::domain_error("OrthogonalGridUtilities: Index must have 3 entries"));
    }

    return getSupportIndices(aIndex.at(0), aIndex.at(1), aIndex.at(2));
}

void OrthogonalGridUtilities::computeGridXYZCoordinates(std::vector<Vector>& aXYZCoordinates) const
{
    double tULength = mMaxUVWCoords(0) - mMinUVWCoords(0);
    double tVLength = mMaxUVWCoords(1) - mMinUVWCoords(1);
    double tWLength = mMaxUVWCoords(2) - mMinUVWCoords(2);

    std::vector<double> tLength = {tULength,tVLength,tWLength};
    std::vector<Vector> tBasis = {mUBasisVector,mVBasisVector,mWBasisVector};

    auto tDimension = getGridDimensions();

    aXYZCoordinates.resize(tDimension[0] * tDimension[1] * tDimension[2]);

    for(int i = 0; i < tDimension[0]; ++i)
    {
        for(int j = 0; j < tDimension[1]; ++j)
        {
            for(int k = 0; k < tDimension[2]; ++k)
            {

                Vector tXYZCoordinates({0.0,0.0,0.0});
                Vector tUVWCoordinates({0.0,0.0,0.0});

                std::vector<int> tIndex = {i,j,k};

                for(int tTempIndex = 0; tTempIndex < 3; ++tTempIndex)
                {
                    double tUVWCoordinate = mMinUVWCoords(tTempIndex) + tIndex[tTempIndex]*tLength[tTempIndex]/mNumElementsInEachDirection[tTempIndex]; 
                    tUVWCoordinates.set(tTempIndex,tUVWCoordinate);
                }

                tXYZCoordinates = tUVWCoordinates(0)*mUBasisVector + tUVWCoordinates(1)*mVBasisVector + tUVWCoordinates(2)*mWBasisVector;

                aXYZCoordinates[getSerializedIndex(i,j,k)] = tXYZCoordinates;
            }
        }
    }
}

void OrthogonalGridUtilities::checkBasis(const Vector& aUBasisVector,
                                         const Vector& aVBasisVector,
                                         const Vector& aWBasisVector) const
{
    if(fabs(aUBasisVector.euclideanNorm() - 1) > 1e-12 || fabs(aVBasisVector.euclideanNorm() -1) > 1e-12 || fabs(aWBasisVector.euclideanNorm() - 1) > 1e-12)
    {
        throw(std::domain_error("OrthogonalGridUtilities: provided basis not unit length"));
    }

    if(dot_product(aUBasisVector,aVBasisVector) > 1e-12 || dot_product(aUBasisVector,aWBasisVector) > 1e-12 || dot_product(aVBasisVector,aWBasisVector) > 1e-12)
    {
        throw(std::domain_error("OrthogonalGridUtilities: provided basis is not orthogonal"));
    }

    if(dot_product(cross_product(aUBasisVector,aVBasisVector),aWBasisVector) < 0)
    {
        throw(std::domain_error("OrthogonalGridUtilities: provided basis is not positively oriented"));
    }
}

void OrthogonalGridUtilities::checkBounds(const Vector& aMaxUVWCoords,
                                          const Vector& aMinUVWCoords) const
{
    for(int i = 0; i < 3; ++i)
    {
        if(aMaxUVWCoords(i) <= aMinUVWCoords(i))
            throw(std::domain_error("OrthogonalGridUtilities: Max coordinates not strictly greater than min coordinates"));
    }
}

void OrthogonalGridUtilities::checkTargetEdgeLength(const double& aTargetEdgeLength) const
{
    if(aTargetEdgeLength <= 0)
        throw(std::domain_error("OrthogonalGridUtilities: Target edge length is not positive"));
}

void OrthogonalGridUtilities::checkNumElementsInEachDirection(const std::vector<int>& aNumElementsInEachDirection) const
{
    if(aNumElementsInEachDirection.size() != 3u)
        throw(std::domain_error("OrthogonalGridUtilities: Grid dimension must be 3"));

    for(int i = 0; i < 3; ++i)
    {
        if(aNumElementsInEachDirection[i] <= 0)
            throw(std::domain_error("OrthogonalGridUtilities: Number of elements in each direction must be positive"));
    }
}

std::vector<std::vector<int>> OrthogonalGridUtilities::getContainingGridElement(const Vector& aPoint) const
{
    std::vector<std::vector<int>> tGridIndicies;

    std::vector<int> tUIndices = getSurroundingIndices(0,aPoint);
    std::vector<int> tVIndices = getSurroundingIndices(1,aPoint);
    std::vector<int> tWIndices = getSurroundingIndices(2,aPoint);

    if(tUIndices.size() == 2u && tVIndices.size() == 2u && tWIndices.size() == 2)
    {
        tGridIndicies.push_back({tUIndices[0],tVIndices[0],tWIndices[0]});
        tGridIndicies.push_back({tUIndices[1],tVIndices[0],tWIndices[0]});
        tGridIndicies.push_back({tUIndices[0],tVIndices[1],tWIndices[0]});
        tGridIndicies.push_back({tUIndices[1],tVIndices[1],tWIndices[0]});
        tGridIndicies.push_back({tUIndices[0],tVIndices[0],tWIndices[1]});
        tGridIndicies.push_back({tUIndices[1],tVIndices[0],tWIndices[1]});
        tGridIndicies.push_back({tUIndices[0],tVIndices[1],tWIndices[1]});
        tGridIndicies.push_back({tUIndices[1],tVIndices[1],tWIndices[1]});
    }

    return tGridIndicies;
}

std::vector<int> OrthogonalGridUtilities::getSurroundingIndices(const int& aWhichDim, const Vector& aPoint) const
{
    //use cramers rule to compute change of basis for input point
    std::vector<int> tSurroundingIndices;
    double tDenominator = determinant3X3(mUBasisVector,mVBasisVector,mWBasisVector);

    double tUCoordinate = determinant3X3(aPoint,mVBasisVector,mWBasisVector)/tDenominator;
    double tVCoordinate = determinant3X3(mUBasisVector,aPoint,mWBasisVector)/tDenominator;
    // double tWCoordinate = determinant3X3(mUBasisVector,mVBasisVector,aPoint)/tDenominator;

    auto tDimensions = getGridDimensions();

    double tULength = mMaxUVWCoords(0) - mMinUVWCoords(0);
    double tVLength = mMaxUVWCoords(1) - mMinUVWCoords(1);
    // double tWLength = mMaxUVWCoords(2) - mMinUVWCoords(2);


    for(int i = 0; i < tDimensions[0]-1; ++i)
    {
        double tFloor = mMinUVWCoords(0) + i*tULength/mNumElementsInEachDirection[0];
        double tCeiling = mMinUVWCoords(0) + (i+1)*tULength/mNumElementsInEachDirection[0];

        if(tFloor <= tUCoordinate && tUCoordinate < tCeiling)
        {
            for(int j = 0; j < tDimensions[1]-1; ++j)
            {
                tFloor = mMinUVWCoords(1) + j*tVLength/mNumElementsInEachDirection[1];
                tCeiling = mMinUVWCoords(1) + (j+1)*tVLength/mNumElementsInEachDirection[1];
                if(tFloor <= tVCoordinate && tVCoordinate < tCeiling)
                {
                    // tGridIndicies.push_back({i,j,0});
                    // tGridIndicies.push_back({i+1,j,0});
                    // tGridIndicies.push_back({i,j+1,0});
                    // tGridIndicies.push_back({i+1,j+1,0});
                }
                else
                {
                    continue;
                }
            }
        }
        else
        {
            continue;
        }
    }

    return tSurroundingIndices;
}

}
