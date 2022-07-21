/***************************************************************************************
* @file     adc.c
* @version  V1.0.9
* @date     3/12/2019  CEM 
*
* @note
* VORAGO Technologies / Reizy
*
* @note
* Copyright (c) 2013-2016 VORAGO Technologies. 
*
* @par
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
* ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY 
* AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. VORAGO TECHNOLOGIES 
* SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
* DAMAGES, FOR ANY REASON WHATSOEVER.
*
****************************************************************************************/
/****************************************************************************************/
/* Include Files                                                                        */
/****************************************************************************************/
#include "pp_adc.h"
#include "pp_i2c_a.h"
#include "pp_i2c_b.h"
/* System configuration*/
/****************************************************************************************/
/* Global variables declared here                                                       */
/****************************************************************************************/
volatile uint32_t ADC_cnt,  ADC_results[5] ;   //  used inside ISR and in main routine
int time_1 = 100;
int time_2 = 1000;

/* Sequence */
void PowerON_ADC(ADC_t* adc_addr){// Flowchart 1
  int ID;
  int j = 0;  // ADC power initialisation counter
  int16_t TLM45, TLM67;
  for(int i=0;i<3;i++){
    ID = 128;
    I2CB_CubeComputer(ID, adc_addr);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    TLM67 = BS_16(adc_addr->TLM[6],adc_addr->TLM[7]);
    printf(" - ID = %d -\n",ID);
    printf(" Node type               is %d .\n",adc_addr->TLM[0]);
    printf(" Interface version       is %d .\n",adc_addr->TLM[1]);
    printf(" Firmware version Major  is %d .\n",adc_addr->TLM[2]);
    printf(" Firmware version Minor  is %d .\n",adc_addr->TLM[3]);
    printf(" Run time(seconds)       is %d .\n",TLM45);
    printf(" Run time(milli seconds) is %d .\n",TLM67);
    printf("n = 1\n");
    if(adc_addr->TLM[0] == 10){
      adc_addr->STATUS = 1;
      break;
    }
    if(i == 2){
      if(j == 1){ // Limitation of ADC power initialisation
        
        /* Power on fail flag updata */
        printf("ADC power on/off control by EPS\n");
        /* ------------------------- */
        adc_addr->STATUS = 2;
        printf("n = 6\n");
        break;
      }
      /* ADC Power switch on/off control routine */
      printf("ADC power on/off control by EPS\n");
      /* --------------------------------------- */
      printf("n = 5\n");
      i = 0; // Loop counter initialisation
      j++;
    }else{
      
      ID = 130;
      I2CB_CubeComputer(ID, adc_addr);
      printf(" Program Index is %d .\n",adc_addr->TLM[0]);
      printf(" Boot Status   is %d .\n",adc_addr->TLM[1]);
      printf("n = 2\n");
      if(adc_addr->TLM[0] != 1){
        
        ID = 100;
        adc_addr->TC[1] = 1;
        I2CB_CubeComputer(ID, adc_addr);
        printf("n = 3\n");
      }
      ID = 101;
      I2CB_CubeComputer(ID, adc_addr);
      VOR_Sleep(time_2);
      printf("n = 4\n");
    }
  }
}

void HKInitial_ADC(ADC_t* adc_addr){// Flowchart 2
  int8_t TLM1_2[8], TLM1_1[8];
  int ID;
  for(int i=0;i<2;i++){
    ID = 132;
    I2CB_CubeComputer(ID, adc_addr);
    printf("n = 1\n");
    
    BS_2(adc_addr->TLM[1],TLM1_2);
    VOR_Sleep(time_1);
    BS_1(adc_addr->TLM[1],TLM1_1);
    VOR_Sleep(time_1);
    printf(" - ADC CURRENT STATUS -\n");
    printf("ADCS run mode  = %d\n", TLM1_2[0]);
    printf("Signal Enabled = %d\n", TLM1_1[4]);
    printf("Motor Enabled  = %d\n\n", TLM1_1[5]);
    adc_addr->STATUS = 2;
    
    if(TLM1_2[0] == 1 && TLM1_1[4] == 1 && TLM1_1[5] ==1){
      
      printf("n = 4\n");
      adc_get_HK(adc_addr);
      adc_addr->STATUS = 1;
      break;
      
    }else{
      
      ID = 10;
      adc_addr->TC[1] = 0x1;
      I2CB_CubeComputer(ID, adc_addr);
      printf("Set ADC first configuration...\n");
      for(int i=0;i<100;i++){
        ID = 240;  
        I2CB_CubeComputer(ID, adc_addr);
        VOR_Sleep(time_1);
        if(adc_addr->TLM[1] == 0x01){
          printf(" => Done!\n");
          break;
        }
        ID = 132;
        I2CB_CubeComputer(ID, adc_addr);
        BS_2(adc_addr->TLM[1],TLM1_2);
        if(TLM1_2[0] == 0x01){
          printf(" => Done!\n");
          break;
        }
        printf("  -> check the number of attempt i = %d\n",i);
      }
      
      ID = 11;
      adc_addr->TC[1] = 0x5;
      I2CB_CubeComputer(ID, adc_addr);
      printf("n = 3\n");
      printf("Set ADC secound configuration...\n");
      for(int i=0;i<100;i++){
        ID = 240;  
        I2CB_CubeComputer(ID, adc_addr);
        if(adc_addr->TLM[1] == 0x01){
          printf(" => Done!\n");
          break;
        }
        ID = 132;
        I2CB_CubeComputer(ID, adc_addr);
        BS_1(adc_addr->TLM[1],TLM1_1);
        if(TLM1_1[4] == 1 && TLM1_1[5] ==1){
          printf(" => Done!\n");
          break;
        }
        VOR_Sleep(time_1);
        printf("  -> check the number of attempt i = %d\n",i);
      }
    }
  }
}

void EstAngRate_ADC(ADC_t* adc_addr){// Flowchart 3
  int ID, i = 0, j = 0, omega_est[64], omega_mems[64], error[64], Timer = 0;
  int8_t TLM0_4[64];
  
  while(1){
    HK_IniRateTele(adc_addr);
    printf("n = 1\n");
    for(int c=0;c<12;c++){
      printf(" HK_ADC[%d] = %d\n",c,adc_addr->HK_normal[c]);
    }
    VOR_Sleep(time_2);
    if(i == 3){
      for(int k=0;k<2;k++){
        ID = 14;
        adc_addr->TC[1] = 2;
        I2CB_CubeComputer(ID, adc_addr);
        printf("n = 3\n");
        VOR_Sleep(time_2);
        ID = 132;
        I2CB_CubeComputer(ID, adc_addr);
        printf("n = 4\n");
        int8_t TLM_4[64];
        BS_4(adc_addr->TLM[0],TLM0_4);
        printf("Estimation mode = %d\n", TLM0_4[0]);
        
        if(TLM_4[0] == 2){
          /* Timer count */
          printf("Timer count start\n");
          /* ----------- */
          printf("n = 5\n");
          break;
        }
      }
    }
    if(Timer > 300 || i == 6){
      /* Timer reset and stop */
      printf("Timer reset and stop\n");
      printf("n = 6\n");
      /* -------------------- */
      VOR_Sleep(time_2);
      ID = 147;
      I2CB_CubeComputer(ID, adc_addr);
      omega_est[0] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
      omega_est[1] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
      omega_est[2] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
      printf(" est_x = %d\n",omega_est[0]);
      printf(" est_y = %d\n",omega_est[1]);
      printf(" est_z = %d\n",omega_est[2]);
      ID = 155;
      I2CB_CubeComputer(ID, adc_addr);
      omega_mems[0] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
      omega_mems[1] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
      omega_mems[2] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
      printf(" mems_x = %d\n",omega_mems[0]);
      printf(" mems_y = %d\n",omega_mems[1]);
      printf(" mems_z = %d\n",omega_mems[2]);
      
      error[0] = abs(omega_mems[0] - omega_est[0]);
      error[2] = abs(omega_mems[2] - omega_est[2]);
      printf(" error_x = %d\n",error[0]);
      printf(" error_z = %d\n",error[2]);
      
      if(error[0] < 100 && error[2] < 100){
        printf("Estimation Complete\n");
        adc_addr->STATUS = 1;
        break;
      }
      if(j == 1){
        /* Estimation error flag update */
        printf("Estimation error\n");
        /* ---------------------------- */
        adc_addr->STATUS = 2;
        printf("n = 8\n");
        break;
      }
      j++;
      i = 4;
      printf("n = 7\n");
    }
    printf("n = 2\n");
    i++;
    printf("i = %d\n",i);
  }
  printf(" ----- Estimation Angular Velocity Sequence Finish -----\n");
}
void Detumbling_ADC(ADC_t* adc_addr){// Flowchart 4
  int ID, i = 0, j = 0, omega[128], omega_ini[128], omega_fin[128], Timer = 0;
  int8_t TLM0_4[8];
  /* --- Detumbling mode --- */
  while(1){
    HK_DetumblTele(adc_addr);
    printf("n = 1\n");
    VOR_Sleep(time_2);
    if(i == 6){
      for(int k=0;k<2;k++){
#if EM
        ID = 147;
        I2CB_CubeComputer(ID, adc_addr);
        omega_ini[0] = abs(BS_16(adc_addr->TLM[0],adc_addr->TLM[1]));
        omega_ini[1] = abs(BS_16(adc_addr->TLM[2],adc_addr->TLM[3]));
        omega_ini[2] = abs(BS_16(adc_addr->TLM[4],adc_addr->TLM[5]));
        printf("n = 4.1\n");
#else
        omega_ini[0] = 200;     // 200 -> 2 [deg/s]
        omega_ini[1] = 200;
        omega_ini[2] = 200;
        printf("n = 4.2\n");
#endif
        printf("omega_ini_x = %d\n",omega_ini[0]);
        printf("omega_ini_y = %d\n",omega_ini[1]);
        printf("omega_ini_z = %d\n",omega_ini[2]);
        printf("\n --- Detumbling start ---\n");
        ID = 13;
        adc_addr->TC[1] = 1;
        adc_addr->TC[2] = 0x3C;        // 60[s] -> 0000 0000 0011 1100 = 0x00 + 0x3C
        adc_addr->TC[3] = 0x00;
        I2CB_CubeComputer(ID, adc_addr);
        ID = 132;
        I2CB_CubeComputer(ID, adc_addr);
        BS_4(adc_addr->TLM[0],TLM0_4);
        printf("Control mode = %d\n",TLM0_4[1]);
        printf("n = 5\n");
        if(TLM0_4[1] == 1){
          /* Timer count */
          printf("\n --- Timer count start ---\n");
          printf("n = 6\n");
          /* ----------- */
          break;
        }
      }
    }
    printf("omega_ini_x = %d\n",omega_ini[0]);
    printf("omega_ini_y = %d\n",omega_ini[1]);
    printf("omega_ini_z = %d\n",omega_ini[2]);
    ID = 132;
    I2CB_CubeComputer(ID, adc_addr);
    BS_4(adc_addr->TLM[0],TLM0_4);
    printf("Control mode = %d\n",TLM0_4[1]);
    printf("n = 2\n");
    if(Timer > 600 || (TLM0_4[1] == 0 && i > 0x0C)){
      printf("Timer = %d\n",Timer);
      break;
    }
    i++;
    printf("n = 3\n");
    printf("i = %d\n",i);
  }
  /* Timer reset and stop */
  printf("\n --- Timer reset and stop ---\n");
  printf("n = 7\n");
  /* -------------------- */
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  omega_fin[0] = abs(BS_16(adc_addr->TLM[0],adc_addr->TLM[1]));
  omega_fin[1] = abs(BS_16(adc_addr->TLM[2],adc_addr->TLM[3]));
  omega_fin[2] = abs(BS_16(adc_addr->TLM[4],adc_addr->TLM[5]));
  
  printf("omega_fin_x = %d\n",omega_fin[0]);
  printf("omega_fin_y = %d\n",omega_fin[1]);
  printf("omega_fin_z = %d\n",omega_fin[2]);
  printf("omega_ini_x = %d\n",omega_ini[0]);
  printf("omega_ini_y = %d\n",omega_ini[1]);
  printf("omega_ini_z = %d\n",omega_ini[2]);
  printf("n = 8\n");
  
  if(omega_fin[0] < omega_ini[0] && omega_fin[1] < omega_ini[1] && omega_fin[2] < omega_ini[2]){
    /* --- Y-Thomson mode --- */
    printf("n = 9\n");
    while(1){
      HK_DetumblTele(adc_addr);
      printf("n = 10\n");
      VOR_Sleep(time_2);
      
      if(j == 6){
        for(int k=0;k<3;k++){
          ID = 13;
          adc_addr->TC[1] = 2;
          adc_addr->TC[2] = 0x10;        // 3600[s] -> 1110 0001 0000 = 0x0E + 0x10
          adc_addr->TC[3] = 0x0E;
          I2CB_CubeComputer(ID, adc_addr);
          printf("n = 13\n");
          ID = 132;
          I2CB_CubeComputer(ID, adc_addr);
          BS_4(adc_addr->TLM[0],TLM0_4);
          if(TLM0_4[1] == 2 || k==2){
#if EM
            
#else
            omega[0] = 150;       // 150 -> 1.5 [deg/s]
            omega[1] = 150;
            omega[2] = 150;
            printf("omega_x = %d\n",omega[0]);
            printf("omega_y = %d\n",omega[1]);
            printf("omega_z = %d\n",omega[2]);
            printf("n = BBM\n");
            printf("k = %d\n",k);
            
#endif
            /* Timer count */
            printf("\n --- Timer count start ---\n");
            printf("n = 14\n");
            /* ----------- */
            break;
          }
        }        
      }
      if(j > 6){
#if EM
        
        ID = 147;
        I2CB_CubeComputer(ID, adc_addr);
        omega[0] = abs(BS_16(adc_addr->TLM[0],adc_addr->TLM[1]));
        omega[1] = abs(BS_16(adc_addr->TLM[2],adc_addr->TLM[3]));
        omega[2] = abs(BS_16(adc_addr->TLM[4],adc_addr->TLM[5]));
#else
        omega[0] = omega[0] - 10;
        omega[1] = omega[1] - 10;
        omega[2] = omega[2] - 10;
        printf("omega_x = %d\n",omega[0]);
        printf("omega_y = %d\n",omega[1]);
        printf("omega_z = %d\n",omega[2]);
#endif
        if(omega[0] < 50 && omega[2] < 50){
          /* Timer reset and stop */
          printf("\n --- Timer reset and stop ---\n");
          printf("n = 15\n");
          /* -------------------- */
          adc_addr->STATUS = 1;
          break;
        }
      }
      j++;
      printf("n = 12\n");
      printf("j = %d\n",j);
    }
  }else{
    /* Detumbling error flag update */
    printf("\n --- Detumbling error ---\n");
    printf("n = 17\n");
    adc_addr->STATUS = 2;
    /* ---------------------------- */
  }
  ID = 13;              // No control mode
  adc_addr->TC[1] = 0;
  adc_addr->TC[2] = 0;
  adc_addr->TC[3] = 0;
  I2CB_CubeComputer(ID, adc_addr);
  printf("\n ----- Finish Detumbling Sequence Finish -----\n");
}
void PitchEst_ADC(ADC_t* adc_addr, int* GPS_adc_addr){
  int8_t TLM1_2[64], TLM1_1[64];
  int ID, t2 = 1000;
  int16_t omega_y, angle_y_1, angle_y_2, delta_angle, error_omega;
  
  ID = 45;
  for(int i=0;i<64;i++){
    adc_addr->TC[0+1] = GPS_adc_addr[i];
  }
  I2CB_CubeComputer(ID, adc_addr);
  
  for(int i=0;i<5;i++){
    ID = 240;  
    I2CB_CubeComputer(ID, adc_addr);
    VOR_Sleep(time_1);
    if(adc_addr->TC[1] == 0x01){
      printf("Done!\n");
      break;
    }
    printf("check the prosessd flag i = %d\n",i);
  }
  
  for(int i=0;i<5;i++){
    ID = 132;
    I2CB_CubeComputer(ID, adc_addr);
    VOR_Sleep(time_1);
    BS_2(adc_addr->TLM[1],TLM1_2);
    VOR_Sleep(time_1);
    BS_1(adc_addr->TLM[1],TLM1_1);
    VOR_Sleep(time_1);
    
    printf("Estimation mode = %d\n", TLM1_2[0]);
    printf("Signal Enabled  = %d\n", TLM1_1[4]);
    printf("Motor Enabled   = %d\n", TLM1_1[5]);
    
    if(TLM1_1[4] == 0 || TLM1_1[5] == 0){
      ID = 11;
      adc_addr->TC[1] = 0x5;
      I2CB_CubeComputer(ID, adc_addr);
    }
    
    if(TLM1_2[0] != 3){
      ID = 14;
      adc_addr->TC[1] = 0x03;
      I2CB_CubeComputer(ID, adc_addr);      
    }
    
    if(TLM1_1[4] == 1 && TLM1_1[5] == 1 && TLM1_2[2] == 3){
      break;
    }
  }
  
  ID = 13;
  adc_addr->TC[1] = 3;
  adc_addr->TC[2] = 0x3C;        // 60[s] -> 0011 1100 = 0x00 + 0x3C = TC[3] + TC[2]
  adc_addr->TC[3] = 0x00;
  I2CB_CubeComputer(ID, adc_addr);
  HK_PitchEst(adc_addr);
  
  for(int i=0;i<10;i++){
    ID = 147;
    I2CB_CubeComputer(ID, adc_addr);
    omega_y = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    ID = 146;
    I2CB_CubeComputer(ID, adc_addr);
    angle_y_1 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    VOR_Sleep(t2);
    ID = 146;
    I2CB_CubeComputer(ID, adc_addr);
    angle_y_2 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    
    delta_angle = angle_y_2 - angle_y_1;        // dx = (x2 - x1)/(t2 - t1), because t2 = 1, t1 = 0
    error_omega = abs(omega_y - delta_angle);
    
    if(error_omega < 10){// 10 -> 0.1 deg/s
      adc_addr->STATUS = 1;
      break;
    }   
  }
  adc_addr->STATUS = 2;
}
void Y_Wheel_ADC(ADC_t* adc_addr){
  int ID, timer = 0;
  float omega_c[64],omega_y, I_yy = 5e-2,I_wheel = 4.87e-5;
  int8_t TLM1_1[8], TLM2_1[8], TLM0_4[8];;
  
  for(int i=0;i<5;i++){
    ID = 132;
    I2CB_CubeComputer(ID, adc_addr);
    VOR_Sleep(time_1);
    BS_4(adc_addr->TLM[0],TLM0_4);
    VOR_Sleep(time_1);
    BS_1(adc_addr->TLM[1],TLM1_1);
    VOR_Sleep(time_1);
    BS_1(adc_addr->TLM[2],TLM2_1);
    VOR_Sleep(time_1);
    
    printf("Signal Enabled  = %d\n", TLM1_1[4]);
    printf("Motor Enabled   = %d\n", TLM1_1[5]);
    printf("CubeWheel-2     = %d\n", TLM2_1[1]);
    printf("Estimation mode = %d\n", TLM0_4[0]);
    printf("Control mode    = %d\n", TLM0_4[1]);
    
    if(TLM1_1[4] == 0 || TLM1_1[5] == 0 || TLM2_1[1] == 0){
      ID = 11;
      adc_addr->TC[1] = 0x05;
      adc_addr->TC[2] = 0x00;
      adc_addr->TC[3] = 0x01;
      I2CB_CubeComputer(ID, adc_addr);
    }
    if(TLM0_4[0] != 3){
      ID = 14;
      adc_addr->TC[1] = 0x03;
      I2CB_CubeComputer(ID, adc_addr);
      VOR_Sleep(time_1);
      
    }
    if(TLM0_4[1] != 0){
      ID = 13;
      adc_addr->TC[1] = 0x0;
      adc_addr->TC[2] = 0x0;
      adc_addr->TC[3] = 0x0;
      I2CB_CubeComputer(ID, adc_addr);
      VOR_Sleep(time_1);
    }
    if(TLM1_1[4] == 1 && TLM1_1[5] == 1 && TLM2_1[1] == 1 && TLM0_4[0] == 3 && TLM0_4[1] == 0){
      break;
    }
  }
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  omega_y = BS_16(adc_addr->TLM[2],adc_addr->TLM[3])*0.01;
  omega_c[1] = 0.1667*I_yy*omega_y/I_wheel;
  
  ID = 17;
  adc_addr->TC[1] = 0;
  adc_addr->TC[2] = 0;
  adc_addr->TC[3] = 0;
  adc_addr->TC[4] = 0;
  adc_addr->TC[5] = 0;
  adc_addr->TC[6] = 0;
  I2CB_CubeComputer(ID, adc_addr);
  
  printf("timer count start\n");
  while(1){
    Y_Wheel_HK(ID ,adc_addr);
    if(timer == 120){
      printf("timer count stop\n");
      ID = 17;
      adc_addr->TC[1] = 0;
      adc_addr->TC[2] = 0;
      adc_addr->TC[3] = 0;
      adc_addr->TC[4] = 0;
      adc_addr->TC[5] = 0;
      adc_addr->TC[6] = 0;
      I2CB_CubeComputer(ID, adc_addr);
      VOR_Sleep(time_1);
      ID = 13;
      adc_addr->TC[1] = 0x2;
      adc_addr->TC[2] = 0x10;        // 3600[s] -> 1110 0001 0000 = 0x0E + 0x10 = TC[3] + TC[2]
      adc_addr->TC[3] = 0x0E;
      I2CB_CubeComputer(ID, adc_addr);
      adc_addr->STATUS = 1;
      break;
    }
    VOR_Sleep(time_2);
#if EM == 0
    timer = timer + 10;
    printf("timer = %d\n",timer);
#endif
  }
}

/* Predefined Process */
void adc_get_HK(ADC_t* adc_addr){
  int ID;
  
  ID = 146;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 148;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 149;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 150;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 195;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 142;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 143;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 144;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 132;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 191;
  I2CB_CubeComputer(ID, adc_addr);
  HK_data(ID,adc_addr);
  VOR_Sleep(time_1);
}
void HK_IniRateTele(ADC_t* adc_addr){
  int ID;  
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
  VOR_Sleep(time_1);
  HK_Detumb(ID,adc_addr);
  ID = 155;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 151;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
}
void HK_DetumblTele(ADC_t* adc_addr){
  int ID;
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 155;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 151;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 157;
  I2CB_CubeComputer(ID, adc_addr);
  HK_Detumb(ID,adc_addr);
}
void HK_PitchEst(ADC_t* adc_addr){
  int ID;
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  PitchEst_HK(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 201;
  I2CB_CubeComputer(ID, adc_addr);  
  PitchEst_HK(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 155;
  I2CB_CubeComputer(ID, adc_addr);
  PitchEst_HK(ID,adc_addr);
  VOR_Sleep(time_1);  
  ID = 170;
  I2CB_CubeComputer(ID, adc_addr);
  PitchEst_HK(ID,adc_addr);
  
}
void HK_Y_Wheel(ADC_t* adc_addr){
  int ID;
  ID = 147;
  I2CB_CubeComputer(ID, adc_addr);
  Y_Wheel_HK(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 201;
  I2CB_CubeComputer(ID, adc_addr);  
  Y_Wheel_HK(ID,adc_addr);
  VOR_Sleep(time_1);
  ID = 155;
  I2CB_CubeComputer(ID, adc_addr);
  Y_Wheel_HK(ID,adc_addr);
  VOR_Sleep(time_1);  
  ID = 170;
  I2CB_CubeComputer(ID, adc_addr);
  Y_Wheel_HK(ID,adc_addr);
  ID = 156;
  I2CB_CubeComputer(ID, adc_addr);
  Y_Wheel_HK(ID,adc_addr);
}
/* HK */
void HK_data(int ID , ADC_t* adc_addr){
  switch (ID){
  case 146:// Estimation Attitude Angles
    adc_addr->HK_normal[0] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[1] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[2] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 147:// Estimation Anglar Velocity
    adc_addr->HK_normal[3] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[4] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[5] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 148:// Satellite Position (ECI)
    adc_addr->HK_normal[6] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[7] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[8] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 149:// Satellite Velocity (ECI)
    adc_addr->HK_normal[9]  =BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[10] =BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[11] =BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 150:// Satellite Position (LLH)
    adc_addr->HK_normal[12] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[13] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[14] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 195:// Power/Temperature
    for(int i = 0; i < 19; i ++){
      adc_addr->HK_normal[i+15] = BS_16(adc_addr->TLM[2*i],adc_addr->TLM[2*i+1]);  
    }
    break;
  case 142:// SRAM latchup counter
    adc_addr->HK_normal[34] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[35] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    break;
  case 143:// EDAC Error Counters
    adc_addr->HK_normal[36] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[37] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[38] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);    
    break;
  case 144:// Communication Status
    adc_addr->HK_normal[39] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[40] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[41] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 132:// State of ADCS
    adc_addr->HK_normal[42] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[43] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[44] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 191:// ADCS measurement
    for(int i = 0; i < 15; i ++){
      adc_addr->HK_normal[i+45] = BS_16(adc_addr->TLM[2*i],adc_addr->TLM[2*i + 1]);  
    }
    break;
  case 151:// Magnetic Field vector
    adc_addr->HK_normal[45] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[46] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[47] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 152:// Coarse Sun Vector
    adc_addr->HK_normal[48] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[49] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[50] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 155:// MEMS anglar velocity
    adc_addr->HK_normal[57] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[58] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[59] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  default:
    printf("No ID\n");
  }
}
void HK_Detumb(int ID , ADC_t* adc_addr){
  if(ID == 147){// Estimation Anglar Velocity
    adc_addr->HK_normal[0] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[1] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[2] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);    
  }else if(ID == 155){// Rate Sensor Rates
    adc_addr->HK_normal[3] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[4] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[5] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);    
  }else if(ID == 170){// Raw magnetometer measurements
    adc_addr->HK_normal[6] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[7] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[8] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
  }else if(ID == 151){// Magnetic Field Vector
    adc_addr->HK_normal[6] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[7] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[8] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
  }else if(ID == 157){// Magnetorquer Command
    adc_addr->HK_normal[9]  = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[10] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[11] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
  }
}
void PitchEst_HK(int ID, ADC_t* adc_addr){
  switch (ID){
  case 147:// Estimation Attitude Angles
    adc_addr->HK_normal[0] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[1] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[2] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 201:// Estimation Fine Anglar Velocity
    adc_addr->HK_normal[3] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[4] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[5] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 155:// Rate Sensor Rates
    adc_addr->HK_normal[6] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[7] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[8] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 170:// Raw magnetometer measurements 
    adc_addr->HK_normal[9]  = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[10] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[11] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  default:
    break;
  }
}
void Y_Wheel_HK(int ID, ADC_t* adc_addr){
  switch (ID){
  case 147:// Estimation Attitude Angles
    adc_addr->HK_normal[0] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[1] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[2] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 201:// Estimation Fine Anglar Velocity
    adc_addr->HK_normal[3] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[4] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[5] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 155:// Rate Sensor Rates
    adc_addr->HK_normal[6] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[7] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[8] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 170:// Raw magnetometer measurements 
    adc_addr->HK_normal[9]  = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[10] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[11] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  case 156:// Wheel Speed
    adc_addr->HK_normal[12] = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    adc_addr->HK_normal[13] = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    adc_addr->HK_normal[14] = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    break;
  default:
    break;
  }
}
void HK_trans(ADC_t* adc_addr){
  for(int i=0;i<6;i++){
    adc_addr->HK_detail[i] = adc_addr->HK_normal[i]*0.01;
  }
  for(int i=6;i<12;i++){
    adc_addr->HK_detail[i] = adc_addr->HK_normal[i]*0.25;
  }
  adc_addr->HK_detail[12] = adc_addr->HK_normal[12]*0.01;
  adc_addr->HK_detail[13] = adc_addr->HK_normal[13]*0.01;
  adc_addr->HK_detail[14] = adc_addr->HK_normal[14]*0.01;
  adc_addr->HK_detail[15] = adc_addr->HK_normal[15]*0.1;
  adc_addr->HK_detail[16] = adc_addr->HK_normal[16]*0.1;
  adc_addr->HK_detail[17] = adc_addr->HK_normal[17]*0.1;
  adc_addr->HK_detail[18] = adc_addr->HK_normal[18]*0.1; 
  adc_addr->HK_detail[19] = adc_addr->HK_normal[19]*0.48828125;
  adc_addr->HK_detail[20] = adc_addr->HK_normal[20]*0.48828125;
  adc_addr->HK_detail[21] = adc_addr->HK_normal[21]*0.48828125;
  adc_addr->HK_detail[22] = adc_addr->HK_normal[22]*0.01;
  adc_addr->HK_detail[23] = adc_addr->HK_normal[23]*0.01;
  adc_addr->HK_detail[24] = adc_addr->HK_normal[24]*0.01;
  adc_addr->HK_detail[25] = adc_addr->HK_normal[25]*0.01;
  adc_addr->HK_detail[26] = adc_addr->HK_normal[26]*0.1;
  adc_addr->HK_detail[27] = adc_addr->HK_normal[27]*0.01;
  adc_addr->HK_detail[28] = adc_addr->HK_normal[28];
  adc_addr->HK_detail[29] = adc_addr->HK_normal[29]*0.1;
  adc_addr->HK_detail[30] = adc_addr->HK_normal[30]*0.1;
  for(int i=31;i<42;i++){
    adc_addr->HK_detail[i] = adc_addr->HK_normal[i];
  }
  for(int i=42;i<45;i++){
    adc_addr->HK_detail[i] = adc_addr->HK_normal[i]*0.1;
  }
  for(int i=45;i<54;i++){
    adc_addr->HK_detail[i] = adc_addr->HK_normal[i]*0.0001;
  }
  for(int i=54;i<57;i++){
    adc_addr->HK_detail[i] = adc_addr->HK_normal[i]*0.01;
  }  
}
/* Function */
void TC_data(int ID,ADC_t* adc_addr){           // Telecommand data setting
  int ADC = 0;
  
  adc_addr->TC[0] = ID;
  
  switch(ID){
  case 10:
    printf("\nPlease select ADCS state & control loop behaviour\n");
    printf(" 0 -> Off\n");
    printf(" 1 -> Enable\n");
    printf(" 2 -> Triggered\n");
    printf(" 3 -> Simulation\n");
    scanf("%d",&ADC);
    adc_addr->TC[1] = ADC;
    
    break;
  case 14:
    printf("\nPlease select Attitude Estimation Mode\n");
    printf(" 0 -> No attitude estimation\n");
    printf(" 1 -> MEMS rate sensing\n");
    printf(" 2 -> Magnetometer rate filter\n");
    printf(" 3 -> Magnetometer rate filter with pitch estimation\n");
    printf(" 4 -> Magnetometer and Fine-sun TRIAD algorithm\n");
    printf(" 5 -> Full-state EKF\n");
    printf(" 6 -> MEMS gyro EKF\n");
    printf(" 7 -> User Coded Estimation Mode\n");
    scanf("%d",&ADC);
    adc_addr->TC[1] = ADC;
    break;
    
  case 100:
    adc_addr->TC[1] = 1;
    break;
    
  case 102:
    adc_addr->TC[1] = 1;
    break;
    
  case 11:
    printf("\nPlease select which program to turn on\n");
    printf(" [00 00 00 01] = 1  -> Signal Power On!\n");
    printf(" [00 00 01 00] = 4  -> Motor Power  On!\n");
    printf(" [00 01 00 00] = 16 -> Cube Sens1   On!\n");
    printf(" [01 00 00 00] = 64 -> Cube Sens2   On!\n");
    scanf("%d",&ADC);
    adc_addr->TC[1] = ADC;
    printf("\nPlease select which program to turn on\n");
    printf(" [00 00 00 01] = 1  -> Cube Star    On!\n");
    printf(" [00 00 01 00] = 4  -> Cube Wheel_1 On!\n");
    printf(" [00 01 00 00] = 16 -> Cube Wheel_2 On!\n");
    printf(" [01 00 00 00] = 64 -> Cube Wheel_3 On!\n");
    scanf("%d",&ADC);
    adc_addr->TC[2] = ADC;
    printf("\nPlease select which program to turn on\n");
    printf(" [00 00 00 01] = 1  -> Motor Power  On!\n");
    printf(" [00 00 01 00] = 4  -> GPS Power    On!\n");
    scanf("%d",&ADC);
    break;
    
  case 13:
    printf("\nPlease select Attitude Estimation Mode\n");
    printf("  0 -> No control\n");
    printf("  1 -> Detumbling control\n");
    printf("  2 -> Y-Thomson spin\n");
    printf("  3 -> Y-Wheel momentum stabilized - Initial Pitch Acquisition\n");
    printf("  4 -> Y-Wheel momentum stabilized - Steady State\n");
    printf("  5 -> XYZ-Wheel control\n");
    printf("  6 -> Rwheel sun tracking control\n");
    VOR_Sleep(10);
    printf("  7 -> Rwheel target tracking control\n");
    printf("  8 -> Very Fast-spin Detumbling control\n");
    printf("  9 -> Fast-spin Detumbling control\n");
    printf(" 10 -> User Specific Control Mode 1\n");
    printf(" 11 -> User Specific Control Mode 2\n");
    printf(" 12 -> Stop R-wheels\n");
    printf(" 13 -> User Coded Control Mode\n");
    printf(" 14 -> Sun-tracking yaw- or roll-only wheel control mode\n");
    VOR_Sleep(10);
    printf(" 15 -> Target-tracking yaw-only wheel control Mode\n");
    scanf("%d",&ADC);
    adc_addr->TC[1] = ADC;
    printf("\nPlease set the Timeout time (0 ~ 65,535(0xFFFF) [s])\n");
    scanf("%d",&ADC);
    adc_addr->TC[2] = 0xFF & ADC;
    adc_addr->TC[3] = ADC >> 8;
    break;
    
  case 16:
    printf("\nPlease enter RW_x speed in the range 0 ~ 6000\n");
    scanf("%d",&ADC);
    adc_addr->TC[1] = 0xFF & ADC;
    adc_addr->TC[2] = ADC >> 8;
    printf("\nPlease enter RW_y speed in the range 0 ~ 6000\n");
    scanf("%d",&ADC);
    adc_addr->TC[3] = 0xFF & ADC;
    adc_addr->TC[4] = ADC >> 8;
    printf("\nPlease enter RW_z speed in the range 0 ~ 6000\n");
    scanf("%d",&ADC);
    adc_addr->TC[5] = 0xFF & ADC;
    adc_addr->TC[6] = ADC >> 8;
    break;
    
  default:
    break; 
  }
  
}
void TLM_print(int ID, ADC_t* adc_addr){             // Telemetry printf
  int8_t  TLM0_1[8], TLM1_1[8], TLM2_1[8], TLM3_1[8], TLM4_1[8], TLM5_1[8]; // 1bit data
  int8_t  TLM1_2[8], TLM0_4[8];// 2 and 4 bit data
  int16_t TLM01, TLM12, TLM23, TLM45, TLM67, TLM_16[64];
  int32_t TLM0123;
  float   X, Y, Z;
  float   V_3, V_5, V_b, T_m, T_rm, f0001[32], f001[16], fTLM_16[32];
#if Detail
  int32_t TLM2345;
  float   CSC, MC, CSMT;
#endif
  
  switch(ID){
  case 128:
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    TLM67 = BS_16(adc_addr->TLM[6],adc_addr->TLM[7]);
    printf(" Node type               is %d .\n",adc_addr->TLM[0]);
    printf(" Interface version       is %d .\n",adc_addr->TLM[1]);
    printf(" Firmware version Major  is %d .\n",adc_addr->TLM[2]);
    printf(" Firmware version Minor  is %d .\n",adc_addr->TLM[3]);
    printf(" Run time(seconds)       is %d .\n",TLM45);
    printf(" Run time(milli seconds) is %d .\n",TLM67);
    
    if(adc_addr->TLM[0] == 10){
      adc_addr->flag = 1;
    }
    printf("flag = %d\n",adc_addr->flag);
    break;
    
#if Detail
  case 129:
    BS_4(adc_addr->TLM[0],TLM0_4);
    TLM12 = BS_16(adc_addr->TLM[1],adc_addr->TLM[2]);
    printf(" Cause of MCU reset      is %d .\n",TLM0_4[0]);
    printf(" Cause of MCU Boot Cause is %d .\n",TLM0_4[1]);
    printf(" Boot Counter            is %d .\n",TLM0_4[2]);
    printf(" Boot Program index      is %d .\n",adc_addr->TLM[3]);
    printf(" Firmware version Major  is %d .\n",adc_addr->TLM[4]);
    printf(" Firmware version Minor  is %d .\n",adc_addr->TLM[5]);
    break;
    
  case 130:
    printf(" Program Index is %d .\n",adc_addr->TLM[0]);
    printf(" Boot Status   is %d .\n",adc_addr->TLM[1]);
    break;
#endif
    
  case 132:
    if(adc_addr->flag == 0){// Bootloader
      TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
      BS_1(adc_addr->TLM[2],TLM2_1);
      BS_1(adc_addr->TLM[3],TLM3_1);
      
#if Detail
      printf(" Boot loader.\n");
      printf(" Up-time (s) is %d .\n",TLM01);
      printf(" SRAM1       is %d .\n",TLM2_1[0]);
      printf(" SRAM2       is %d .\n",TLM2_1[1]);
      printf(" SRAM Latch-up Error and not be recovered is %d .\n",TLM2_1[2]);
      printf(" SRAM Latch-up Occurred but recovered     is %d .\n",TLM2_1[3]);
      printf(" SD card initialisation error    is %d .\n",TLM2_1[4]);
      printf(" SD card read error              is %d .\n",TLM2_1[5]);
      VOR_Sleep(10);
      printf(" SD card write error             is %d .\n",TLM2_1[6]);
      printf(" External Flash Error            is %d .\n",TLM2_1[7]);
      printf(" Internal Flash Error            is %d .\n",TLM3_1[0]);
      printf(" EEPROM Error                    is %d .\n",TLM3_1[1]);
      printf(" Boot Register Corrupt           is %d .\n",TLM3_1[2]);
      printf(" Communications Error with Radio is %d .\n",TLM3_1[3]);
#endif
      
    }else{// CubeACP
      BS_4(adc_addr->TLM[0],TLM0_4);
      BS_2(adc_addr->TLM[1],TLM1_2);
      BS_1(adc_addr->TLM[1],TLM1_1);
      BS_1(adc_addr->TLM[2],TLM2_1);
      BS_1(adc_addr->TLM[3],TLM3_1);
      BS_1(adc_addr->TLM[4],TLM4_1);
      BS_1(adc_addr->TLM[5],TLM5_1);
      
      printf(" Raw data.\n");
      printf(" TLM[0] = %d.\n",adc_addr->TLM[0]);
      printf(" TLM[1] = %d.\n",adc_addr->TLM[1]);
      printf(" TLM[2] = %d.\n",adc_addr->TLM[2]);
      printf(" TLM[3] = %d.\n",adc_addr->TLM[3]);
      printf(" TLM[4] = %d.\n",adc_addr->TLM[4]);
      printf(" TLM[5] = %d.\n",adc_addr->TLM[5]);
      VOR_Sleep(time_1);
      printf(" CubeACP.\n");
      printf(" Attitude estimation mode   is %d .\n",TLM0_4[0]);
      printf(" Control mode               is %d .\n",TLM0_4[1]);
      printf(" ADCS run mode              is %d .\n",TLM1_2[0]);
      printf(" ASGP4 mode                 is %d .\n",TLM1_2[1]);
      printf(" CubeControl Signal Enabled is %d .\n",TLM1_1[4]);
      printf(" CubeControl Motor Enabled  is %d .\n",TLM1_1[5]);
      printf(" CubeSense 1 Enabled        is %d .\n",TLM1_1[6]);
      printf(" CubeSense 2 Enabled        is %d .\n",TLM1_1[7]);
      VOR_Sleep(time_1);
      printf(" CubeWheel 1 Enabled        is %d .\n",TLM2_1[0]);
      printf(" CubeWheel 2 Enabled        is %d .\n",TLM2_1[1]);
      printf(" CubeWheel 3 Enabled        is %d .\n",TLM2_1[2]);
      printf(" CubeStar Enabled           is %d .\n",TLM2_1[3]);
      printf(" GPS Receiver Enabled       is %d .\n",TLM2_1[4]);
      printf(" GPS LNA Power Enabled      is %d .\n",TLM2_1[5]);
      printf(" Motor Driver Enabled       is %d .\n",TLM2_1[6]);
      printf(" Sun is Above Local Horizon is %d .\n",TLM2_1[7]);
      VOR_Sleep(time_1);
      printf(" CubeSense 1 Communications Error       is %d .\n",TLM3_1[0]);
      printf(" CubeSense 2 Communications Error       is %d .\n",TLM3_1[1]);
      printf(" CubeControl Signal Communication Error is %d .\n",TLM3_1[2]);
      printf(" CubeControl Motor Communications Error is %d .\n",TLM3_1[3]);
      printf(" CubeWheel 1 Communications Error       is %d .\n",TLM3_1[4]);
      printf(" CubeWheel 2 Communications Error       is %d .\n",TLM3_1[5]);
      printf(" CubeWheel 3 Communications Error       is %d .\n",TLM3_1[6]);
      printf(" CubeStar Communications Error          is %d .\n",TLM3_1[7]);
      VOR_Sleep(time_1);
      printf(" Magnetometer Range Error          is %d .\n",TLM4_1[0]);
      printf(" Cam 1 SRAM Overcurrent Detected   is %d .\n",TLM4_1[1]);
      printf(" Cam 1 3V3 Overcurrent Detected    is %d .\n",TLM4_1[2]);
      printf(" Cam 1 Sensor Busy Error           is %d .\n",TLM4_1[3]);
      printf(" Cam 1 Sensor Detection Error      is %d .\n",TLM4_1[4]);
      printf(" Sun Sensor Range Error            is %d .\n",TLM4_1[5]);
      printf(" Cam 2 SRAM Overcurrent Detected   is %d .\n",TLM4_1[6]);
      printf(" Cam 2 3V3 Overcurrent Detected    is %d .\n",TLM4_1[7]);
      VOR_Sleep(time_1);
      printf(" Cam2 Sensor Busy Error            is %d .\n",TLM5_1[0]);
      printf(" Cam2 Sensor Detection Error       is %d .\n",TLM5_1[1]);
      printf(" Nadir Sensor Range Error          is %d .\n",TLM5_1[2]);
      printf(" Rate Sensor Range Error           is %d .\n",TLM5_1[3]);
      printf(" Wheel Speed Range Error           is %d .\n",TLM5_1[4]);
      printf(" Coarse Sun Sensor Error           is %d .\n",TLM5_1[5]);
      printf(" StarTracker Match Error           is %d .\n",TLM5_1[6]);
      printf(" Star Tracker Overcurrent Detected is %d .\n",TLM5_1[7]);
    }
    break;    
    
#if Detail
  case 135:
    BS_1(adc_addr->TLM[0],TLM0_1);
    printf(" ADCS Config Load Error       is %d .\n",TLM0_1[0]);
    printf(" Orbit Parameter Load Error   is %d .\n",TLM0_1[1]);
    printf(" System Config Load Error     is %d .\n",TLM0_1[2]);
    printf(" SD card initialisation Error is %d .\n",TLM0_1[3]);
    printf(" SD card read Error           is %d .\n",TLM0_1[4]);
    printf(" SD card write Error          is %d .\n",TLM0_1[5]);
    break;    
    
  case 140:
    TLM0123 = BS_32(adc_addr->TLM[0],adc_addr->TLM[1],adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45   = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    printf(" Current Unix Time is %d .\n",TLM0123);
    printf(" Milliseconds      is %d .\n",TLM45);
    break;  
    
  case 141:
    TLM0123 = BS_32(adc_addr->TLM[0],adc_addr->TLM[1],adc_addr->TLM[2],adc_addr->TLM[3]);
    printf(" Time of Event   is %d .\n",TLM0123);
    printf(" Event ID        is %d .\n",adc_addr->TLM[4]);
    printf(" Event Parameter is %d .\n",adc_addr->TLM[5]);
    break;
    
  case 142:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    printf(" TLM[]  is %d .\n",adc_addr->TLM[0]);
    printf(" TLM[]  is %d .\n",adc_addr->TLM[1]);
    printf(" TLM[]  is %d .\n",adc_addr->TLM[2]);
    printf(" TLM[]  is %d .\n",adc_addr->TLM[3]);
    printf(" SRAM 1 latchups is %d .\n",TLM01);
    printf(" SRAM 2 latchups is %d .\n",TLM23);
    break;
    
  case 143:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    printf(" Single SRAM upsets   is %d .\n",TLM01);
    printf(" Double SRAM upsets   is %d .\n",TLM23);
    printf(" Multiple SRAM upsets is %d .\n",TLM45);
    break;
#endif
    
  case 144:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    
    BS_1(adc_addr->TLM[4],TLM4_1);
    
    printf(" Telecommand counter          is %d .\n",TLM01);
    printf(" Telemetry request counter    is %d .\n",TLM23);
    printf(" Telecommand buffer overrun   is %d .\n",TLM4_1[0]);
    printf(" UART protocol error          is %d .\n",TLM4_1[1]);
    printf(" UART incomplete message      is %d .\n",TLM4_1[2]);
    printf(" I2C telemetry error          is %d .\n",TLM4_1[3]);
    printf(" I2C telecommand buffer error is %d .\n",TLM4_1[4]);
    printf(" CAN telecommand buffer error is %d .\n",TLM4_1[5]);
    break;
    
  case 147:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    X  = TLM01*0.01;
    Y  = TLM23*0.01;
    Z  = TLM45*0.01;
    printf(" Estimated X Anglar Rate is %d [-].\n",TLM01);
    printf(" Estimated Y Anglar Rate is %d [-].\n",TLM23);
    printf(" Estimated Z Anglar Rate is %d [-].\n",TLM45);
    printf(" Estimated X Anglar Rate is %5.2f [deg/s].\n",X);
    printf(" Estimated Y Anglar Rate is %5.2f [deg/s].\n",Y);
    printf(" Estimated Z Anglar Rate is %5.2f [deg/s].\n",Z);
    break;
    
  case 151:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    X   = TLM01*0.01;
    Y   = TLM23*0.01;
    Z   = TLM45*0.01;
    printf(" Magnetic field X is %d [-].\n",TLM01);
    printf(" Magnetic field Y is %d [-].\n",TLM23);
    printf(" Magnetic field Z is %d [-].\n",TLM45);
    printf(" Magnetic field X is %6.2f [uT].\n",X);
    printf(" Magnetic field Y is %6.2f [uT].\n",Y);
    printf(" Magnetic field Z is %6.2f [uT].\n",Z);
    break;
    
#if Detail
  case 152:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    X   = TLM01*0.0001;
    Y   = TLM23*0.0001;
    Z   = TLM45*0.0001;
    printf(" Coarse Sun X is %d [-].\n",TLM01);
    printf(" Coarse Sun Y is %d [-].\n",TLM23);
    printf(" Coarse Sun Z is %d [-].\n",TLM45);
    printf(" Coarse Sun X is %9.4f [-].\n",X);
    printf(" Coarse Sun Y is %9.4f [-].\n",Y);
    printf(" Coarse Sun Z is %9.4f [-].\n",Z);
    break;
    
  case 153:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    X   = TLM01*0.0001;
    Y   = TLM23*0.0001;
    Z   = TLM45*0.0001;
    printf(" Sun X is %d [-].\n",TLM01);
    printf(" Sun Y is %d [-].\n",TLM23);
    printf(" Sun Z is %d [-].\n",TLM45);
    printf(" Sun X is %9.4f [-].\n",X);
    printf(" Sun Y is %9.4f [-].\n",Y);
    printf(" Sun Z is %9.4f [-].\n",Z);
    break;
#endif
    
  case 155:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    X  = TLM01*0.01;
    Y  = TLM23*0.01;
    Z  = TLM45*0.01;
    printf(" X Angular Rate is %d [-].\n",TLM01);
    printf(" Y Angular Rate is %d [-].\n",TLM23);
    printf(" Z Angular Rate is %d [-].\n",TLM45);
    printf(" X Angular Rate is %5.2f [deg/s].\n",X);
    printf(" Y Angular Rate is %5.2f [deg/s].\n",Y);
    printf(" Z Angular Rate is %5.2f [deg/s].\n",Z);
    break;
    
  case 168:
    printf(" CSS1 is %d [-].\n",adc_addr->TLM[0]);
    printf(" CSS2 is %d [-].\n",adc_addr->TLM[1]);
    printf(" CSS3 is %d [-].\n",adc_addr->TLM[2]);
    printf(" CSS4 is %d [-].\n",adc_addr->TLM[3]);
    printf(" CSS5 is %d [-].\n",adc_addr->TLM[4]);
    printf(" CSS6 is %d [-].\n",adc_addr->TLM[5]);
    break;
    
  case 169:
    printf(" CSS7  is %d [-].\n",adc_addr->TLM[0]);
    printf(" CSS8  is %d [-].\n",adc_addr->TLM[1]);
    printf(" CSS9  is %d [-].\n",adc_addr->TLM[2]);
    printf(" CSS10 is %d [-].\n",adc_addr->TLM[3]);
    break;
    
  case 170:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    printf(" Raw MagX is %d [-].\n",TLM01);
    printf(" Raw MagY is %d [-].\n",TLM23);
    printf(" Raw MagZ is %d [-].\n",TLM45);
    break;
    
  case 172:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    V_3 = TLM01*0.48828125;
    V_5 = TLM23*0.48828125;
    V_b = TLM45*0.48828125;
    printf(" CubeControl 3V3 Current  is %d [-].\n",TLM01);
    printf(" CubeControl 5V Current   is %d [-].\n",TLM23);
    printf(" CubeControl Vbat Current is %d [-].\n",TLM45);
    printf(" CubeControl 3V3 Current  is %9f [mA].\n",V_3);
    printf(" CubeControl 5V Current   is %9f [mA].\n",V_5);
    printf(" CubeControl Vbat Current is %9f [mA].\n",V_b);
    break;
    
  case 174:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    T_m   = TLM23*0.1;
    T_rm  = TLM45*0.1;
    printf(" MCU Temperature               is %d [C].\n",TLM01);
    printf(" Magnetometer Temperature      is %d [-].\n",TLM23);
    printf(" Redu Magnetometer Temperature is %d [-].\n",TLM45);
    printf(" MCU Temperature               is %5.2d [C].\n",TLM01);
    printf(" Magnetometer Temperature      is %5.2f [C].\n",T_m);
    printf(" Redu Magnetometer Temperature is %5.2f [C].\n",T_rm);
    break;
    
  case 175:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    printf(" X-Rate sensor temperature is %d [C].\n",TLM01);
    printf(" Y-Rate sensor temperature is %d [C].\n",TLM23);
    printf(" Z-Rate sensor temperature is %d [C].\n",TLM45);
    break;
    
  case 191:    
    for(int i = 0; i < 36; i ++){
      TLM_16[i] = BS_16(adc_addr->TLM[2*i],adc_addr->TLM[2*i + 1]);  
    }
    for(int i = 0;i < 3; i ++){
      f001[i]   = TLM_16[i]*0.01;        // Magnetic Fields
      f001[i+3] = TLM_16[i+12]*0.01;     // Angular Rate
    }
    for(int i = 0;i < 3;i ++){
      f0001[i]    = TLM_16[i+3]*0.0001;  // Coarse Sun
      f0001[i+3]  = TLM_16[i+6]*0.0001;  // Sun
      f0001[i+6]  = TLM_16[i+9]*0.0001;  // Nadiar
      f0001[i+9]  = TLM_16[i+18]*0.0001; // Star1 vector body
      f0001[i+12] = TLM_16[i+21]*0.0001; // Star1 vector orbit
      f0001[i+15] = TLM_16[i+24]*0.0001; // Star2 vector body
      f0001[i+18] = TLM_16[i+27]*0.0001; // Star2 vector orbit
      f0001[i+21] = TLM_16[i+30]*0.0001; // Star3 vector body
      f0001[i+24] = TLM_16[i+33]*0.0001; // Star3 vector orbit
    }
    
    printf(" Magnetic Field X is %9.2f [uT].\n",f001[0]);
    printf(" Magnetic Field Y is %9.2f [uT].\n",f001[1]);
    printf(" Magnetic Field Z is %9.2f [uT].\n",f001[2]);
    printf(" Coarse Sun X     is %9.4f [-].\n",f0001[0]);
    printf(" Coarse Sun Y     is %9.4f [-].\n",f0001[1]);
    printf(" Coarse Sun Z     is %9.4f [-].\n",f0001[2]);
    printf(" Sun X            is %9.4f [-].\n",f0001[3]);
    printf(" Sun Y            is %9.4f [-].\n",f0001[4]);
    VOR_Sleep(time_1);
    printf(" Sun Z            is %9.4f [-].\n",f0001[5]);
    printf(" Nadiar X         is %9.4f [-].\n",f0001[6]);
    printf(" Nadiar Y         is %9.4f [-].\n",f0001[7]);
    printf(" Nadiar Z         is %9.4f [-].\n",f0001[8]);
    printf(" X Angular Rate   is %9.2f [deg/s].\n",f001[3]);
    printf(" Y Angular Rate   is %9.2f [deg/s].\n",f001[4]);
    printf(" Z Angular Rate   is %9.2f [deg/s].\n",f001[5]);
    printf(" X Wheel Speed    is %9d [rpm].\n",TLM_16[15]);
    VOR_Sleep(time_1);
    printf(" Y Wheel Speed    is %9d [rpm].\n",TLM_16[16]);
    printf(" Z Wheel Speed    is %9d [rpm].\n",TLM_16[17]);
    printf(" Star1 BX         is %9.4f [-].\n",f0001[9]);
    printf(" Star1 BY         is %9.4f [-].\n",f0001[10]);
    printf(" Star1 BZ         is %9.4f [-].\n",f0001[11]);
    printf(" Star1 OX         is %9.4f [-].\n",f0001[12]);
    printf(" Star1 OY         is %9.4f [-].\n",f0001[13]);
    printf(" Star1 OZ         is %9.4f [-].\n",f0001[14]);
    VOR_Sleep(time_1);
    printf(" Star2 BX         is %9.4f [-].\n",f0001[15]);
    printf(" Star2 BY         is %9.4f [-].\n",f0001[16]);
    printf(" Star2 BZ         is %9.4f [-].\n",f0001[17]);
    printf(" Star2 OX         is %9.4f [-].\n",f0001[18]);
    printf(" Star2 OY         is %9.4f [-].\n",f0001[19]);
    printf(" Star2 OZ         is %9.4f [-].\n",f0001[20]);
    printf(" Star3 BX         is %9.4f [-].\n",f0001[21]);
    printf(" Star3 BY         is %9.4f [-].\n",f0001[22]);
    VOR_Sleep(time_1);
    printf(" Star3 BZ         is %9.4f [-].\n",f0001[23]);
    printf(" Star3 OX         is %9.4f [-].\n",f0001[24]);
    printf(" Star3 OY         is %9.4f [-].\n",f0001[25]);
    printf(" Star3 OZ         is %9.4f [-].\n",f0001[26]);
    break;
    
  case 195:
    for(int i = 0; i < 19; i ++){
      TLM_16[i] = BS_16(adc_addr->TLM[2*i],adc_addr->TLM[2*i + 1]);  
    }
    
    fTLM_16[0]   = TLM_16[0]*0.1;
    fTLM_16[1] = TLM_16[1]*0.1;
    fTLM_16[2] = TLM_16[2]*0.1;
    fTLM_16[3] = TLM_16[3]*0.1; 
    fTLM_16[4] = TLM_16[4]*0.48828125;
    fTLM_16[5] = TLM_16[5]*0.48828125;
    fTLM_16[6] = TLM_16[6]*0.48828125;
    
    fTLM_16[7] = TLM_16[7]*0.01;
    fTLM_16[8] = TLM_16[8]*0.01;
    fTLM_16[9] = TLM_16[9]*0.01;
    fTLM_16[10] = TLM_16[10]*0.01;
    fTLM_16[11] = TLM_16[11]*0.1;
    fTLM_16[12] = TLM_16[12]*0.01;
    
    fTLM_16[14] = TLM_16[14]*0.1;
    fTLM_16[15] = TLM_16[15]*0.1;
    
    printf(" Raw data\n");
    printf(" CubeSense1 3V3 Current       is %d .\n",TLM_16[0]);
    printf(" CubeSense1 Cam SRAM Current  is %d .\n",TLM_16[1]);
    printf(" CubeSense2 3V3 Current       is %d .\n",TLM_16[2]);
    printf(" CubeSense2 Cam SRAM Current  is %d .\n",TLM_16[3]);
    printf(" CubeControl 3V3 Current      is %d .\n",TLM_16[4]);
    printf(" CubeControl 5V Current       is %d .\n",TLM_16[5]);
    printf(" CubeControl Vbat Current     is %d .\n",TLM_16[6]);
    VOR_Sleep(time_1);
    printf(" Wheel1Current                is %d .\n",TLM_16[7]);
    printf(" Wheel2Current                is %d .\n",TLM_16[8]);
    printf(" Wheel3Current                is %d .\n",TLM_16[9]);
    printf(" CubeStarCurrent              is %d .\n",TLM_16[10]);
    printf(" Magnetorquer Current         is %d .\n",TLM_16[11]);
    printf(" CubeStar MCU temperature     is %d .\n",TLM_16[12]);
    printf(" MCU Temperature              is %d .\n",TLM_16[13]);
    printf(" Magnetometer Temperature     is %d .\n",TLM_16[14]);
    VOR_Sleep(time_1);
    printf(" Sec Magnetometer Temperature is %d .\n",TLM_16[15]);
    printf(" X-Rate Sensor Temperature    is %d .\n",TLM_16[16]);
    printf(" Y-Rate Sensor Temperature    is %d .\n",TLM_16[17]);
    printf(" Z-Rate Sensor Temperature    is %d .\n",TLM_16[18]);
    VOR_Sleep(time_1);
    printf(" Data\n");
    printf(" CubeSense1 3V3 Current       is %.1f .\n",fTLM_16[0]);
    printf(" CubeSense1 Cam SRAM Current  is %.1f .\n",fTLM_16[1]);
    printf(" CubeSense2 3V3 Current       is %.1f .\n",fTLM_16[2]);
    printf(" CubeSense2 Cam SRAM Current  is %.1f .\n",fTLM_16[3]);
    printf(" CubeControl 3V3 Current      is %.8f .\n",fTLM_16[4]);
    printf(" CubeControl 5V Current       is %.8f .\n",fTLM_16[5]);
    printf(" CubeControl Vbat Current     is %.8f .\n",fTLM_16[6]);
    VOR_Sleep(time_1);
    printf(" Wheel1Current                is %.2f .\n",fTLM_16[7] );
    printf(" Wheel2Current                is %.2f .\n",fTLM_16[8] );
    printf(" Wheel3Current                is %.2f .\n",fTLM_16[9] );
    printf(" CubeStarCurrent              is %.2f .\n",fTLM_16[10]);
    printf(" Magnetorquer Current         is %.2f .\n",fTLM_16[11]);
    printf(" CubeStar MCU temperature     is %.2f .\n",fTLM_16[12]);
    printf(" MCU Temperature              is %d .\n",TLM_16[13]);
    printf(" Magnetometer Temperature     is %.1f .\n",fTLM_16[14]);
    VOR_Sleep(time_1);
    printf(" Sec Magnetometer Temperature is %.1f .\n",fTLM_16[15]);
    printf(" X-Rate Sensor Temperature    is %d .\n",TLM_16[16]);
    printf(" Y-Rate Sensor Temperature    is %d .\n",TLM_16[17]);
    printf(" Z-Rate Sensor Temperature    is %d .\n",TLM_16[18]);
    break;
    
#if Detail
  case 196:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    TLM67 = BS_16(adc_addr->TLM[6],adc_addr->TLM[7]);
    printf(" Time to Perform ADCS Update     is %d .\n",TLM01);
    printf(" Time to Perform Sen/Act         is %d .\n",TLM23);
    printf(" Time to Execute SGP4 Propagator is %d .\n",TLM45);
    printf(" Time to Execute IGRF Model      is %d .\n",TLM67);
    break;
#endif
    
  case 197:
    BS_2(adc_addr->TLM[0],TLM0_1);
    BS_2(adc_addr->TLM[1],TLM1_1);
    BS_2(adc_addr->TLM[2],TLM2_1);
    
    printf(" CubeControl Signal Power Selection is %d .\n",TLM0_1[0]);
    printf(" CubeControl Motor Power Selection  is %d .\n",TLM0_1[1]);
    printf(" CubeSense 1 Power Selection        is %d .\n",TLM0_1[2]);
    printf(" CubeSense 2 Power Selection        is %d .\n",TLM0_1[3]);
    printf(" CubeStarPower Power Selection      is %d .\n",TLM1_1[0]);
    printf(" CubeWheel 1 Power Power Selection  is %d .\n",TLM1_1[1]);
    printf(" CubeWheel 2 Power Power Selection  is %d .\n",TLM1_1[2]);
    printf(" CubeWheel 3 Power Power Selection  is %d .\n",TLM1_1[3]);
    VOR_Sleep(time_1);
    printf(" Motor Power is %d .\n",TLM2_1[0]);
    printf(" GPS Power   is %d .\n",TLM2_1[1]);
    break;
    
#if Detail
  case 198:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    CSC   = TLM01*0.01;
    MC    = TLM23*0.1;
    CSMT  = TLM45*0.01;
    printf(" CubeStarCurrent          is %d [-].\n",TLM01);
    printf(" Magnetorquer Current     is %d [-].\n",TLM23);
    printf(" CubeStar MCU temperature is %d [-].\n",TLM45);
    printf(" CubeStarCurrent          is %4.2f [mA].\n",CSC);
    printf(" Magnetorquer Current     is %4.2f [mA].\n",MC);
    printf(" CubeStar MCU temperature is %4.2f [C].\n",CSMT);
    break;
    
  case 215:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    printf(" Sec MagX is %d [-].\n",TLM01);
    printf(" Sec MagY is %d [-].\n",TLM23);
    printf(" Sec MagZ is %d [-].\n",TLM45);
    break;

  case 216:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    TLM23 = BS_16(adc_addr->TLM[2],adc_addr->TLM[3]);
    TLM45 = BS_16(adc_addr->TLM[4],adc_addr->TLM[5]);
    printf(" Raw RateX is %d [-].\n",TLM01);
    printf(" Raw RateY is %d [-].\n",TLM23);
    printf(" Raw RateZ is %d [-].\n",TLM45);
    break;
    
  case 220:
    TLM01 = BS_16(adc_addr->TLM[0],adc_addr->TLM[1]);
    printf(" Time Since Iteration Start is %d .\n",TLM01);
    printf(" Current Execution Point    is %d .\n",adc_addr->TLM[2]);
    break;
    
  case 232:
    TLM2345 = BS_32(adc_addr->TLM[2],adc_addr->TLM[3],adc_addr->TLM[4],adc_addr->TLM[5]);
    TLM67 = BS_16(adc_addr->TLM[6],adc_addr->TLM[7]);
    printf(" Boot Program Index is %d.\n",adc_addr->TLM[0]);
    printf(" Busy state         is %d .\n",adc_addr->TLM[1]);
    printf(" File Size          is %d byte .\n",TLM2345);
    printf(" Checksum           is %d .\n",TLM67);
    break;
#endif
    
  case 240:
    printf(" Last TC ID               is %d .\n",adc_addr->TLM[0]);
    printf(" Processed flag           is %d .\n",adc_addr->TLM[1]);
    printf(" TC error status          is %d .\n",adc_addr->TLM[2]);
    printf(" TC parameter error index is %d .\n",adc_addr->TLM[3]);
    break;
    
  default:
    
    break; 
  }
}
void HK_print(int n, ADC_t* adc_addr){
  
  uint8_t HK_normal_size  = 0x48;       // -> 72 (10 decimal number)
  uint8_t HK_IniRate_size = 0x09;

#if Detail
  uint8_t HK_Detumb_size  = 0x0C;
  uint8_t HK_Pitch_size   = 0x0C;
  uint8_t HK_Wheel_size   = 0x0E;
#endif
  
  int i;
  
  switch(n){
  case 1:       // normal HK
    printf("HK_normal[] = [");
    for (i = 0; i < HK_normal_size; i++){
      printf(" %6d",adc_addr->HK_normal[i]);
      if(i%12 == 11){
        printf(";\n               ");
      }
    }
    printf("\r- Finish -\n");
    break;
    
  case 2:       // detail HK
    HK_trans(adc_addr);
    printf("adc_addr->HK_normal[] = [");
    for (i = 0; i < HK_normal_size; i++){
      printf(" %9.3f",adc_addr->HK_detail[i]);
      if(i%12 == 11){
        printf(";\n            ");
      } 
    } 
    printf("\r");
    break;
    
  case 3:       // Initial Rate Estimation HK
    printf("HK_IniRate[] = [");
    for (i = 0; i < HK_IniRate_size; i++){
      printf(" %6d",adc_addr->HK_IniRate[i]);
      if(i%12 == 11){
        printf(";\n                ");
      }
    }
    printf("\r- Finish -\n");
    break;
    
#if Detail
  case 4:       // Detumbling HK
    printf("HK_Detumb[] = [");
    for (i = 0; i < HK_Detumb_size; i++){
      printf(" %6d",adc_addr->HK_Detumbling[i]);
      if(i%12 == 11){
        printf(";\n              ");
      }
    }
    printf("\r- Finish -\n");
    break;

  case 5:       // Pitch estimation HK
    printf("HK_Pitch[] = [");
    for (i = 0; i < HK_Pitch_size; i++){
      printf(" %6d",adc_addr->HK_Pitch[i]);
      if(i%12 == 11){
        printf(";\n             ");
      }
    }
    printf("\r- Finish -\n");
    break;
    
  case 6:       // Y-Wheel ramp up test HK
    printf("HK_wheel[] = [");
    for (i = 0; i < HK_Wheel_size; i++){
      printf(" %6d",adc_addr->HK_Wheel[i]);
      if(i%12 == 11){
        printf(";\n             ");
      }
    }
    printf("\r- Finish -\n");
    break;
#endif
  default:
    
    break;    
  }
  
  
}
uint8_t eclipse(ADC_t* adc_addr){
  
  /**********************************************************************/
  /* output: uint8_t ecli                                               */
  /* - outline -                                                        */
  /*  Shade or sun exposure determination for each surface using CSS    */
  /* - example -                                                        */
  /* ecli =                                                             */
  /*   [0000 0001] -> +x is exposed the sunlight                        */
  /*   [0000 0010] -> -x is exposed the sunlight                        */
  /*   [0000 0100] -> +y is exposed the sunlight                        */
  /*   [0000 1000] -> -y is exposed the sunlight                        */
  /*   [0001 0000] -> -z is exposed the sunlight                        */
  /*   [0001 1111] -> all surface are exposed the sunlight              */
  /**********************************************************************/
  
  int ID;
  uint8_t CSS[64], ecli, x_posi, x_minu, y_posi, y_minu, z_minu, tmp = 0;
  ID = 168;
  I2CB_CubeComputer(ID, adc_addr);
  for(int i=0;i<6;i++){
    CSS[i] = adc_addr->TLM[i];    
  }
  ID = 169;
  I2CB_CubeComputer(ID, adc_addr);
  for(int i=0;i<6;i++){
    CSS[i+6] = adc_addr->TLM[i];    
  }
  
  if(50 < CSS[0] && 50 < CSS[1]){
    x_posi = 0x01;
  }else{
    x_posi = 0x00;
  }
  
  if(50 < CSS[2] && 50 < CSS[3]){
    x_minu = 0x02;
  }else{
    x_minu = 0x00;
  }
  
  if(50 < CSS[4] && 50 < CSS[5]){
    y_posi = 0x04;
  }else{
    y_posi = 0x00;
  }
  
  if(50 < CSS[6] && 50 < CSS[7]){
    y_minu = 0x08;
  }else{
    y_minu = 0x00;
  }
  
  if(50 < CSS[8] && 50 < CSS[9]){
    z_minu = 0x10;
  }else{
    z_minu = 0x00;
  }
  
  tmp = tmp | x_posi; 
  tmp = tmp | x_minu;
  tmp = tmp | y_posi;
  tmp = tmp | y_minu;
  ecli = tmp | z_minu;
  
  return ecli;
  
}

/* I2C */
void I2CA_CubeComputer(int ID, ADC_t* adc_addr){
  adc_addr->TC[0] = ID;
  uint8_t TC_WORDS, TLM_WORDS;
  
  /* Telecommand */
  if (0 < ID && ID < 128){
    TC_WORDS = TC_Byte(ID);
    if(TC_WORDS != 0){
      I2CA_TC(adc_addr->TC,TC_WORDS);
    }
    /* Telemetry */
  }else if (128 <= ID && ID <= 255){
    TLM_WORDS = TLM_Byte(ID);      // Get RX data size
    if(TLM_WORDS != 0){
      I2CA_TLM(adc_addr->TLM,adc_addr->TC,TLM_WORDS);   // I2C communication
    }
    /* Error */
  }else{
    printf("\nNo ID\n\n");
  }
}
void I2CB_CubeComputer(int ID, ADC_t* adc_addr){
  adc_addr->TC[0] = ID;
  uint8_t TC_WORDS, TLM_WORDS;
  
  /* Telecommand */
  if (0 < ID && ID < 128){
    TC_WORDS = TC_Byte(ID);
    if(TC_WORDS != 0){
      I2CB_TC(adc_addr->TC,TC_WORDS);
    }
    /* Telemetry */
  }else if (128 <= ID && ID <= 255){
    TLM_WORDS = TLM_Byte(ID);      // Get RX data size
    if(TLM_WORDS != 0){  
      I2CB_TLM(adc_addr->TLM,adc_addr->TC,TLM_WORDS);   // I2C communication
    }
    /* Error */
  }else{
    printf("\nNo ID\n\n");
  }
}
/* Telecommand */
uint8_t TC_Byte(int ID){         // Get TC data size
  uint8_t TC_WORDS;
  if(ID == 6 || ID == 101){
    TC_WORDS = 1;
  }else if(ID == 10 || ID == 14 || ID == 100 || ID == 102){
    TC_WORDS = 2;
  }else if(ID == 11 || ID == 13){
    TC_WORDS = 4;
  }else if(ID == 16){
    TC_WORDS = 7;
  }else if(ID == 45){
    TC_WORDS = 64;
  }else{
    TC_WORDS = 0;
  }
  return TC_WORDS;
}
/* Telemetry */
uint8_t TLM_Byte(int ID){        // Get TLM data size
  uint8_t TLM_WORDS;
  if(ID == 191){
    TLM_WORDS = 72;
  }else if(ID == 195){
    TLM_WORDS = 38;
  }else if(ID == 128 || ID == 196 || ID == 232){
    TLM_WORDS = 8;
  }else if(ID == 129 || ID == 132 || ID == 140 || ID == 141 || ID == 142 || ID == 143 || ID == 144 || ID == 146 || ID == 147 || ID == 148 || ID == 149){
    TLM_WORDS = 6;
  }else if(ID == 150 || ID == 151 || ID == 152 || ID == 153 || ID == 155 || ID == 156 || ID == 157 || ID == 168 || ID == 169){
    TLM_WORDS = 6;
  }else if(ID == 170 || ID == 172 || ID == 174 || ID == 175 || ID == 198 || ID == 215 || ID == 216){
    TLM_WORDS = 6;
  }else if(ID == 240){
    TLM_WORDS = 4;
  }else if(ID == 197 || ID == 220){
    TLM_WORDS = 3;
  }else if(ID == 130){
    TLM_WORDS = 2;
  }else if(ID == 135){
    TLM_WORDS = 1;
  }else{
    TLM_WORDS = 0;
    printf("\nPlease set new TLM byte\n\n");
  }
  return TLM_WORDS;
}


/* Bit shift */
int32_t BS_32(int32_t a,int32_t b,int32_t c,int32_t d){                 // 32 bit display (LSB,-,-,MSB) 
  int16_t tmp16ab;
  int16_t tmp16cd;
  int32_t tmp32;
  
  tmp16ab = b << 0x8;
  tmp16ab = tmp16ab | a;
  
  tmp16cd = d << 0x8;
  tmp16cd = tmp16cd | c;
  
  tmp32   = tmp16cd << 0x10;
  tmp32   = tmp32 | tmp16ab;
  return tmp32;
}
int16_t BS_16(int32_t a,int32_t b){                                     // 16 bit display (LSB , MSB)
  int16_t tmp;
  
  tmp = b << 0x8;
  tmp = tmp | a;
  return tmp;
}
void BS_4(int32_t a,int8_t *b){                               // 4 bit display
  b[0] = a & 0xF;
  b[1] = a >> 0x4;
}
void BS_2(int32_t a,int8_t *b){       // 2 bit display
  b[0] = a & 0x3;
  b[1] = a >> 0x2  & 0x3;
  b[2] = a >> 0x4  & 0x3;
  b[3] = a >> 0x6  & 0x3;
}
void BS_1(int32_t a,int8_t *b){// 1 bit display
  b[0] = a & 1;
  b[1] = a >> 0x1  & 0x1;
  b[2] = a >> 0x2  & 0x1;
  b[3] = a >> 0x3  & 0x1;
  b[4] = a >> 0x4  & 0x1;    
  b[5] = a >> 0x5  & 0x1;
  b[6] = a >> 0x6  & 0x1;    
  b[7] = a >> 0x7  & 0x1;
}