extern int interrupt (int number, int AX, int BX, int CX, int DX);
extern char readChar();
extern void loadProgram();
extern void launchProgram(int segment);
extern void readSector(char *buffer,int sector);
extern void writeSector(char *buffer,int sector);
extern void makeInterrupt21();
extern void printChar(char chr);
void executeProgram(char* programName);
void executeWait(char* programName);
int div(int a, int b);
int mod(int a, int b);
void writeFile(char* name, char* buffer, int  numberOfSectors);
void printString(char* string);
void readString(char * buffer);
void killProcess(int id);
void addTerminationChars(char buffer[],int index);
void backSpace();
void ListFiles();
void deleteFile(char* name);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void println();
int secondCont;
char cmd[5];

struct PCB {
unsigned int status;
unsigned int stackPointer;
unsigned int segment;
struct PCB *waiter;
}ProcessTable[8];

struct regs {
unsigned int es;
unsigned int ds;
unsigned int ax;
unsigned int bp;
unsigned int di;
unsigned int si;
unsigned int dx;
unsigned int cx;
unsigned int bx;
unsigned int ip;
unsigned int cs;
unsigned int flags;
};



struct PCB *currentProcess;

int main(){
   int i=0;
   secondCont=0;
   for(i=0;i<8;i++){
       ProcessTable[i].status=4;
       ProcessTable[i].stackPointer=0xff00;
       ProcessTable[i].segment=(i+2)*0x1000;
       ProcessTable[i].waiter=0;
   }
   currentProcess=0;
   makeInterrupt21(); 
   cmd[0] = 's';
   cmd[1] = 'h';
   cmd[2] = 'e';
   cmd[3] = 'l';
   cmd[4] = 'l';
   cmd[5] = '\0';
   executeProgram(cmd);
   irqInstallHandler();
   setTimerPhase(100); 
   while(1);
   return 0;
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
  if(sec>-1)
    writeFile(fileName2, buffer, sec-1);
}

void writeFile(char* fileName, char* buffer, int numberOfSectors){
  int i=0, j=0, k=0, sec=0,m=0,tmp=0;
  char mapSec[512];
  char dirBuffer[512];
  char newSector[512];
  readSector(dirBuffer,2);
  readSector(mapSec,1);
  
  for(i = 0; i < 16; i++){
    if(dirBuffer[i*32] == 0){
      for(j = 0; j < 6; j++){
         if(fileName[j]=='\0' || fileName[j]=='\n')
         	tmp = 1;
        if(tmp == 0)
            dirBuffer[i*32+j]=fileName[j];
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


void intToString(int number){
	int i = 0,j=0;	
	char buffer[10];	
    for(j=0;j<10;j++){
        buffer[j]=0x0;
    }
	while(number!=0){
		buffer[i] = (char)mod(number,10)+48;
		number=div(number,10);
		i++;	
	}	
	for(i = 9; i != -1; i--)
		if(buffer[i]!=0)
			printChar(buffer[i]);
}

void ListFiles(){  
    int i=0,sec=0;
    char dirBuffer[512];
    char buffer[13312];
    char name[7];  
    readSector(dirBuffer, 2);
    for(i=0;i<16;i++) {
      if(dirBuffer[i*32]!=0){
         int j;
         for(j=0;j<6;j++) {
             if(dirBuffer[i*32+j]==0) 
                break;
             name[j]=dirBuffer[i*32+j];              
         }
         if(j>0){
            name[j] = '\0';     
         }
         sec=readFile(name,buffer);
         printString(name);
         printChar(' ');
         intToString(sec-1);  
         println();
      }      
    }
}

void terminate(){
    setKernelDataSegment();
    currentProcess->status=4;
    currentProcess->stackPointer=0xff00;
	if(currentProcess->waiter!=0)
		currentProcess->waiter->status=1;
	restoreDataSegment();
    #asm
	sti
	#endasm
    while(1);
}


void initializeProgram(int segment){
    struct regs registers;
    registers.ax = 0;
	registers.bp = 0;
	registers.di = 0;
	registers.si = 0;
	registers.dx = 0;
	registers.cx = 0;
	registers.bx = 0;
	registers.ip = 0;
	registers.ds = segment;
	registers.es = segment;
    registers.cs = segment;
	registers.flags = 0x0200;
	moveToSegment(segment,0xff00,&registers,24);
}

void executeProgram(char* programName){
    int i=0,segment=0;
    char buffer[13312];
    for(i=0;i<13312;i++)
        buffer[i]=0x00;
    setKernelDataSegment();
    for(i=0;i<8;i++){
        if(ProcessTable[i].status==4){
            ProcessTable[i].status=1;         
            segment=ProcessTable[i].segment;
            break;
        }
    }
    restoreDataSegment();
    if(readFile(programName, buffer)!=-1){ 
        moveToSegment(segment,0, buffer, 13312); 
        initializeProgram(segment);
    }
}

void executeWait(char* programName){
    char buffer[13312];
    int i=0,segment=0;
    setKernelDataSegment();
    for(i=0;i<8;i++){
        if(ProcessTable[i].status==4){
            ProcessTable[i].status=1;
            ProcessTable[i].waiter=currentProcess;
            segment=ProcessTable[i].segment;
            currentProcess->status=2;
            break;
        } 
    }
    restoreDataSegment();
    if(readFile(programName, buffer)!=-1){ 
        moveToSegment(segment,0, buffer, 13312); 
        initializeProgram(segment);
    }

}

void killProcess(int id){
  setKernelDataSegment();
  ProcessTable[id-1].status = 4;
  ProcessTable[id-1].stackPointer = 0xff00;
  if(ProcessTable[id-1].waiter!=0)
		ProcessTable[id-1].waiter->status=1;
  restoreDataSegment();
}


int listProcess(int* pointerTable){
    int i=0,cont=0;
    setKernelDataSegment();
    for(i=0;i<8;i++){
        if(ProcessTable[i].status!=4)
            cont++;
    }
    moveToSegment(currentProcess->segment,pointerTable,&ProcessTable,70);
    restoreDataSegment();
    return cont;
}


void scheduleProcess(int currentProcessSP){
	int tmpCurrent=-1;
	setKernelDataSegment();
	if(currentProcess!=0){
		if(currentProcess->status!=4){
			currentProcess->stackPointer = currentProcessSP;
			if(currentProcess->status==3)
				currentProcess->status = 1;
			tmpCurrent = div(currentProcess->segment,0x1000)-2;
		}
	}
    while(1){
        tmpCurrent++;
        if(tmpCurrent==7)
            tmpCurrent=0;          
        if(ProcessTable[tmpCurrent].status==1){
			currentProcess = &(ProcessTable[tmpCurrent]);
			currentProcess->status=3;
            break;
		}
    }
	restoreDataSegment();
}

void handleTimerInterrupt(){
    secondCont++;
	if(secondCont>99){
		printString("TIC");
		secondCont=0;
	}
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
         executeProgram((char*)bx);
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
         killProcess(bx);
         break;
      case 10:
         clearScreen();
         break;
      case 11:
          copyFile((char*)bx,(char*)cx);
         break;
      case 12:
         ListFiles();
         break;
      case 13:
         executeWait((char*)bx);
         break;
      case 14:
         ax=listProcess(bx);
         break;
      case 15:
         printChar((char)bx);
      default:
         printString("Error Ocurred");
         break;   
  }
}
