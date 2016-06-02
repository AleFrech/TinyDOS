int strcmp(char *str1,char*str2);
void commandInterpreter(char *buffer);
void type(char *args);
void execute(char *args);
int main(){
    while(1){
       char buffer[30];
       syscall_printString("cmd:>");
       syscall_readString(buffer);
       commandInterpreter(buffer);
    }
    
    return 0;
}

int strcmp(char *str1,char*str2){
  int i;
  for(i=0; i <30; i++){
    if(str1[i]!='\0' && str2[i]!='\0'){
      if(str1[i] != str2[i]){
        return 0;
      }
    }else{
      if(str1[i]!=str2[i]) 
        return 0;
      return 1;
    }    
  }
}


void type(char *args){
   char buffer[13312];
   int i;
   for(i=0;i<13312;i++){
     buffer[i]=0;
   }
   syscall_readFile(args,buffer);
   syscall_printString(buffer);
}

void execute(char *args){
   syscall_executeProgram(args,0x2000);
}

void commandInterpreter(char *buffer){
  char command[30];
  int offset = splitBufferCommand(buffer,command);
    if(strcmp("type",command)){
        type(buffer+offset);
    }else if(strcmp("execute",command)){
        execute(buffer+offset);
    }else if(strcmp("clear",command)){
        syscall_clearScreen();
    }else if(strcmp("ls",command)){
        syscall_listFiles();
    }else{
        syscall_printString("Command not found\r\n"); 
    }
}

int splitBufferCommand(char* buffer,char * command){
  int offset=0;
  while(offset<30){
    if(buffer[offset]==' ' || buffer[offset]=='\n'){
        command[offset] = '\0';
        offset++;
        break;   
    }else{
        command[offset]=buffer[offset];
        offset++;
    }      
  }
  return offset;
}