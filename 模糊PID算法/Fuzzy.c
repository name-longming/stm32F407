#include "include.h"
#define rank 7
//      //e -3 -2 -1  0  1  2  3   //  ec
//          {0, 5, 6, 3, 0, 0, 0}, // -3
//          {0, 4, 5, 3, 0, 0, 0}, // -2
//          {0, 0, 2, 3, 0, 0, 0}, // -1
//          {0, 0, 0, 0, 0, 0, 0}, //  0
//          {0, 0, 0, 3, 2, 0, 0}, //  1
//          {0, 0, 0, 3, 5, 4, 0}, //  2
//          {0, 0, 0, 3, 6, 5, 0}  //  3
/*
int rule_kp[7][7]=    //偏差大变化小，给最大
{
  //ec 0 1 2 3 4 5 6  //e
      {2,2,1,1,1,0,0},//0    3,2,2,1,1,1,0
      {3,3,2,1,1,1,0},//1    2,2,1,1,1,0,0 
      {4,3,3,2,2,1,1},//2    2,2,1,1,1,0,0 
      {4,4,3,3,2,1,1},//3    4,3,2,1,1,1,0 
      {4,4,4,3,2,2,1},//4
      {5,5,5,4,4,3,2},//5
      {6,6,5,5,4,3,3} //6x
};

int rule_kd[7][7]=    //偏差小变化大，给最大
{
  //ec 0 1 2 3 4 5 6  //e
      {3,3,4,5,5,6,6},//0
      {3,3,4,4,5,5,5},//1
      {2,3,3,4,5,5,5},//2
      {1,2,2,3,3,4,5},//3
      {1,1,2,2,3,3,3},//4
      {0,1,1,1,2,2,3},//5
      {0,0,1,1,1,2,2} //6
};*/
//P3.8  D2.8
float Fuzzy1(float E,float D)//面积法 两输入 一输出
{
    int En=0,Dn=0;
    float EFP[7]={-65,-40,-20,0,20,40,65};//40-45
    float DFP[7]={-45,-30,-15,0,15,30,-45};//18-20
    float UFP[7]={0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.08};
    float EF[2]={0}, DF[2]={0},UF[7]={0},U=0;
    //偏差小，且快速增大，给最大输出，区分正负
    int rule[7][7]={
      //e -3 -2 -1  0  1  2  3   // ec
          {3, 4, 5, 0, 5, 4, 3}, // -3
          {4, 4, 3, 0, 3, 4, 4}, // -2
          {5, 4, 2, 0, 2, 4, 5}, // -1
          {6, 4, 2, 0, 2, 4, 6}, //  0
          {5, 4, 2, 0, 2, 4, 5}, //  1
          {4, 4, 3, 0, 3, 4, 4}, //  2
          {3, 4, 5, 0, 5, 4, 3}  //  3
    };
  /*-----E隶属函数描述-----*/
  if(E < EFP[0]){ EF[0] =1.0; En= 0; }
  else if(E < EFP[1]){ EF[0] = (EFP[1]-E)/(EFP[1]-EFP[0]); En=0;}
  else if(E < EFP[2]){ EF[0] = (EFP[2]-E)/(EFP[2]-EFP[1]); En=1;}
  else if(E < EFP[3]){ EF[0] = (EFP[3]-E)/(EFP[3]-EFP[2]); En=2;}
  else if(E < EFP[4]){ EF[0] = (EFP[4]-E)/(EFP[4]-EFP[3]); En=3;}
  else if(E < EFP[5]){ EF[0] = (EFP[5]-E)/(EFP[5]-EFP[4]); En=4;}
  else if(E < EFP[6]){ EF[0] = (EFP[6]-E)/(EFP[6]-EFP[5]); En=5;}
  else	{EF[0] =1;En=6;}
  EF[1] = 1 - EF[0]; 
  /*-----D隶属函数描述-----*/
  if(D <= DFP[0]){ DF[0] =1.0;  Dn = 0;}
  else if(D < DFP[1]){ DF[0] = (DFP[1]-D)/(DFP[1]-DFP[0]); Dn=0;}
  else if(D < DFP[2]){ DF[0] = (DFP[2]-D)/(DFP[2]-DFP[1]); Dn=1;}
  else if(D < DFP[3]){ DF[0] = (DFP[3]-D)/(DFP[3]-DFP[2]); Dn=2;}
  else if(D < DFP[4]){ DF[0] = (DFP[4]-D)/(DFP[4]-DFP[3]); Dn=3;}
  else if(D < DFP[5]){ DF[0] = (DFP[5]-D)/(DFP[5]-DFP[4]); Dn=4;}
  else if(D < DFP[6]){ DF[0] = (DFP[6]-D)/(DFP[6]-DFP[5]); Dn=5;}
  else	{DF[0] =1;Dn=6;}
  DF[1] = 1 - DF[0];
  /*查询规则表*/  
  if(En<(rank-1) && Dn<(rank-1))        //都没到边缘
  {
    UF[rule[Dn][En]]    += EF[0]*DF[0];
    UF[rule[Dn][En+1]]  += EF[1]*DF[0];	
    UF[rule[Dn+1][En]]  += EF[0]*DF[1];
    UF[rule[Dn+1][En+1]]+= EF[1]*DF[1];
  }
  else if(En==(rank-1) && Dn<(rank-1))  // E到边缘
  {
    UF[rule[Dn][En]]  += EF[0]*DF[0];                
    UF[rule[Dn+1][En]]+= EF[0]*DF[1];
  }
  else if(En<(rank-1) && Dn==(rank-1))  // D达到边缘
  {
    UF[rule[Dn][En]]  += EF[0]*DF[0];	
    UF[rule[Dn][En+1]]+= EF[1]*DF[0];
  }
  else				       //同时到边缘
  {
    UF[rule[Dn][En]] += EF[0]*DF[0];		
  }
  //解模糊
  for(int i=0;i<rank;i++)   U+=UF[i]* UFP[i];
  return U;
}

float Fuzzy2(float E,float D)//面积法 两输入 一输出
{
    int En=0,Dn=0;
    float EFP[7]={-80,-50,-20,0,20,50,80};//40-45
    float DFP[7]={-45,-30,-15,0,15,30,-45};//18-20
    float UFP[7]={ 3.5, 3.7, 3.9, 4.2, 4.5, 4.8, 5};    //3.0, 3.5, 3.9, 4.2, 4.5, 4.8, 5
    float EF[2]={0}, DF[2]={0},UF[7]={0},U=0;
    int rule[7][7]={
        {3,3,2,1,1,0,0},
        {3,3,3,2,1,1,0},
        {5,4,4,3,2,1,1},
        {5,5,5,3,3,2,1},
        {6,5,5,4,3,2,2},
        {6,5,5,5,3,3,2}
    //e -3 -2 -1  0  1  2  3   //  ec
//        {0, 5, 6, 3, 0, 0, 0}, // -3
//        {0, 4, 5, 3, 0, 0, 0}, // -2
//        {0, 0, 2, 3, 0, 0, 0}, // -1
//        {0, 0, 0, 0, 0, 0, 0}, //  0
//        {0, 0, 0, 3, 2, 0, 0}, //  1
//        {0, 0, 0, 3, 5, 4, 0}, //  2
//        {0, 0, 0, 3, 6, 5, 0}  //  3
    };
  /*-----E隶属函数描述-----*/
  if(E < EFP[0]){ EF[0] =1.0; En= 0; }
  else if(E < EFP[1]){ EF[0] = (EFP[1]-E)/(EFP[1]-EFP[0]); En=0;}
  else if(E < EFP[2]){ EF[0] = (EFP[2]-E)/(EFP[2]-EFP[1]); En=1;}
  else if(E < EFP[3]){ EF[0] = (EFP[3]-E)/(EFP[3]-EFP[2]); En=2;}
  else if(E < EFP[4]){ EF[0] = (EFP[4]-E)/(EFP[4]-EFP[3]); En=3;}
  else if(E < EFP[5]){ EF[0] = (EFP[5]-E)/(EFP[5]-EFP[4]); En=4;}
  else if(E < EFP[6]){ EF[0] = (EFP[6]-E)/(EFP[6]-EFP[5]); En=5;}
  else	{EF[0] =1;En=6;}
  EF[1] = 1 - EF[0]; 
  /*-----D隶属函数描述-----*/
  if(D <= DFP[0]){ DF[0] =1.0;  Dn = 0;}
  else if(D < DFP[1]){ DF[0] = (DFP[1]-D)/(DFP[1]-DFP[0]); Dn=0;}
  else if(D < DFP[2]){ DF[0] = (DFP[2]-D)/(DFP[2]-DFP[1]); Dn=1;}
  else if(D < DFP[3]){ DF[0] = (DFP[3]-D)/(DFP[3]-DFP[2]); Dn=2;}
  else if(D < DFP[4]){ DF[0] = (DFP[4]-D)/(DFP[4]-DFP[3]); Dn=3;}
  else if(D < DFP[5]){ DF[0] = (DFP[5]-D)/(DFP[5]-DFP[4]); Dn=4;}
  else if(D < DFP[6]){ DF[0] = (DFP[6]-D)/(DFP[6]-DFP[5]); Dn=5;}
  else	{DF[0] =1;Dn=6;}
  DF[1] = 1 - DF[0];
  /*查询规则表*/  
  if(En<(rank-1) && Dn<(rank-1))        //都没到边缘
  {
    UF[rule[Dn][En]]    += EF[0]*DF[0];
    UF[rule[Dn][En+1]]  += EF[1]*DF[0];	
    UF[rule[Dn+1][En]]  += EF[0]*DF[1];
    UF[rule[Dn+1][En+1]]+= EF[1]*DF[1];
  }
  else if(En==(rank-1) && Dn<(rank-1))  // E到边缘
  {
    UF[rule[Dn][En]]  += EF[0]*DF[0];                
    UF[rule[Dn+1][En]]+= EF[0]*DF[1];
  }
  else if(En<(rank-1) && Dn==(rank-1))  // D达到边缘
  {
    UF[rule[Dn][En]]  += EF[0]*DF[0];	
    UF[rule[Dn][En+1]]+= EF[1]*DF[0];
  }
  else				       //同时到边缘
  {
    UF[rule[Dn][En]] += EF[0]*DF[0];		
  }
  //解模糊
  for(int i=0;i<rank;i++)   U+=UF[i]* UFP[i];
  return U;
}

float Fuzzy3(float E,float D)//面积法 两输入 一输出
{
    int En=0,Dn=0;
    float EFP[7]={-150,-100,-50,0,50,100,150};
    float DFP[7]={-105,-70,-35,0,35,70,105};
    float UFP[7]={ 0, 2.5, 5, 7.5, 10, 13, 16};
    float EF[2]={0}, DF[2]={0},UF[7]={0},U=0;
    int rule[7][7]={
      //e -3 -2 -1  0  1  2  3   //  ec
          {0, 5, 6, 3, 0, 0, 0}, // -3
          {0, 4, 5, 3, 0, 0, 0}, // -2
          {0, 0, 2, 3, 0, 0, 0}, // -1
          {0, 0, 0, 0, 0, 0, 0}, //  0
          {0, 0, 0, 3, 2, 0, 0}, //  1
          {0, 0, 0, 3, 5, 4, 0}, //  2
          {0, 0, 0, 3, 6, 5, 0}  //  3
    };
  /*-----E隶属函数描述-----*/
  if(E < EFP[0]){ EF[0] =1.0; En= 0; }
  else if(E < EFP[1]){ EF[0] = (EFP[1]-E)/(EFP[1]-EFP[0]); En=0;}
  else if(E < EFP[2]){ EF[0] = (EFP[2]-E)/(EFP[2]-EFP[1]); En=1;}
  else if(E < EFP[3]){ EF[0] = (EFP[3]-E)/(EFP[3]-EFP[2]); En=2;}
  else if(E < EFP[4]){ EF[0] = (EFP[4]-E)/(EFP[4]-EFP[3]); En=3;}
  else if(E < EFP[5]){ EF[0] = (EFP[5]-E)/(EFP[5]-EFP[4]); En=4;}
  else if(E < EFP[6]){ EF[0] = (EFP[6]-E)/(EFP[6]-EFP[5]); En=5;}
  else	{EF[0] =1;En=6;}
  EF[1] = 1 - EF[0]; 
  /*-----D隶属函数描述-----*/
  if(D <= DFP[0]){ DF[0] =1.0;  Dn = 0;}
  else if(D < DFP[1]){ DF[0] = (DFP[1]-D)/(DFP[1]-DFP[0]); Dn=0;}
  else if(D < DFP[2]){ DF[0] = (DFP[2]-D)/(DFP[2]-DFP[1]); Dn=1;}
  else if(D < DFP[3]){ DF[0] = (DFP[3]-D)/(DFP[3]-DFP[2]); Dn=2;}
  else if(D < DFP[4]){ DF[0] = (DFP[4]-D)/(DFP[4]-DFP[3]); Dn=3;}
  else if(D < DFP[5]){ DF[0] = (DFP[5]-D)/(DFP[5]-DFP[4]); Dn=4;}
  else if(D < DFP[6]){ DF[0] = (DFP[6]-D)/(DFP[6]-DFP[5]); Dn=5;}
  else	{DF[0] =1;Dn=6;}
  DF[1] = 1 - DF[0];
  /*查询规则表*/  
  if(En<(rank-1) && Dn<(rank-1))        //都没到边缘
  {
    UF[rule[Dn][En]]    += EF[0]*DF[0];
    UF[rule[Dn][En+1]]  += EF[1]*DF[0];	
    UF[rule[Dn+1][En]]  += EF[0]*DF[1];
    UF[rule[Dn+1][En+1]]+= EF[1]*DF[1];
  }
  else if(En==(rank-1) && Dn<(rank-1))  // E到边缘
  {
    UF[rule[Dn][En]]  += EF[0]*DF[0];                
    UF[rule[Dn+1][En]]+= EF[0]*DF[1];
  }
  else if(En<(rank-1) && Dn==(rank-1))  // D达到边缘
  {
    UF[rule[Dn][En]]  += EF[0]*DF[0];	
    UF[rule[Dn][En+1]]+= EF[1]*DF[0];
  }
  else				       //同时到边缘
  {
    UF[rule[Dn][En]] += EF[0]*DF[0];		
  }
  //解模糊
  for(int i=0;i<rank;i++)   U+=UF[i]* UFP[i];
  return U;
}
