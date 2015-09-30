#include <conio.h>     /* include for kbhit() and getch() functions */
#include <windows.h>   /* required before including mmsystem.h */
#include <mmsystem.h>  /* multimedia functions (such as MIDI) for Windows */
#include <signal.h>
#include "RtMidi.h"

using namespace std;

//Public variables
HMIDIOUT device;
RtMidiOut *midiout = 0;
std::vector<unsigned char> midiMessage;

void initMIDI(int num){

	int i, keyPress;
	int nPorts;
	char input;

	midiout = new RtMidiOut();
	// Check available ports.
	nPorts = midiout->getPortCount();
	if ( nPorts == 0 ) {
		cout << "No ports available!" << endl;
		delete midiout;
		exit(0);
	}

	// List Available Ports
	cout << "\nPort Count = " << nPorts << endl;
	cout << "Available Output Ports\n-----------------------------\n";
	for( i=0; i<nPorts; i++ )
	{
		try {
			cout << "  Output Port Number " << i << " : " << midiout->getPortName(i) << endl;
		}
		catch(RtError &error) {
			error.printMessage();
			delete midiout;
			exit(0);
		}
	}
	if(num == -1){
		cout << "\nSelect an output port number :" << endl;
		cin >> keyPress;
		while( keyPress < 0 || keyPress >= midiout->getPortCount() )
		{
			cout << "\nIncorrect selection. Please try again :" << endl;
			cin >> keyPress;
		}
		// Open Selected Port
		midiout->openPort( keyPress );
		keyPress = NULL;
	}else {
		cout << "\nUsing port 1 as default." << endl;
		midiout->openPort( num );
	}
}

void playMusic(int timbre, int tone, int velocity){
	midiMessage.push_back( timbre );
	midiMessage.push_back( tone );  
	midiMessage.push_back( velocity );
	midiout->sendMessage( &midiMessage );
	midiMessage.clear();
}