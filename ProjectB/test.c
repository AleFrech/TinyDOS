extern void syscall_printString(char *str);
extern void syscall_readString(char  *str);
extern void syscall_readSector(char *buffer, int sector);

int main(){
  
  char line[80]; 
  char buffer[512]; 
   
  syscall_printString("Enter a line: ");
  syscall_readString(line);
  syscall_printString("-----\r\n");
  syscall_printString(line);
  syscall_printString("------\r\n");
  syscall_printString("Reading sector 30 from floppy\r\n");
  syscall_readSector(buffer, 30);
  syscall_printString(buffer);  
   while(1){}
   return 0;
}