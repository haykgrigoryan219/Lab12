// SimpleThread.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                      //
///                           cross platform multithreading made simple                                  //
///                                                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                      //
/// compile on linux with g++ by adding  -D_COMPILE_LINUX  and also  -pthread  to the command line.      //
///                                                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                      //
/// compile on windows with cl.exe by adding  /D_COMPILE_WINDOWS  to                                     //
/// the command line BEFORE the /LINK argument or it will not compile                                    //
///                                                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;

//crossplatform type definitions
#ifdef _COMPILE_WINDOWS
//#ifdef _WIN32
#include <Windows.h> //for Sleep and other time funcs, and thread funcs
typedef HANDLE THREADVAR;
typedef DWORD WINAPI THREADFUNCVAR;
typedef LPVOID THREADFUNCARGS;
typedef THREADFUNCVAR(*THREADFUNC)(THREADFUNCARGS);
typedef CRITICAL_SECTION THREAD_LOCK;
#endif
#ifdef _COMPILE_LINUX
#include <pthread.h>
#include <unistd.h>
typedef pthread_t THREADVAR;
typedef void* THREADFUNCVAR;
typedef void* THREADFUNCARGS;
typedef THREADFUNCVAR(*THREADFUNC)(THREADFUNCARGS);
typedef pthread_mutex_t THREAD_LOCK;
typedef unsigned long int DWORD_PTR;
typedef unsigned int DWORD;
typedef unsigned long long int uint64;
typedef long long int int64;
#endif

//cross platform functions
THREADVAR PrepareThread(THREADFUNC f, THREADFUNCARGS arg);
void StopThread(THREADVAR t);
void InitThreadLock(THREAD_LOCK& t);
void LockThread(THREAD_LOCK& t);
void UnlockThread(THREAD_LOCK& t);
void sleep(int ms);

// the thread function declaration
THREADFUNCVAR MyAsyncSimpleThread(THREADFUNCARGS lpParam);

// globals
int globlint = 0;
bool quitnow = false;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                      //
/// entry point                                                                                          //
///                                                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	THREAD_LOCK recv_lock;
	InitThreadLock(recv_lock);

	// prepare parameter(s) for the async thread
	DWORD_PTR* svRecvThrArgs = new DWORD_PTR[1];
	// pass the thread lock variable as parameter
	svRecvThrArgs[0] = (DWORD_PTR)&recv_lock;

	// start the thread with parameters
	THREADVAR recvThrHandle = PrepareThread(MyAsyncSimpleThread, (THREADFUNCARGS)svRecvThrArgs);

	// loop increment and check for exit
	while (!quitnow) {
///*
		LockThread(recv_lock); // lock with the same var

		globlint++;
		if (globlint > 5000) {
			quitnow = true;
		}
		cout << "\n\tMain loop.\t" << globlint << '\n';
		UnlockThread(recv_lock); // unlock with the same var
//*/
		sleep(5);
	}

	StopThread(recvThrHandle);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                      //
/// thread                                                                                               //
///                                                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
THREADFUNCVAR MyAsyncSimpleThread(THREADFUNCARGS lpParam) {
	// get parameter array
	DWORD_PTR* arg = (DWORD_PTR*)lpParam;
	// get 0th param, thats the thread lock variable used in main func
	THREAD_LOCK& ref_recv_lock = *((THREAD_LOCK*)arg[0]);

	// loop increment, check for exit and print
	while (!quitnow) {
		LockThread(ref_recv_lock);  // lock with the same var
		globlint++;
		if (globlint > 100) {
			quitnow = true;
		}
		cout << "\nMyAsyncSimpleThread.\t" << globlint << endl;
		UnlockThread(ref_recv_lock);  // unlock with the same var
		sleep(1);
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                      //
/// cross platform definitions                                                                           //
///                                                                                                      //
///////////////////////////////////////////////////////////////////////////////////////////////////////////

THREADVAR PrepareThread(THREADFUNC f, THREADFUNCARGS arg) {
#ifdef _COMPILE_LINUX
	pthread_t out;
	pthread_create(&out, NULL, f, arg);
	return out;
#endif
#ifdef _COMPILE_WINDOWS
	DWORD thrId;
	THREADVAR out = CreateThread(
		NULL,          // default security attributes
		0,             // use default stack size  
		(LPTHREAD_START_ROUTINE)f,    			// thread function name
		arg,          // argument to thread function 
		0,             // use default creation flags 
		&thrId			// returns the thread identifier 
	);
	return out;
#endif
}

void StopThread(THREADVAR t) {
#ifdef _COMPILE_LINUX
	pthread_exit((void*)t);
#endif
#ifdef _COMPILE_WINDOWS
	TerminateThread(t, 0);
	CloseHandle(t);
#endif
}


void InitThreadLock(THREAD_LOCK& t) {
#ifdef _COMPILE_LINUX
	t = PTHREAD_MUTEX_INITIALIZER;
	//UnlockThread(t);
	//pthread_mutex_init(&t, NULL);
#endif
#ifdef _COMPILE_WINDOWS
	InitializeCriticalSection(&t);
#endif
}

void LockThread(THREAD_LOCK& t) {
#ifdef _COMPILE_LINUX
	pthread_mutex_lock(&t);
#endif
#ifdef _COMPILE_WINDOWS
	EnterCriticalSection(&t);
#endif
}

void UnlockThread(THREAD_LOCK& t) {
#ifdef _COMPILE_LINUX
	pthread_mutex_unlock(&t);
#endif
#ifdef _COMPILE_WINDOWS
	LeaveCriticalSection(&t);
#endif
}


void sleep(int ms) {
#ifdef _COMPILE_LINUX
	usleep(ms * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
#ifdef _COMPILE_WINDOWS
	Sleep(ms);
#endif
}

