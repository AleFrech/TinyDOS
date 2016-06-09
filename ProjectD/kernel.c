extern int interrupt (int number, int AX, int BX, int CX, int DX);
extern char readChar();
extern void loadProgram();
extern void launchProgram(int segment);
extern void readSector(char *buffer,int sector);
extern void writeSector(char *buffer,int sector);
extern void makeInterrupt21();
extern void printChar(char chr);
void executeProgram(char* programName, int segment);
void writeFile(char* name, char* buffer, int  numberOfSectors);
void printString(char* string);
void readString(char * buffer);
void addTerminationChars(char buffer[],int index);
void backSpace();
void ListFiles();
void deleteFile(char* name);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void println();
char cmd[5];


int main(){
   char buffer[13312]; 
   cmd[0] = 's';
   cmd[1] = 'h';
   cmd[2] = 'e';
   cmd[3] = 'l';
   cmd[4] = 'l';
   cmd[5] = '\0';
   makeInterrupt21();
   executeProgram(cmd,0x2000);
   while(1);
   return 0;
}

void println(){
  printChar('\r');
  printChar('\n');
}

void printString(char * string){
  int i;
  for(i=0;string[i]!='\0';i++){
 		printChar(string[i]);
 	}
}

void readString(char* buffer){ 
  char currentChar; 
  int i=0;
  buffer[0]=0x00;
  while(currentChar!=0xD && i<79 ){
    currentChar=readChar(); 
    if(currentChar==0x8 && i>0){
      backSpace(currentChar);
      i--;
    }else if(currentChar!=0x8 && currentChar!=0xD){ 
      buffer[i]=currentChar;      
      printChar(buffer[i]);
      i++;
    }
  }
  addTerminationChars(buffer,i);
  println();
}

void backSpace(char chr){
  printChar(chr);
  printChar(0x0);
  printChar(chr);
}

void addTerminationChars(char buffer[],int index){
   buffer[index]=0x0;
}

int readFile (char *name, char *buffer){  
    int i=0,sec=0,tmp=0;
    char dirBuffer[512];
    readSector(dirBuffer,2);
    for(i=0;i<16;i++) {
        int j=0;
        for(j=0;j<7;j++){
            if(j<6){               
                if(j==0 && dirBuffer[i*32]==0) break;         
                if((name[j]==0 || name[j] == '\n') && dirBuffer[32*i+j]==0) tmp = 1;
                if(tmp == 0 && name[j]!=dirBuffer[32*i+j]) break;
            }else{
                int address=0;
                sec=0; 
                do{
                    address = 32*i+j+sec;
                    readSector(buffer+(sec*512), dirBuffer[address]);
                    sec++;
                }while(dirBuffer[address]!=0);
                return sec;
            }
        }
    }
    return -1;
}

void deleteFile(char* name){
    int i;
    char mapSec[512];
    char dirBuffer[512];
    readSector(dirBuffer,2);
    readSector(mapSec,1);
    for(i=0;i<16;i++) {
        int j=0;
        for(j=0;j<7;j++){
            if(j<6){               
                 if((j==0 && dirBuffer[i*32]==0) || ((name[j]==0 || name[j] == '\n') || dirBuffer[32*i+j]!=0) &&  (name[j]!=dirBuffer[32*i+j]))
                  break;
            }else{
                int k=0;
                int address=0;
                dirBuffer[32*i] = 0x00;
                do{
                    address = 32*i+j+k;  
                    mapSec[(int) dirBuffer[address]] = 0x00;
                    dirBuffer[address] = 0x00; 
                    k++; 
                }while(dirBuffer[address]!=0);
                writeSector(dirBuffer, 2);
                writeSector(mapSec, 1);
                return;
            }
        }
    }
}


void copyFile (char *fileName1, char *fileName2){
  int sec;
  char buffer[13312];
  buffer[0]=0x00;
  sec = readFile(fileName1, buffer);
  writeFile(fileName2, buffer, sec);
}


void writeFile(char* fileName, char* buffer, int numberOfSectors){
  int i=0, j=0, k=0, sec=0,m=0;
  char mapSec[512];
  char dirBuffer[512];
  char newSector[512];
  readSector(dirBuffer,2);
  readSector(mapSec,1);
  
  for(i = 0; i < 16; i++){
    if(dirBuffer[i*32] == 0){
      for(j = 0; j < 6; j++){
        if(fileName[j] != '\0' &&  fileName[j] != '\n')
           dirBuffer[i*32+j] =fileName[j];
        else
          dirBuffer[i*32+j]=0x00;
      }
      for(k=0;k<numberOfSectors;k++){
        sec=0;
        while(sec<512){
            if(mapSec[sec]==0x00){
                mapSec[sec]=0xFF;
                break;
            }
            sec++;
        }
        for(m = 0; m <  512; m++){
            newSector[m] = buffer[k*512 +m];
        }
        writeSector(newSector,sec);
        dirBuffer[i*32+j] =sec;    
        j++;       
      }
      for(j=0;j<26-numberOfSectors;j++){
        dirBuffer[i*32+6+numberOfSectors+j]=0x00;
      }
      writeSector(dirBuffer, 2);
      writeSector(mapSec, 1);
      return;
    }
  }
}


int div(int a, int b){
  int quo = 0;
  while((quo + 1)*b <=a){
    quo++;
  }
  return quo;
}

int mod(int a, int b){
  while(a >= b)
      a = a - b;
  return a;
}


char GetSizeOfSector(int sec, int index,char* nums){
  int number=div(sec,index);
  if(number!=0)
    return nums[div(sec,index)];
  return ' ';
}

void ListFiles(){  
    int i=0,sec=0;
    char dirBuffer[512];
    char buffer[13312];
    char fullName[25];
    char name[7];
    char num[10];  
    num[0] = '0';
    num[1] = '1';
    num[2] = '2';
    num[3] = '3';
    num[4] = '4';
    num[5] = '5';
    num[6] = '6';
    num[7] = '7';
    num[8] = '8';
    num[9] = '9';     
    readSector(dirBuffer, 2);
    for(i=0;i<16;i++) {
      if(dirBuffer[i*32]!=0){
         int j;
         for(j=0;j<6;j++) {
             if(dirBuffer[i*32+j]==0) 
                break;
             name[j]=dirBuffer[i*32+j];
             fullName[j]=dirBuffer[i*32+j];               
         }
         if(j>0){
            name[j] = '\0';
            fullName[j]=' ';           
         } 
         sec=readFile(name,buffer);
         fullName[j+1]=GetSizeOfSector(sec,1000,num);
         fullName[j+2]=GetSizeOfSector(sec,100,num);
         fullName[j+3]=GetSizeOfSector(sec,10,num);
         fullName[j+4] = num[mod(sec, 10)-1];
         fullName[j+5] = ' ';
         fullName[j+6] = 's';
         fullName[j+7] = 'e';
         fullName[j+8] = 'c';
         fullName[j+9] = 't';
         fullName[j+10] = 'o';
         fullName[j+11] = 'r';
         fullName[j+12] = 's';
         fullName[j+13] = '\0';
         printString(fullName);
         println();
      }      
    }
}

void executeProgram(char* programName, int segment){
    char buffer[13312];
    if(readFile(programName, buffer)!=-1){
        moveToSegment(segment,0,buffer, 13312);   
        launchProgram(segment);
    }
    interrupt(0x21,5, 0, 0, 0);
}


void terminate(){
    cmd[0] = 's';
    cmd[1] = 'h';
    cmd[2] = 'e';
    cmd[3] = 'l';
    cmd[4] = 'l';
    cmd[5] = '\0';
    executeProgram(cmd,0x2000);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
  switch(ax){
      case 0:
         printString((char*)bx);
         break;
      case 1:
         readString((char*)bx);
         break;
      case 2:
         readSector((char*)bx,cx);
         break;
      case 3:
         readFile((char*)bx,(char*)cx);
         break;
      case 4:
         executeProgram((char*)bx,cx);
         break;
      case 5:
         terminate();
         break;
      case 6:
         writeSector((char*)bx,cx);
         break;
      case 7:
         deleteFile((char*)bx);
         break;
      case 8:
         writeFile((char*)bx,(char*)cx,dx);
         break;
      case 9:
         copyFile((char*)bx,(char*)cx);
         break;
      case 10:
         clearScreen();
         terminate();
         break;
      case 11:
         ListFiles();
         terminate();
         break;
      default:
         printString("Error Ocurred");
         break;   
  }
}
