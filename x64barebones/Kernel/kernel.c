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
void secondProcess() {
	while(1) {
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process 1");
	}
}

void init(){ //change to real init process
	while(1){
		int j=0;
		while(j<100000000){
			j++;
		}
		ncPrint("Process 0");
		
	}
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

	/**DEBUGGING**/
	/*ncPrintHex(allocate(0x1000));ncNewline();
	ncPrintHex(allocate(0x1000));ncNewline();
	ncPrintHex(allocate(0x1000));ncNewline();
	while(1);
	/**DEBUGGING**/

	//((EntryPoint)sampleCodeModuleAddress)();

	/* Add init process */
	addProcess(init);

	addProcess(secondProcess); //Debugging

	//Init should add shell process
	//addProcess(sampleCodeModuleAddress);
	

	/* Switch to first processs, and allow interruptions */
	endLoadingKernel();

	while(1);

	return 0;
}


/* - DEBUGGING - */

void acaEstoy(void * a) {
	ncPrint("aca estoy");ncNewline();
	//ncPrintHex(a);
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