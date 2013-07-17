#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>


#define BUF_SIZE 256
#define N_FFD 10
#define N_MODELICA 10
#define N_COMMAND 1
#define BUF_FFD_SIZE (N_FFD*sizeof(double))
#define BUF_MODELICA_SIZE ((N_Modelica+1)*sizeof(double))
#define BUF_COMMAND_SIZE (N_COMMAND*sizeof(int))

//TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szName[]=TEXT("MyFileMappingObject");
TCHAR szMsg[]=TEXT("Message from first process.");

TCHAR ffdDataName[] = TEXT("FFDDataMappingObject");
TCHAR modelicaDataName[] = TEXT("ModelicaDataMappingObject");
TCHAR commandDataName[] = TEXT("commandMappingObject");



/******************************************************************************
 Allocate memory for data sharing 
******************************************************************************/
int main( )
{
  double ffdData[N_FFD];
  double modelicaData[N_MODELICA];
  int commandData[N_COMMAND];

  HANDLE hMapFile;
  LPCTSTR pBuf;
  HANDLE ffdDataMapFile;
  LPCTSTR ffdDataBuf;
  HANDLE modelicaDataMapFile;
  LPCTSTR modelicaDataBuf;
  HANDLE commandDataMapFile;
  LPCTSTR commandDataBuf;

  int i;
  for(i=0; i<10; i++)
  {
    ffdData[i] = (double)i;
    printf("ffdData[%d] =%f\n", i, ffdData[i]);
  }

  getchar();

  commandData[0] = 1;
  /*---------------------------------------------------------------------------
  | Create a named file mapping object for a specified file
  ---------------------------------------------------------------------------*/
  ffdDataMapFile = CreateFileMapping(
                INVALID_HANDLE_VALUE,    // use paging file
                NULL,                    // default security
                PAGE_READWRITE,          // read/write access
                0,                       // maximum object size (high-order DWORD)
                BUF_FFD_SIZE,                // maximum object size (low-order DWORD)
                ffdDataName);                 // name of mapping object
  commandDataMapFile = CreateFileMapping(
                INVALID_HANDLE_VALUE,    // use paging file
                NULL,                    // default security
                PAGE_READWRITE,          // read/write access
                0,                       // maximum object size (high-order DWORD)
                BUF_COMMAND_SIZE,                // maximum object size (low-order DWORD)
                commandDataName);                 // name of mapping object

  // Send warning if can not create shared memory
  if(ffdDataMapFile==NULL)
  {
    _tprintf(TEXT("Could not create file mapping object (%d).\n"),
            GetLastError());
    return 1;
  }
  if(commandDataMapFile==NULL)
  {
    _tprintf(TEXT("Could not create file mapping object (%d).\n"),
            GetLastError());
    return 1;
  }

  /*---------------------------------------------------------------------------
  | Mps a view of a file mapping into the address space of a calling process
  ---------------------------------------------------------------------------*/
  ffdDataBuf = (LPTSTR) MapViewOfFile(ffdDataMapFile,   // handle to map object
                      FILE_MAP_ALL_ACCESS, // read/write permission
                      0,
                      0,
                      BUF_FFD_SIZE);
  commandDataBuf = (LPTSTR) MapViewOfFile(ffdDataMapFile,   // handle to map object
                      FILE_MAP_ALL_ACCESS, // read/write permission
                      0,
                      0,
                      BUF_COMMAND_SIZE);

  //modelicaDataBuf = (LPTSTR) MapViewOfFile(modelicaDataMapFile,   // handle to map object
  //                    FILE_MAP_ALL_ACCESS, // read/write permission
  //                    0,
  //                    0,
  //                    BUF_DATA_SIZE);
  if(ffdDataBuf == NULL)
  {
    _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());
      CloseHandle(ffdDataMapFile);
    return 1;
  }

  if(commandDataBuf == NULL) 
  {
    _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());
      CloseHandle(commandDataMapFile);
    return 1;
  }

  // Copy a block of memory from szMsg to pBuf
  CopyMemory((PVOID)ffdDataBuf, ffdData, (N_FFD * sizeof(double)));
  CopyMemory((PVOID)commandDataBuf, commandData, (N_COMMAND * sizeof(int)));

  while(commandDataBuf[0] > 0){
    Sleep(10);
  }

  UnmapViewOfFile(ffdDataBuf);
  UnmapViewOfFile(commandDataBuf);
  CloseHandle(ffdDataMapFile);
  CloseHandle(commandDataMapFile);

  return 0;
}