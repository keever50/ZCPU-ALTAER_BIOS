

#define FirstTopMemory 65536
#define LastMemoryAddress 65535
#define DefaultMappingSize 9999
#define DefaultMappingSeperation 10000
#define DefaultCharColorStyle 999

//Define interrupt tables
stef;
LIDTR IntTable;
//PRINT STRING 10h
IntTable[(10*4)+0]=PrintINT;
IntTable[(10*4)+1]=0;
IntTable[(10*4)+2]=0;
IntTable[(10*4)+3]=41;
//WAIT 9h
IntTable[(9*4)+0]=WaitINT;
IntTable[(9*4)+1]=0;
IntTable[(9*4)+2]=0;
IntTable[(9*4)+3]=41;

main();



int 1;

char Devices[8];
int48* ConsoleScreen;
void main()
{

    //Init//
    int48* ExtBus = FirstTopMemory;
    ExtBus[16]=40; //Controll. 32 is the offset. Add 8 ports for device info. 40 Addresses to control.
    
    ExtBus[17]=1; //Recursive scan.
    ExtBus[20]=1; //Get all device info.
    
    //Get devices
    //char Devices[8];
    GetDevices(ExtBus, Devices);

    //Map ext bus    
    int48 i;
    for(i=0;i<8;i++)
    {
        int48 Seperator = i * 2;
        int48 Size = ( i * 2 ) + 1;
        int48 Seperation = DefaultMappingSeperation * i;
        ExtBus[Seperator] = Seperation;
        ExtBus[Size] = Seperation+DefaultMappingSize; 
    }

    //speaker init
    int48* Speaker = FindDeviceAddress( ExtBus, Devices, 17 );

    //Console init
    ConsoleScreen = FindDeviceAddress( ExtBus, Devices, 11 );
    ConsoleScreen[2041]=1;//clear
    ConsoleScreen[2045]=0;//Reset cursor
    ConsoleScreen[2046]=1;//Enable cursor
    ConsoleScreen[2042]=000;//BG
       
    //Keyboard init
    int48* Keyboard = FindDeviceAddress( ExtBus, Devices, 15 );
    if(Keyboard == -1)
    {
        Print(ConsoleScreen, "Keyboard not found!\n\0", 911); 
        Beep(Speaker, 0.01, 2);     
        
    }else
    {
        Print(ConsoleScreen, "Keyboard found\n\0", 191);   
    }
    
    //Primary drive init
    int48* PrimaryDrive = FindDeviceAddress( ExtBus, Devices, 1 );
    if(Keyboard == -1)
    {
        Print(ConsoleScreen, "Storage not found!\n\0", 911);  
        Beep(Speaker, 0.01, 2);      
    }else
    {
        Print(ConsoleScreen, "Storage found\n\0", 191);   
        PrimaryDrive[4]=0;       
    }

    //bios welcome
    Print(ConsoleScreen, "BIOS 0.8219 alpha\n\0", DefaultCharColorStyle);
    Print(ConsoleScreen, "/ALTAER/ \n\0", 118);
    
    Print(ConsoleScreen, "Hold TAB for bios setup\n\0", DefaultCharColorStyle);
    wait(2);
    
    //HOLD TAB TO START BIOS UTILLITY MODE
    if(GetKey(Keyboard) != 9)
    {
        Boot( PrimaryDrive, ConsoleScreen );  
    } 
    
    //Command Util mode
    ConsoleScreen[2041]=1;//clear
    ConsoleScreen[2045]=0;//Reset cursor
    ConsoleScreen[2046]=1;//Enable cursor    
    ConsoleScreen[2042]=004;//BG
    
    Beep(Speaker, 0.1, 1);
    Beep(Speaker, 0.1, 1.5);
    wait(1);
    Print(ConsoleScreen, "BIOS 0.8219 alpha\n\0", 555);
    Print(ConsoleScreen, "/ALTAER/ \n\0", 555);
    Print(ConsoleScreen, "Command utility mode\n\0", 999);
    Print(ConsoleScreen, "Enter help for commands\n\n\0", 888);


    //Command line
    char Command[255];
    while(1==1)
    {
        ClearCharArray(Command, 255);
        ConsoleRead( ConsoleScreen, Keyboard, Command );  
        OUT 0, 25252;  
        Print(ConsoleScreen, ">\0", 999); 
        Print(ConsoleScreen, Command, 999);
        Print(ConsoleScreen, "\n", 999);
        //COMMANDS
        if(strstr(Command, "help") > 0) //HELP
        {
            Print(ConsoleScreen, "Commands: \n\0", DefaultCharColorStyle);  
            Print(ConsoleScreen, " dev: shows connected devices\n\0", DefaultCharColorStyle); 
            Print(ConsoleScreen, " boot: boots from first disk\n\0", DefaultCharColorStyle); 
            Print(ConsoleScreen, " reboot: restarts cpu\n\0", DefaultCharColorStyle);  
        }
        if(strstr(Command, "reboot") > 0) //REBOOT
        {
            int 0;
        } 
        if(strstr(Command, "boot") > 0) //BOOT
        {
            Print(ConsoleScreen, "Booting...\n\0", DefaultCharColorStyle);  
            Boot( PrimaryDrive, ConsoleScreen );
        }  
        if(strstr(Command, "spam") > 0) //spam command for testing screen.
        {
            char spamchars[10];
            float random;
            while(1==1)
            {
                Wait(0.01);
                RAND EAX;
                random = EAX;
                Beep( Speaker, 0.01, random );
                random = random * 999999;
                IntToChars(random, spamchars, 6);
                Print(ConsoleScreen, spamchars, random);  
            }
        }         
        if(strstr(Command, "dev") > 0) //DEV
        {
            Print(ConsoleScreen, "Devices connected:\n\0", DefaultCharColorStyle);  
            int48 devi;
            char portnum[5];
            for(devi=0;devi<8;devi++)
            {
                if(Devices[devi]>0)
                {
                    IntToChars(devi, portnum, 3);
                    portnum[3]=0; //puts 0 at end of string
                    Print(ConsoleScreen, portnum, 999); 
                    Print(ConsoleScreen, ":\0", 999); 
                    
                    if(Devices[devi]==1)
                    {
                        int48* unknowndev = FindDeviceAddress( ExtBus, Devices, 1 );
                        IntToChars(unknowndev[510], portnum, 3);
                        portnum[3]=0; //puts 0 at end of string                    
                        Print(ConsoleScreen, portnum, 191);      
                    }else
                    {
                        IntToChars(Devices[devi], portnum, 3);
                        portnum[3]=0; //puts 0 at end of string                    
                        Print(ConsoleScreen, portnum, 191);                         
                    }
                    Print(ConsoleScreen, "\n", 191); 
                }else
                {
                    IntToChars(devi, portnum, 3);
                    portnum[3]=0; //puts 0 at end of string                    
                    Print(ConsoleScreen, portnum, 999); 
                    Print(ConsoleScreen, ":\0", 999); 
                    Print(ConsoleScreen, "none\n\0", 911);        
                }
            }
        }        
                             
    }
}


//INTERRUPTS
PrintINT: //INT 10
    if(EAX == 13)//EAX 13:   Write string
    {   //ESI = String source.
        int48 Col = EBX;
        char* INT10_WriteString_Source = ESI;
        Print(ConsoleScreen, ESI, Col);           
    }
IRET;

int48 WaitInt_Time;
WaitINT:

    OUT 0, EBX;
    WaitInt_Time = EBX;
    Wait( WaitInt_Time );
IRET;

//C Functions
void CopyBlock( int48* A, int48* B, int48 Bytes )
{
    ESI = A;
    EDI = B;
    EAX = Bytes;
    MCOPY EAX;        
}

void SlowCopyBlock( int48* A, int48* B, int48 Bytes )
{
    int48 I;
    int48 I2;
    int48 ToGo = A + Bytes;
    for(I=A; A<=ToGo; I++)
    {
        B[I2]=A[I];
        I2++;        
    }    
}

void Boot( int48* Drive, int48* ConsoleScreen )
{
    Drive[4]=1; //Motor 
    wait(0.1)
    Print(ConsoleScreen, "Reading bootloader\n", 999);
    Drive[5] = 1; //sector 1
    while(Drive[8] ==3 ){} //Wait until done
    Drive[6] = 1;
    while(Drive[8] == 3){}
    Print(ConsoleScreen, "Copying\n", 999);
    
    int48* NewAddr = 10000;
    
    //copy
    MOV ESI, Drive+512;
    MOV EDI, NewAddr;
    MCOPY 511;
    
    Print(ConsoleScreen, NewAddr, 991 );
    wait(0.1);
    Drive[4]=0; //Motor  

    Print(ConsoleScreen, "Jumping\n", 999);   
    
    OUT 0, *NewAddr; 
    Wait(1);
    //PUSHA;
    //MOV SS, 10000*2;
    //MOV ESP,2999;
   //MOV DS, 10000;
    //CPUSET 9,3000;
    
   // stef;

    //Console clear
    ConsoleScreen[2041]=1;//clear
    ConsoleScreen[2045]=0;//Reset cursor
    ConsoleScreen[2046]=1;//Enable cursor    
    ConsoleScreen[2042]=000;//BG
    
    JMPF 0,NewAddr;
}

void Beep( int48* Speaker, int48 dur, int48 pitch )
{
    Speaker[2] = pitch;
    Speaker[3] = 1;
    Speaker[0] = 1;
    //wait
    Wait(dur);
    Speaker[0] = 0;   
    Wait(0.01);     
}

void Wait( int48 Time )
{
    TIMER EAX;
    int48 CurTime = EAX;
    int48 TargetTime = CurTime+Time;
    
    while(CurTime <= TargetTime)
    {
        TIMER EAX;     
        CurTime = EAX;   
    }   
}

void ClearCharArray(char* Array, int48 Size)
{
    int48 i;
    for(i=0;i<Size;i++)
    {
        Array[i]=0;    
    }  
}

void IntToChars( int48 num, char* chars, int48 digits )
{
    digits = digits - 1;
    int48 number = num + 1;
    int48 number2;
    int48 i;
    for(i=digits;i>=0;i--)
    {
        MOV EAX, number;
        FCEIL EAX;
        number2 = EAX;
        chars[i] = 47+( number2 % 10 ) ;  
        if(chars[i]=='/'){chars[i]='9'} //dirty quick fix
        number = number / 10;  
    }        
}

//finds the first occurrence of the string "needle" in the longer string "haystack"
char *strstr(char *haystack, char *needle) {
  register char *s = haystack;
  register char *p = needle;

  while (1) {
    if (!*p) {
      return haystack;
    }
    if (*p == *s) {
      ++p;
      ++s;
    } else {
      p = needle;
      if (!*s) {
        return '\0';
      }
      s = ++haystack;
    }
  }
}



char FindDevice( char* Devices, char DeviceID ) //Returns index. -1 on not found.
{
    int48 i;
    for(i=0;i<8;i++)
    {
        if(Devices[i] == DeviceID)
        {
            return i;
            break;    
        }   
    }   
    return -1; 
}

void GetDevices( int48* extbus, char* result ) //Gets all devices on extbus and writes to results array.
{
    int48 i;
    for(i=0;i<8;i++)
    {
        result[i]=extbus[32+i];      
    }    
}

int48* FindDeviceAddress( int48* extbus, char* devices, char deviceID) //Returns pointer to device.
{
    char Index = FindDevice( devices, deviceID );
    if(Index == -1)
    {
        return -1; 
    }
    else
    {
        int48* Address = extbus[Index*2]+FirstTopMemory+40; //+40 offset of extbus itself.
        return Address;
    }
}


void Print( int48* Console, char* str, int48 col )
{
    while(*str != 0)
    {
        if(Console[2045]>1018) //1080
        {
            Console[2038]=1; //shift
            Console[2045]=960; //go back a full line to start of end.
            Console[2047] = 0 //User defined. X
        }
        
        if(*str == '\n')
        {
            Console[2045]-=(Console[2047]); //go back a counted line. X
            Console[2045]+=64; //Jump back a full line
            Console[2047] = 0 //User defined. X   
        }else{
            //Put the char
            Console[Console[2045]] = *str;
            Console[Console[2045]+1] = col;
            Console[2045]+=2; //Move cursor
        } 
        str++; //Next char
        
        Console[2047] = Console[2045] % 60; //User defined. X
        Console[2047] = Console[2047] + 4;
    }    
}


void PrintChar( int48* Console, char str, int48 col )
{
        if(Console[2045]>1078) //1080
        {
            Console[2038]=1; //shift
            Console[2045]=1020; //go back a full line to start of end.
            Console[2047] = 0 //User defined. X
        }
        
        if(*str == '\n')
        {
            Console[2045]-=(Console[2047]); //go back a counted line. X
            Console[2045]+=64; //Jump back a full line
            Console[2047] = 0 //User defined. X   
        }else{
            //Put the char
            Console[Console[2045]] = str;
            Console[Console[2045]+1] = col;
            Console[2045]+=2; //Move cursor
        } 
        
        Console[2047] = Console[2045] % 60; //User defined. X
        Console[2047] = Console[2047] + 4;  
}

char GetKey( int48* Keyboard )
{
    return Keyboard[1];  
}

char CheckKey( char* Keys, char Key, char Count )
{
    char I;
    for(I=0;I<Count;I++)
    {
        if(Key==Keys[I])
        {
            return 1;
            break;    
        }    
    }
    return 0;
}

char* ConsoleRead( int48* Console, int48* Keyboard, int48* Input ) //Input is output string.
{
    int48 OldCursorPos = Console[2045];
    
    int48 StartPos = 1020;
    Console[2045] = StartPos;//Cursor
    
    char SpecialKeys[2] = {10,127};
    char OldKey = 0;
    char LineCursor = 0;
    
    while(1==1)
    {
        Wait(0.01);
        char Key = GetKey( Keyboard );
        if( Key != OldKey )
        {  
            OldKey = Key;
            if(Key != 0)
            {
                //Check special key. Dont write if it's not a normal char.
                if(CheckKey( SpecialKeys, Key, 2 ) != 1)
                {
                    Console[StartPos+(LineCursor*2)] = Key;
                    Console[StartPos+(LineCursor*2)+1] = 999;
                    Input[LineCursor] = Key;
                    LineCursor++; 
                    OUT 0, Key;   
                }else
                {
                    //BACKSPACE. Erases one char and move one char back.
                    if(Key == 127)
                    {                      
                        LineCursor--;    
                        Input[LineCursor] = 0;
                        Console[StartPos+(LineCursor*2)] = 0;
                        Console[StartPos+(LineCursor*2)+1] = 000;                          
                    }  
                    //ENTER. Resets line cursor and erase whole line. Return input.
                    if(Key == 10)
                    {
                        if(LineCursor > 0)
                        {
                            LineCursor = 0;
                            Console[2039]=17;//Clear row.
                            Console[2045] = OldCursorPos;
                            break;
                        }
                    }
                }  
                OUT 1, LineCursor; 
                Console[2045] = StartPos + (LineCursor*2); 
            }
        }
    }         
}

IntTable:
