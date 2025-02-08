#ifndef __MPU_6050_H
#define __MPU_6050_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define GRAVITY 9.81f
#define LOW_FILTER 0.02
#define HIGH_FILTER 0.98

typedef struct
{
    float P[4];//covariance matrix to tell use how well is our estimation, 2*2 matrix but we merge 2 column into 1 array
    float Q[2];//Nosie matrix of gyroscope,2*2 matrix, but only count the diagonal of matrix
    float R[3];//Nosie matrix of accelerometer,3*3 matrix, but we only count the diagonal of matrix
    float gx,gy,gz,ax,ay,az;
    float estimated_roll;//follow radian, phi
    float estimated_pitch;//follow radian,theta
		float KF_roll_degree;
		float KF_pitch_degree;
		float CF_roll_degree;
		float CF_pitch_degree;
    float CF_roll;
    float CF_pitch;
    float sr;
    float cr;
    float tp;
    float sp;
    float cp;
    float A[4];//2 by 2 matrix but we merge 2 row into 1 array
    float C[6];//3*2 matrix but we merge 3 rows into 1 array
    float h[3];//3*1 matrix 
    float f[2];//2*1 matrix
    float G[9];//3*3 matrix
    float Ginv[9];
    float K[6];//2 by 3 matrix but we merge 2 rows into 1 array
    float sampleTime;
   
    uint8_t MPU_RX_DATA [10];

}MPU6050_DataFusionTypedef;


void mpu6050init(void);
void getGyroData(MPU6050_DataFusionTypedef *m);
void getAccelData(MPU6050_DataFusionTypedef *m);
void ComplimenttaryFilterInit(MPU6050_DataFusionTypedef *m,float sampleTime);
void KalmanFilterInit(MPU6050_DataFusionTypedef *m,float PInit,float Q0,float Q1,float R0,float R1,float R2,float sampleTime);
void PredictionStep(MPU6050_DataFusionTypedef *m);
void UpdateStep(MPU6050_DataFusionTypedef *m);
void mpu6050ExtendedKalmanFilter(MPU6050_DataFusionTypedef *m);
void mpu6050ComplimentaryFilter(MPU6050_DataFusionTypedef *m);

#define MPU_I2C_ADDRESS 0x68<<1
#ifdef __cplusplus
}
#endif
#endif
