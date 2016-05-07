extern void putInMemory (int segment, int address, char character);

void PrintChar(int address,char * chr,int byteoffset,int color);
int CalculateAddress(int lineNumber);
int CalculateStartPosition(int address,int columnNumber);
void PrintString(int lineNumber,int columnNumber,char * string,int color);
void ClearScreen();

int main(){

   char *b;

   ClearScreen();
   b="Sistemas OPERATIVOS 1";
   PrintString(1,26,b,0x7);
   b = "UNIVERSIDAD TECNOLOGICA CENTROAMERICANA (UNITEC)";
   PrintString(2,15,b,0x7);
   b = "21151029 ALJANDRO FRECH";
   PrintString(3,25,b,0x7);
   b = "MI PRIMER SISTEMA OPERATIVO";
   PrintString(4,23,b,0x7);
   b = "--------------------------------------------------------------------------------";
   PrintString(5,1,b,0x7);  
   b = "***************************** HOLA MUNDO !! ************************************";
   PrintString(6,1,b,0x1);
   b = "**************************** HELLO WORLD !! ************************************";
   PrintString(7,1,b,0x2);
   b = "***************************** HALLO WELT !! ************************************";
   PrintString(8,1,b,0x3);
   b = "***************************** HOLA MON !! **************************************";
   PrintString(9,1,b,0x4 | 0x1);
   b = "***************************** HEJ VERDEN !! ************************************";
   PrintString(10,1,b,0x5);
   b = "***************************** TERE,MAAILM !! ***********************************";
   PrintString(11,1,b,0x6);
   b = "*************************** BONJOUR LE MONDE !! ********************************";
   PrintString(12,1,b,0x7);
   b = "****************************** HELO BYD !! *************************************";
   PrintString(13,1,b,0x8);
   b = "***************************** OLA MUNDIAL !! ***********************************";
   PrintString(14,1,b,0x9);
   b = "**************************** HELLO MONDYAL !! **********************************";
   PrintString(15,1,b,0xA);
   b = "****************************** CIAO MONDO !! ***********************************";
   PrintString(16,1,b,0xB);
   b = "***************************** SALVE MUNDI !! ***********************************";
   PrintString(17,1,b,0xC);
   b = "****************************** HEI VERDEN !! ***********************************";
   PrintString(18,1,b,0xD);
   b = "****************************** OLA MUNDO !! ************************************";
   PrintString(19,1,b,0xE);
   b = "***************************** SALUT LUME !! ************************************";
   PrintString(20,1,b,0xF);
   b = "*************************** POZDRAV SVIJETU !! **********************************";
   PrintString(21,1,b,0x1);
   b = "****************************** AHOJ SVETE !! ************************************";
   PrintString(22,1,b,0x3);
   b = "****************************** BOK SVIJETE !! ***********************************";
   PrintString(23,1,b,0x5);
   b = "***************************** KAIXO MUNDUA !! ***********************************";
   PrintString(24,1,b,0xA);
   b = "*************************** SAWUBONA MHLABA !! **********************************";
   PrintString(25,1,b,0xD);

   return 0;
}

void PrintString(int lineNumber,int colunmNumber,char * string,int color){
	int i=0;
  int lineaddress=CalculateAddress(lineNumber);
  int address=CalculateStartPosition(lineaddress,colunmNumber);
 	for(i=0;string[i]!='\0';i++){
 		PrintChar(address,string[i],i*2,color);
 	}
}

void PrintChar(int address,char *chr,int byteoffset,int color){
   putInMemory(0xB000,address+byteoffset,chr);
   putInMemory(0xB000,address+(byteoffset+1),color);
}

void ClearScreen(){
   int i;
   for(i=0;i<10;i++){
      int j;
      for(j=0;j<80;j++){
         PrintChar(CalculateAddress(i+1),' ',j*2,0x7);
      }
   }
}

int CalculateAddress(int lineNumber){
	int x=80*(lineNumber-1);
 	int y= x*2;
 	int address=0x8000+y;
 	return address;
}

int CalculateStartPosition(int address,int columnNumber){
  int newAddress=address;
  int i;
  for(i=1;i<columnNumber;i++){
    newAddress=newAddress+2;
  }
  return newAddress;
}

