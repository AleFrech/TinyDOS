extern int interrupt (int number, int AX, int BX, int CX, int DX);
int readFile (char *fileName, char *buffer);
extern void loadProgram();
extern void launchProgram(int segment);
void executeProgram(char* programName, int segment);
extern void makeInterrupt21();
extern void printChar(char chr);
void printString(char* string);
void readString(char * buffer);
void addTerminationChars(char buffer[],int index);
void backSpace();
extern void readSector(char *buffer,int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
extern char readChar();
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
   interrupt(0x21,4,cmd,0x2000,0);
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

void readString(char buffer[]){ 
  char currentChar; 
  int i=0;  
  while(currentChar!=0xd && i<78 ){
    currentChar=readChar(); 
    if(currentChar==0x8 && i>0){
      backSpace((char)currentChar);
      i--;
    }else if(currentChar!=0x8){ 
      buffer[i]=currentChar;      
      printChar(currentChar);
      i++;
    }
  }
  addTerminationChars(buffer,i);
  println();
}

void backSpace(char chr){
  printChar(chr);
  printChar(' ');
  printChar(chr);
}

void addTerminationChars(char buffer[],int index){
   buffer[index]=(char)0xa;
   buffer[index+1]=(char)0x0;
}

int readFile (char *fileName, char *buffer){  
    int i;
    char dirBuffer[512];
    readSector(dirBuffer,2);
    for(i=0;i<16;i++) {
        int j=0;
        for(j=0;j<7;j++){
            if(j<6){               
                if((j==0 && dirBuffer[i*32]==0) || (fileName[j]!=dirBuffer[32*i+j]) ) 
                    break;
            }else{
                int k=0;
                int address=0;
                do{
                    address = 32*i+j+k;
                    readSector(buffer+(k*512), dirBuffer[address]);
                    k++;
                }while(dirBuffer[address]!=0);
                return 0;
            }
        }
    }
    return -1;
}

void executeProgram(char* programName, int segment){
    char buffer[13312];
    readFile(programName, buffer);
    moveToSegment(segment, buffer, 13312);   
    launchProgram(segment);
    interrupt(0x21,5, 0, 0, 0);
}


void terminate(){
    cmd[0] = 's';
    cmd[1] = 'h';
    cmd[2] = 'e';
    cmd[3] = 'l';
    cmd[4] = 'l';
    cmd[5] = '\0';
    interrupt(0x21, 4,cmd, 0x2000, 0);
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
      case 10:
         clearScreen();
         terminate();
         break;
      default:
         printString("Error Ocurred");
         break;   
  }
}