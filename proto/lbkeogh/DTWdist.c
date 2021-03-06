#include "DTWdist.h"

void printCostMatrixFloat(float **costMatrix, uint32_t xSize, uint32_t ySize) {
    for (int32_t col = 0; col < xSize; col++) {
        for (int32_t row = 0; row < ySize; row++) {
            printf("%f\t", costMatrix[col][row]);
        }
        printf("\n");
    }
}


float minimumOfVectorFloat(float *vector, uint32_t vectorLen) {
    float minimum = vector[0];
    for (int32_t i = 1; i < vectorLen; i++) {
        if(minimum > vector[i]) {
            minimum = vector[i];
        }
    }
    return minimum;
}

float maximumOfVectorFloat(float *vector, uint32_t vectorLen) {
    float maximum = vector[0];
    for (int32_t i = 1; i < vectorLen; i++) {
        if(maximum < vector[i]) {
            maximum = vector[i];
        }
    }
    return maximum;
}



float calculateDistance(float *mfcc1, float *mfcc2, uint32_t mfcc1Len, uint32_t mfcc2Len, uint_fast32_t warpingConstant) {
    int32_t w = 0;
    int32_t jMax = 0;
    int32_t jMin = 0;
    int32_t i = 0, j = 0;
    float cost = 0;
    float minVector[3] = {0, 0, 0};


    float **costMatrix = (float**) malloc(sizeof(float *) * mfcc1Len);
    for (int32_t i = 0; i < mfcc1Len; i++) {
        costMatrix[i] = (float *) malloc(sizeof(float) * mfcc2Len);
    }

    for (i = 0; i < mfcc1Len; i++) {
        for (j = 0; j < mfcc2Len; j++) {
            costMatrix[i][j] = INFINITY;
        }
    }
    costMatrix[0][0] = 0;
    

    w = MAX(warpingConstant, abs(mfcc1Len - mfcc2Len));


    for (i = 1; i < mfcc1Len; i++) {
        jMax = MAX(1, i - w);
        jMin = MIN(mfcc2Len, i + w);
        for (j = jMax; j < jMin; j++) {
            costMatrix[i][j] = 0;
        }
    }

    // printCostMatrixFloat(costMatrix, mfcc1Len, mfcc2Len);

    for (i = 1; i < mfcc1Len; i++) {
        jMax = MAX(1, i - w);
        jMin = MIN(mfcc2Len, i + w);
        // printf("%d\t%d\n", jMax, jMin);
        for (j = jMax; j < jMin; j++) {
            cost = fabs(mfcc1[i] - mfcc2[j]);
            // printf("%d\t%d\t%f\t%f\t%f\n", i, j, mfcc1[i] - mfcc2[j], mfcc1[i], mfcc2[i]);
            minVector[0] = costMatrix[i-1][j];
            minVector[1] = costMatrix[i][j - 1];
            minVector[2] = costMatrix[i - 1][j - 1];
            costMatrix[i][j] = cost + minimumOfVectorFloat(minVector, 3);
        }
    }

    // printCostMatrixFloat(costMatrix, mfcc1Len, mfcc2Len);
    // printf("\n");

    for (int32_t i = 0; i < mfcc1Len; i++) {
        free(costMatrix[i]);
    }
    free(costMatrix);

    return costMatrix[mfcc1Len - 1][mfcc2Len - 1];
}

float LBKeogh(float *mfcc1, float *mfcc2, uint32_t mfcc1Len, uint32_t mfcc2Len, uint_fast32_t warpingConstant) {
    float distance = 0;
    float upperBound = 0;
    float lowerBound = 0;
    int32_t i = 0;
    int32_t startIndex = 0;
    int32_t endIndex = 0;


    for (i = 0; i < mfcc1Len; i++) {
        ((i - warpingConstant) >= 0) ? startIndex = (i - warpingConstant) : 0;
        endIndex = i + warpingConstant;
        lowerBound = minimumOfVectorFloat(&mfcc2[startIndex], (mfcc1Len - endIndex));
        upperBound = maximumOfVectorFloat(&mfcc2[startIndex], (mfcc1Len - endIndex));
    
        if (i > upperBound) distance += ((i - upperBound) * (i - upperBound));
        else if (i < lowerBound) distance += ((i - lowerBound) * (i - lowerBound));

    }

    return distance;
}