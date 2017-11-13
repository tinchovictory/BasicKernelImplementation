#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <interruptions.h>

#include <videoDriver.h>
#include <keyBoardDriver.h>
#include <pci.h>
#include <RTL8139.h>

#include <systemCalls.h>
#include <scheduler.h>
#include <memoryManager.h>

#include <pipefs.h>
#include <mutex.h>
#include <screenLoader.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

/* Assembly functions */
void cli(); /* Stop interruptions */
void endLoadingKernel(); /*  */

/* Initialize kernel loading */
void startLoadingKernel() {
	cli();
	/* Initialize kernel stack memory position */
	//setKernelStack();
}


/* Debugging processes */

void thread() {
	while(1) {
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("thread1");
	}
}

void thread2() {
	while(1) {
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("thread2");
	}
}

void secondProcess() {
	int i=0;
	while(1) {
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process 1");
		//addThreadToProcess(0, thread);
		i++;
		if(i==4) {
			blockProcess(0);
			ncPrint("--BLOCKED--");
			//printAllProcess();
		}
		if(i==10) {
			unblockProcess(0);
			ncPrint("--UNBLOCKED--");
		}
	}
}


void p() {
	ncPrint("hola ");
	removeProcess(2);
	while(1);
}

int i = 0;
void a() {
	char buff[50] = {0};
	while(1){
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process A");
		//i++;
		if(i == 0) {
			ncNewline();
			ncPrint("Starting pipe: ");
			ncPrint(do_pipe(1,2));
			ncNewline();

			ncPrint("Write");ncNewline();

			buff[0] = 'h';buff[1] = 'o';buff[2] = 'l';buff[3] = 'a';

			writeToPipe("1-2",buff);
			i++;
		}
		if(i==3) {
			ncPrint("Write");ncNewline();

			buff[0] = 'h';buff[1] = 'o';buff[2] = 'l';buff[3] = 'a';

			writeToPipe("1-2",buff);
			i++;
		}

	}
}

void b() {
	char buff[50] = {0};
	while(1) {
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process B");
		if(i==1) {
			ncPrint("Read");ncNewline();
			readFromPipe("1-2",buff);
			ncPrint(buff);ncNewline();
			i++;
			buff[0] = 0;
		}
		if(i==2) {
			i++;
			readFromPipe("1-2",buff);
			ncPrint("Read ");ncNewline();ncPrint(buff);ncNewline();
			
		}
	}
}

int mutexId = -1;
void m1() {
	int h = 0, t = 0;
	ncPrint("Start process 1");ncNewline();

	ncPrint("Start mutex");ncNewline();

	mutexId = createMutex();


	while(t<5) {
		int j = 0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process 1 ");
		t++;
	}

	requestAccess(mutexId);
	ncNewline();ncPrint("Lock mutex");ncNewline();			
	down(mutexId);

	t = 0;
	while(t<20) {
		int j=0;

		while(j<100000000){
			j++;
		}
		ncPrint("Process 1 ");
		t++;
	}

	ncPrint("Unock mutex");ncNewline();			
	up(mutexId);	

	t = 0;
	while(t<3) {
		int j = 0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process 1 ");
		t++;
	}

	ncPrint("Stop mutex");ncNewline();
	//destroy(mutexId);

	ncPrint("Stop process 1");ncNewline();

	//removeProcess(1);
	while(1);
}


void m2() {
	int h = 0, t = 0;
	ncPrint("Start process 2");ncNewline();

	/*ncPrint("Start mutex");ncNewline();

	mutexId = createMutex();
*/
	while(mutexId == -1) {
		ncPrint("waiting for mutex");ncNewline();
		t = 0;
		while(t<15) {
			int j = 0;
			while(j<100000000){
				j++;
			}
			ncPrint("Process 2 ");
			t++;
		}
	}

	ncNewline();ncPrint("Request Access to mutex");ncNewline();
	requestAccess(mutexId);
	ncNewline();ncPrint("Lock mutex");ncNewline();			
	down(mutexId);

	t = 0;
	while(t<20) {
		int j=0;

		while(j<100000000){
			j++;
		}
		ncPrint("Process 2 ");
		t++;
	}

	ncPrint("Unock mutex");ncNewline();			
	up(mutexId);	

	t = 0;
	while(t<3) {
		int j = 0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process 2 ");
		t++;
	}

	ncPrint("Stop mutex");ncNewline();
	destroy(mutexId);

	ncPrint("Stop process 2");ncNewline();

	//removeProcess(2);
	while(1);
}


void init(){ //change to real init process
	while(1){
		int j=0;
		while(j<100000000){
			j++;
		}
		//ncPrint("");
	}
}

void changeOnFocus(){
	int focusI = 0;
	while(1){
		int j=0;
		while(j<100000000){
			j++;
		}
		focusI++;

		if (focusI == 10) {
			focusI = 0;
		}
		
		if (focusI == 0) {
			loadScreen(1);
		}
		if (focusI == 5) {
			loadScreen(2);
		}
	}
}

void onFocusTest(){ //change to real init process
	addProcess(sampleCodeModuleAddress);
		
	loadScreen(1);

	addProcess(sampleCodeModuleAddress);

	addProcess(changeOnFocus);
}

int main()
{	

	startLoadingKernel();

	initializeInterruptions();
	activeRTLdma();
	initRTL();

	/* Start memory allocator */
	memAllocatorInit ((void *) 0x1000000, (void *)0x8000000);
	//memAllocatorInit (0x300000, 0x400000);
	
	setKernelStack();

	ncClear();

	//((EntryPoint)sampleCodeModuleAddress)();

	/* Add init process */
	addProcess(init);

	onFocusTest();

	//addProcess(m1);
	//addProcess(m2);
	/*addProcess(secondProcess); //Debugging
	addThreadToProcess(1, thread);//Debugging
	addThreadToProcess(1, thread2);//Debugging

	addProcess(p);//Debugging
	*/
	//Init should add shell process
	//addProcess(sampleCodeModuleAddress);
	

	/* Switch to first processs, and allow interruptions */
	endLoadingKernel();

	while(1);

	return 0;
}


/* - DEBUGGING - */

void acaEstoy() {
	ncPrint("aca estoy");ncNewline();
	//ncPrintDec(a);
	while(1);
}

void printEthMsg(ethMsg msg1){
	ncNewline();ncPrint("Mac destination: ");
		for(int i=0; i<MAC_SIZE; i++){
			ncPrintHex(msg1.mac[i]);
		}
		ncNewline();ncPrint("length: ");
		ncPrintDec(msg1.length);ncNewline();
		ncPrint("Message: ");ncNewline();
		for(int i=0; i<msg1.length; i++){
			ncPrintChar(msg1.msg[i]);
		}
		ncNewline();
		ncPrint("---------End of message----");ncNewline();ncNewline();
}