#include "EcatDeviceAPI.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <fstream>
using namespace std;

#define Threshold 2000000000
class FT_data{
    public:
        FT_data() {}
        explicit FT_data(uint16_t deviceno,uint16_t slaveno);
        void set_bias();
        float FT(const string ft);
    private:
        uint16_t DeviceNo,SlaveNo;
        uint32_t CountPerF,CountPerT,FUnit,TUnit;
        uint32_t zero[6];
        uint32_t FTCount[6];
        uint32_t Abort;
        uint32_t max_count=4294967295;
    
};
FT_data::FT_data(uint16_t deviceno,uint16_t slaveno){
    DeviceNo=deviceno;
    SlaveNo=slaveno;
    Abort=0;
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x2021, 0x37, 4, &CountPerF, &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x2021, 0x38, 4, &CountPerT, &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x2021, 0x2f, 2, &FUnit, &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x2021, 0x30, 2, &TUnit, &Abort);
    cout<<"Counts Per Force  "<<CountPerF<<endl;
    cout<<"Counts Per Torque  "<<CountPerT<<endl;
    cout<<"The Force units are  "<<( (FUnit=1)?"N":"Not sure")<<endl;
    cout<<"The Torque units are  "<<( (TUnit=2)?"Nm":"Not sure")<<endl;
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x6000, 0x01, 4, &FTCount[0], &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x6000, 0x02, 4, &FTCount[1], &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x6000, 0x03, 4, &FTCount[2], &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x6000, 0x04, 4, &FTCount[3], &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x6000, 0x05, 4, &FTCount[4], &Abort);
    ECAT_GetSlaveSdoObject(deviceno, slaveno, 0x6000, 0x06, 4, &FTCount[5], &Abort);
    zero[0]=FTCount[0];
    zero[1]=FTCount[1];
    zero[2]=FTCount[2];
    zero[3]=FTCount[3];
    zero[4]=FTCount[4];
    zero[5]=FTCount[5];
}
void FT_data::set_bias(){
    uint16_t OffsetByte, DataSize;
    uint8_t Data[RW_PDO_DATA_SIZE_MAX];
    DataSize = 4; //4 bytes
    Data[0] = 0x41;
    Data[1] = 0x00;
    Data[2] = 0x00;
    Data[3] = 0x02;
    ECAT_SetSlaveRxPdoData(DeviceNo, SlaveNo, 0, DataSize, Data);
    Data[0] = 0x40;
    usleep(1000); //0.001s
    ECAT_SetSlaveRxPdoData(DeviceNo, SlaveNo, 0, DataSize, Data);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x01, 4, &zero[0], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x02, 4, &zero[1], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x03, 4, &zero[2], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x04, 4, &zero[3], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x05, 4, &zero[4], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x06, 4, &zero[5], &Abort);
}
int compute(uint32_t FTCount,uint32_t zero,uint32_t FT){
    uint32_t max_count=4294967295;                
    uint32_t CountPerF=1000000;
 
    return FT;
}
float FT_data::FT(const string ft){
    float FT_data[6];
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x01, 4, &FTCount[0], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x02, 4, &FTCount[1], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x03, 4, &FTCount[2], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x04, 4, &FTCount[3], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x05, 4, &FTCount[4], &Abort);
    ECAT_GetSlaveSdoObject(DeviceNo, SlaveNo, 0x6000, 0x06, 4, &FTCount[5], &Abort);
    for(int i=0;i<6;i++){
       if((FTCount[i])<Threshold && zero[i]<Threshold){
            FT_data[i]=(float)(zero[i]-FTCount[i])/CountPerF;
        }
        else if(FTCount[i]>Threshold && zero[i]<Threshold){
            FT_data[i]=(float)((max_count-FTCount[i])+zero[i])/CountPerF;
        }
        else if(FTCount[i]<Threshold && zero[i]>Threshold){
            FT_data[i]=(float)((zero[i]-max_count)-FTCount[i])/CountPerF;
        }
        else if(FTCount[i]>Threshold && zero[i]>Threshold){
            FT_data[i]=(float)(zero[i]-FTCount[i])/CountPerF;
        }
        else
            cout<<"error"<<endl;
        if(FT_data[i]>4000){
            FT_data[i]=0;
        }
    }


    if(ft=="Fx")
        return FT_data[0];
    else if(ft=="Fy")
        return FT_data[1];
    else if(ft=="Fz")
        return FT_data[2];
    else if(ft=="Tx")
        return FT_data[3];
    else if(ft=="Ty")
        return FT_data[4];
    else if(ft=="Tz")
        return FT_data[5];
    else{
        cout<<"error input"<<endl;
        return 0;
    }
}

int main(void) {

    int32_t ret;
    uint16_t DeviceCnt,DeviceNo;
    uint8_t CardID[CARD_DEVICE_NO_MAX];

    ECAT_GetDeviceCnt(&DeviceCnt, CardID);
    DeviceNo=CardID[0];
    ECAT_OpenDevice(DeviceNo);

    uint8_t Source, Logic, Enable, ServoOff;
    uint16_t SlaveNo, BitNo;
    ECAT_GetDeviceEmg(DeviceNo, &Source, &Enable, &Logic, &SlaveNo, &BitNo, &ServoOff);

    cout<<DeviceNo<<endl;
    cout<<SlaveNo<<endl;
    
    char Slavename[MAX_SLAVE_NAME_LENGTH];
    uint8_t AcatState; 
    uint16_t Alias;
    uint32_t ProductCode, VendorID, RevisionNo, SerialNo, SlaveType;
    ECAT_GetSlaveInfo(DeviceNo, SlaveNo, &Alias, &ProductCode,
        &VendorID, &RevisionNo, &SerialNo,&AcatState, &SlaveType,Slavename);

    uint16_t NetworkInfoNo = 0;
    uint8_t EnumCycleTime = DEV_OP_CYCLE_TIME_1MS;
    uint32_t WcErrCnt=0;
    ECAT_StartDeviceOpTask(DeviceNo, NetworkInfoNo, EnumCycleTime, WcErrCnt);
    

    FT_data FORCE( DeviceNo, SlaveNo);
    FORCE.set_bias();
    int i=0;
    // float a=10;
    // float b;
    // b=a/3;
    // cout<<b<<endl;
    ifstream in("datafile");
    ofstream out("datafile");
    while (1){
        usleep(1*100);
        i++;
        // cout<<FORCE.FT("Fx")<<" ";
        // cout<<FORCE.FT("Fy")<<" ";
        cout<<FORCE.FT("Fz")<<" ";
        // cout<<FORCE.FT("Tx")<<" ";
        // cout<<FORCE.FT("Ty")<<" ";
        // cout<<FORCE.FT("Tz")<<" ";
        cout<<endl;
        // out<<FORCE.FT("Fz")<<endl;

    }
    

    
   
    ECAT_StopDeviceOpTask(DeviceNo);
    ECAT_CloseDevice(DeviceNo);
  
}