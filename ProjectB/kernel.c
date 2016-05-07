extern int interrupt (int number, int AX, int BX, int CX, int DX);
extern void makeInterrupt21();
void printChar(char chr);
void printString(char* string);
void readString(char buffer[]);
void addTerminationChars(char buffer[],int index);
void backSpace();
void writeInBuffer();
void readSector(char *buffer,int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
int mod(int a,int b);
void println();


int main(){
  
  char line[80];  
  makeInterrupt21();
  interrupt(0x21,1,line,0,0);
  interrupt(0x21,0,line,0,0);
   while(1){}
   return 0;
}


void println(){
  printChar('\r');
  printChar('\n');
}

void printChar(char chr){
  char al = chr;
  char ah = 0xE;
  int ax = ah * 256 + al; 
  interrupt(0x10, ax, 0, 0, 0);
}

void printString(char * string){
  int i;
  for(i=0;string[i]!='\0';i++){
 		printChar(string[i]);
 	}
}

void readString(char buffer[]){ 
  int readedchar=0; 
  int i=0;  
  while(readedchar!=0xd && i<78 ){
    readedchar=interrupt(0x16,0,0,0,0); 
    if(readedchar==0x8 && i>0){
      backSpace((char)readedchar);
      i--;
    }else if(readedchar!=0x8){       
      writeInBuffer(buffer,(char)readedchar,i);
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

void writeInBuffer(char buffer[],char chr,int index){
      buffer[index]=chr;
      printChar(chr);
}


void readSector(char* buffer,int sector){
 int relativeSector = mod(sector,18)+1;
 int head = mod(div(sector,18),2);
 int track = div(sector,36);
 interrupt(0x13,2*256+1,buffer,track*256+relativeSector,head*256+0);
}


int mod(int a,int b){
  while(a>=b){
    a=a-b;
  }
  return a;
}

int div(int a, int b){
  int quotient=0;
  while((quotient+1)*b<=a){
    quotient++;
  }
  return quotient;
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
      default:
         printString("Error Ocurred");
         break;   
  }
}