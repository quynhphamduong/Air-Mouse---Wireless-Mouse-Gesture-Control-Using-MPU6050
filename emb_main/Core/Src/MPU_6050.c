#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "MPU_6050.h"
#include "main.h"

void mpu6050init(void)
{

    uint8_t val;
	val=0;
    //Power Management 1 Using Internal 8MHz oscillator
	HAL_I2C_Mem_Write(&hi2c1, MPU_I2C_ADDRESS, 107, I2C_MEMADD_SIZE_8BIT, &val, 1, 10);
    // Sample Rate = Gyroscope Output Rate / (0 + 1) = 8 kHz
	HAL_I2C_Mem_Write(&hi2c1, MPU_I2C_ADDRESS, 25, I2C_MEMADD_SIZE_8BIT, &val, 1, 10);
    // DLPF = 1/8 kHz , we are using 8khz 
	HAL_I2C_Mem_Write(&hi2c1, MPU_I2C_ADDRESS, 26, I2C_MEMADD_SIZE_8BIT, &val, 1, 10);
    // Gyroscope full scale range +-250
	HAL_I2C_Mem_Write(&hi2c1, MPU_I2C_ADDRESS, 27, I2C_MEMADD_SIZE_8BIT, &val, 1, 10);
    // Accelerometer full scale range +-2g
	HAL_I2C_Mem_Write(&hi2c1, MPU_I2C_ADDRESS, 28, I2C_MEMADD_SIZE_8BIT, &val, 1, 10);
}

void getGyroData(MPU6050_DataFusionTypedef *m)
{
    /*
    read register that store mpu gyroscope value from register 67 to 72 and store into
    buffer address
    */
    HAL_I2C_Mem_Read(&hi2c1,  MPU_I2C_ADDRESS, 67,I2C_MEMADD_SIZE_8BIT,m->MPU_RX_DATA,6,10);

    int16_t GyroX=((*(m->MPU_RX_DATA))<<8)+(*(m->MPU_RX_DATA+1));
    int16_t GyroY=((*(m->MPU_RX_DATA+2))<<8)+(*(m->MPU_RX_DATA+3));
    int16_t GyroZ=((*(m->MPU_RX_DATA+4))<<8)+(*(m->MPU_RX_DATA+5));

    m->gx=((float)GyroX)/131*(3.141592654/180);
    m->gy=((float)GyroY)/131*(3.141592654/180);
    m->gz=((float)GyroZ)/131*(3.141592654/180);
}

void getAccelData(MPU6050_DataFusionTypedef *m)
{

    HAL_I2C_Mem_Read(&hi2c1,MPU_I2C_ADDRESS,59,I2C_MEMADD_SIZE_8BIT,m->MPU_RX_DATA,6,10);
    
    int16_t AccX=((*(m->MPU_RX_DATA))<<8)+(*(m->MPU_RX_DATA+1));
    int16_t AccY=((*(m->MPU_RX_DATA+2))<<8)+(*(m->MPU_RX_DATA+3));
    int16_t AccZ=((*(m->MPU_RX_DATA+4))<<8)+(*(m->MPU_RX_DATA+5));

    m->ax=((float)AccX)/16384;
    m->ay=((float)AccY)/16384;
    m->az=((float)AccZ)/16384;
}

void ComplimenttaryFilterInit(MPU6050_DataFusionTypedef *m,float sampleTime)
{
	m->sampleTime=sampleTime;
}

void KalmanFilterInit(MPU6050_DataFusionTypedef *m,float PInit,float Q0,float Q1,float R0,float R1,float R2,float sampleTime)
{
    m->estimated_pitch=0;
    m->estimated_roll=0;

    m->P[0]=PInit;
    m->P[1]=0;
    m->P[2]=0;
    m->P[3]=PInit;

    m->Q[0]=Q0;
    m->Q[1]=Q1;
    m->R[0]=R0;
    m->R[1]=R1;
    m->R[2]=R2;

    m->sampleTime=sampleTime;
}

void PredictionStep(MPU6050_DataFusionTypedef *m)
{
    
    m->sr=sin(m->estimated_roll);
    m->cr=cos(m->estimated_roll);
    m->tp=tan(m->estimated_pitch);
    //Euler Angles
    m->f[0]=m->gx+m->gy*m->sr*m->tp+m->gz*m->cr*m->tp;
    m->f[1]=m->gy*m->cr-m->gz*m->sr;

    //Predict our state
    m->estimated_roll=m->estimated_roll+m->sampleTime*m->f[0];
    m->estimated_pitch=m->estimated_pitch+m->sampleTime*m->f[1];

    //
    m->sr=sin(m->estimated_roll);
    m->cr=cos(m->estimated_roll);
    m->sp=sin(m->estimated_pitch);
    m->cp=cos(m->estimated_pitch);
    m->tp=tan(m->estimated_pitch);

    //Linearize the f(x,u)
    m->A[0]=m->gy*m->cr*m->tp - m->gz*m->tp*m->sr;
    m->A[1]=m->gz*m->cr*(pow(m->tp,2) + 1) + m->gy*m->sr*(pow(m->tp,2) + 1);
    m->A[2]= -m->gz*m->cr - m->gy*m->sr;
    m->A[3]=0;

    //Update corvariance matric
    float temp0=m->P[0] + m->sampleTime*(m->Q[0] + m->P[0]*m->A[0] + m->P[1]*m->A[1] + m->A[0]*m->P[0] + m->A[1]*m->P[2]);
    float temp1=m->P[1]+m->sampleTime*(m->P[0]*m->A[2] + m->P[1]*m->A[3] + m->A[0]*m->P[1] + m->A[1]*m->P[3]);
    float temp2=m->P[2]+m->sampleTime*(m->P[2]*m->A[0] + m->P[3]*m->A[1] + m->A[2]*m->P[0] + m->A[3]*m->P[2]);
    float temp3=m->P[3]+m->sampleTime*(m->Q[1] + m->P[2]*m->A[2] + m->P[3]*m->A[3] + m->A[2]*m->P[1] + m->A[3]*m->P[3]);

    m->P[0]=temp0;
    m->P[1]=temp1;
    m->P[2]=temp2;
    m->P[3]=temp3;
}

void UpdateStep(MPU6050_DataFusionTypedef *m)
{
     m->sr=sin(m->estimated_roll);
    m->cr=cos(m->estimated_roll);
    m->sp=sin(m->estimated_pitch);
    m->cp=cos(m->estimated_pitch);

    //y=h(x,u)+nosie
    m->h[0]=GRAVITY*m->sp;
    m->h[1]=-GRAVITY*m->cp*m->sr;
    m->h[2]=-GRAVITY*m->cp*m->cr;

    //Linearize the h(x,u)
    m->C[0]=0;
	m->C[1]=GRAVITY*m->cp;
	m->C[2]=-GRAVITY*m->cp*m->cr;
	m->C[3]=GRAVITY*m->sp*m->sr;
	m->C[4]=GRAVITY*m->cp*m->sr;
	m->C[5]=GRAVITY*m->cr*m->sp;

    m->G[0]=m->R[0] + m->C[0]*(m->C[0]*m->P[0] + m->C[1]*m->P[2]) + m->C[1]*(m->C[0]*m->P[1] + m->C[1]*m->P[3]);
    m->G[1]=m->C[2]*(m->C[0]*m->P[0] + m->C[1]*m->P[2]) + m->C[3]*(m->C[0]*m->P[1] + m->C[1]*m->P[3]);
    m->G[2]=m->C[4]*(m->C[0]*m->P[0] + m->C[1]*m->P[2]) + m->C[5]*(m->C[0]*m->P[1] + m->C[1]*m->P[3]);
    m->G[3]=m->C[0]*(m->C[2]*m->P[0] + m->C[3]*m->P[2]) + m->C[1]*(m->C[2]*m->P[1] + m->C[3]*m->P[3]);
    m->G[4]=m->R[1] + m->C[2]*(m->C[2]*m->P[0] + m->C[3]*m->P[2]) + m->C[3]*(m->C[2]*m->P[1] + m->C[3]*m->P[3]);
    m->G[5]=m->C[4]*(m->C[2]*m->P[0] + m->C[3]*m->P[2]) + m->C[5]*(m->C[2]*m->P[1] + m->C[3]*m->P[3]);
    m->G[6]=m->C[0]*(m->C[4]*m->P[0] + m->C[5]*m->P[2]) + m->C[1]*(m->C[4]*m->P[1] + m->C[5]*m->P[3]);
    m->G[7]=m->C[2]*(m->C[4]*m->P[0] + m->C[5]*m->P[2]) + m->C[3]*(m->C[4]*m->P[1] + m->C[5]*m->P[3]);
    m->G[8]=m->R[2] + m->C[4]*(m->C[4]*m->P[0] + m->C[5]*m->P[2]) + m->C[5]*(m->C[4]*m->P[1] + m->C[5]*m->P[3]);

    m->Ginv[0]=(m->G[4]*m->G[8] - m->G[5]*m->G[7])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[1]=-(m->G[1]*m->G[8] - m->G[2]*m->G[7])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[2]=(m->G[1]*m->G[5] - m->G[2]*m->G[4])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[3]=-(m->G[3]*m->G[8] - m->G[5]*m->G[6])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[4]=(m->G[0]*m->G[8] - m->G[2]*m->G[6])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[5]=-(m->G[0]*m->G[5] - m->G[2]*m->G[3])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[6]=(m->G[3]*m->G[7] - m->G[4]*m->G[6])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[7]=-(m->G[0]*m->G[7] - m->G[1]*m->G[6])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    m->Ginv[8]=(m->G[0]*m->G[4] - m->G[1]*m->G[3])/(m->G[0]*m->G[4]*m->G[8] - m->G[0]*m->G[5]*m->G[7] - m->G[1]*m->G[3]*m->G[8] + m->G[1]*m->G[5]*m->G[6] + m->G[2]*m->G[3]*m->G[7] - m->G[2]*m->G[4]*m->G[6]);
    
    m->K[0]=m->Ginv[0]*(m->P[0]*m->C[0] + m->P[1]*m->C[1]) + m->Ginv[3]*(m->P[0]*m->C[2] + m->P[1]*m->C[3]) + m->Ginv[6]*(m->P[0]*m->C[4] + m->P[1]*m->C[5]);
    m->K[1]=m->Ginv[1]*(m->P[0]*m->C[0] + m->P[1]*m->C[1]) + m->Ginv[4]*(m->P[0]*m->C[2] + m->P[1]*m->C[3]) + m->Ginv[7]*(m->P[0]*m->C[4] + m->P[1]*m->C[5]);
    m->K[2]=m->Ginv[2]*(m->P[0]*m->C[0] + m->P[1]*m->C[1]) + m->Ginv[5]*(m->P[0]*m->C[2] + m->P[1]*m->C[3]) + m->Ginv[8]*(m->P[0]*m->C[4] + m->P[1]*m->C[5]);
    m->K[3]=m->Ginv[0]*(m->P[2]*m->C[0] + m->P[3]*m->C[1]) + m->Ginv[3]*(m->P[2]*m->C[2] + m->P[3]*m->C[3]) + m->Ginv[6]*(m->P[2]*m->C[4] + m->P[3]*m->C[5]);
    m->K[4]=m->Ginv[1]*(m->P[2]*m->C[0] + m->P[3]*m->C[1]) + m->Ginv[4]*(m->P[2]*m->C[2] + m->P[3]*m->C[3]) + m->Ginv[7]*(m->P[2]*m->C[4] + m->P[3]*m->C[5]);
    m->K[5]=m->Ginv[2]*(m->P[2]*m->C[0] + m->P[3]*m->C[1]) + m->Ginv[5]*(m->P[2]*m->C[2] + m->P[3]*m->C[3]) + m->Ginv[8]*(m->P[2]*m->C[4] + m->P[3]*m->C[5]);

    m->estimated_roll=m->estimated_roll+m->K[0]*(m->ax - m->h[0]) + m->K[1]*(m->ay - m->h[1]) + m->K[2]*(m->az - m->h[2]);
    m->estimated_pitch=m->estimated_pitch+m->K[3]*(m->ax - m->h[0]) + m->K[4]*(m->ay - m->h[1]) + m->K[5]*(m->az - m->h[2]);

    m->P[0]=- m->P[0]*(m->C[0]*m->K[0] + m->C[2]*m->K[1] + m->C[4]*m->K[2] - 1) - m->P[2]*(m->C[1]*m->K[0] + m->C[3]*m->K[1] + m->C[5]*m->K[2]);
    m->P[1]=- m->P[1]*(m->C[0]*m->K[0] + m->C[2]*m->K[1] + m->C[4]*m->K[2] - 1) - m->P[3]*(m->C[1]*m->K[0] + m->C[3]*m->K[1] + m->C[5]*m->K[2]);
    m->P[2]=- m->P[0]*(m->C[0]*m->K[3] + m->C[2]*m->K[4] + m->C[4]*m->K[5]) - m->P[2]*(m->C[1]*m->K[3] + m->C[3]*m->K[4] + m->C[5]*m->K[5] - 1);
    m->P[3]=- m->P[1]*(m->C[0]*m->K[3] + m->C[2]*m->K[4] + m->C[4]*m->K[5]) - m->P[3]*(m->C[1]*m->K[3] + m->C[3]*m->K[4] + m->C[5]*m->K[5] - 1);
}

void mpu6050ExtendedKalmanFilter(MPU6050_DataFusionTypedef *m)
{
    getGyroData(m);
    getAccelData(m);
    PredictionStep(m);
    UpdateStep(m);
		m->KF_roll_degree=m->estimated_roll*180/3.141592654;
		m->KF_pitch_degree=m->estimated_pitch*180/3.141592654;
}

void mpu6050ComplimentaryFilter(MPU6050_DataFusionTypedef *m)
{
    float accRawAngleX=atan2( m->ay,sqrt(pow(m->ax,2) + pow(m->az,2))) ;
    m->CF_roll=HIGH_FILTER*(m->CF_roll+m->gx*m->sampleTime)+LOW_FILTER*accRawAngleX;

    float accRawAngleY=atan2( m->ax,sqrt(pow(m->ay,2) + pow(m->az,2))) ;
    m->CF_pitch=HIGH_FILTER*(m->CF_pitch+m->gy*m->sampleTime)+LOW_FILTER*accRawAngleY;

		m->CF_pitch_degree=m->CF_pitch*180/3.141592654;
		m->CF_roll_degree=m->CF_roll*180/3.141592654;
}
