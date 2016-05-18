extern int interrupt (int number, int AX, int BX, int CX, int DX);
extern void loadProgram();
extern void makeInterrupt21();
extern void printChar(char chr);
void printString(char* string);
void readString(char * buffer);
void addTerminationChars(char buffer[],int index);
void backSpace();
void readSector(char *buffer,int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
int mod(int a,int b);
extern int readChar();
void println();


int main(){
   makeInterrupt21();
   loadProgram();   
   while(1){}
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
  while(currentChar!=0xd && i<79 ){
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