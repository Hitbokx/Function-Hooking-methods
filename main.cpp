#include<Windows.h>
#include<iostream>

bool hook( void* toHook, void* ourFunc(), int length )
{
	if( length < 5 )
		return false;

	DWORD oldProtect{};
	VirtualProtect( toHook, length, PAGE_EXECUTE_READWRITE, &oldProtect );
	memset( toHook, 0x90, length );

	DWORD relativeAddr{ (DWORD)ourFunc - ((DWORD)toHook + 5) };

	*(BYTE*)toHook = 0xE9;                            //jmp
	*(DWORD*)((DWORD)toHook + 1) = relativeAddr;      // to relative address

	VirtualProtect( toHook, length, oldProtect, &oldProtect );

	return true;
}

void __declspec(naked) ourFunc( DWORD jmpBackAdd )
{
	__asm
	{
		;asm code here
	}
}

DWORD WINAPI myThreadProc( HMODULE hModule )
{
	// Create console

	AllocConsole( ); // To allocate console for logging
	FILE* f;
	freopen_s( &f, "CONOUT$", "w", stdout );
	std::cout << "OG for a fee, stay sippin' fam\n\n";

	while ( !GetAsyncKeyState( VK_END ) & 1 )
	{
		// key input

		Sleep( 5 );
	}

	// Cleanup & eject
	if ( f )
		fclose( f );
	FreeConsole( );
	FreeLibraryAndExitThread( hModule, 0 );

	return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	switch ( fdwReason )
	{
		case DLL_PROCESS_ATTACH:

			::DisableThreadLibraryCalls( hinstDLL );
			CloseHandle( CreateThread( nullptr, 0, LPTHREAD_START_ROUTINE( myThreadProc ), hinstDLL, 0, nullptr ) );
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			//perform any necesary clean-up
			break;

	}
	return TRUE; //Succesful DLL_PROCESS_ATTACH
}