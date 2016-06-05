int strcmp(char *str1,char*str2);
void commandInterpreter(char *buffer);
int splitBuffer(char* buffer,char * command);
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

int splitBuffer(char* buffer,char * command){
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

void type(char *args){
   char buffer[13312];
   int i;
   for(i=0;i<13312;i++){
     buffer[i]=0;
   }
   syscall_readFile(args,buffer);
   syscall_printString(buffer);
   syscall_printString("\r\n");
}

void execute(char *args){
   syscall_executeProgram(args,0x2000);
}

void delete(char* args){
  syscall_deleteFile(args);
}

void Copy(char *args){
  char filename1[30];
  int offset =splitBuffer(args,filename1);
  syscall_copyFile(filename1,args+offset);
}


void commandInterpreter(char *buffer){
  char command[30];
  int offset = splitBuffer(buffer,command);
    if(strcmp("type",command)){
        type(buffer+offset);
    }else if(strcmp("execute",command)){
        execute(buffer+offset);
    }else if(strcmp("delete",command)){
        delete(buffer+offset);
    }else if(strcmp(command, "copy")){
        Copy(buffer+offset);
    }else if(strcmp("clear",command)){
        syscall_clearScreen();
    }else if(strcmp("ls",command)){
        syscall_listFiles();
    }else{
        syscall_printString("Command not found\r\n"); 
    }
}