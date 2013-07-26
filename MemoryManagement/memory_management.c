///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>


#define BUF_SIZE 256
#define N_COMMAND 1

#define BUF_FFD_SIZE (sizeof(ffdSharedData))
#define BUF_MODELICA_SIZE (sizeof(ModelicaSharedData))
#define BUF_COMMAND_SIZE (N_COMMAND*sizeof(INT))

//TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szName[]=TEXT("MyFileMappingObject");

TCHAR ffdDataName[] = TEXT("FFDDataMappingObject");
TCHAR modelicaDataName[] = TEXT("ModelicaDataMappingObject");
TCHAR commandDataName[] = TEXT("commandMappingObject");

typedef struct {
  float number[3];
  int command;
  char message[20];
}ffdSharedData;

typedef struct {
  float number;
  int command;
  float arr[3];
  char message[20];
}ModelicaSharedData;



/******************************************************************************
 Allocate memory for data sharing 
******************************************************************************/
int main( )
{
  HANDLE ffdDataMapFile;
  HANDLE modelicaDataMapFile;
  HANDLE commandDataMapFile;
  LPCTSTR ffdDataBuf;
  LPCTSTR modelicaDataBuf;

  ffdSharedData ffdData;
  ModelicaSharedData modelicaData;

  /////////////////////////////////////////////////////////////////////////////
  // Fixme: Feak Data for Debug
  modelicaData.arr[0] = 0;
  modelicaData.arr[1] = 1;
  modelicaData.arr[2] = 2;
  modelicaData.number = 3.14159;
  modelicaData.command = 10;
  strcpy(modelicaData.message, "This is Modelica data\0");

  /*---------------------------------------------------------------------------
  | Create named file mapping objects for specified files
  ---------------------------------------------------------------------------*/
  ffdDataMapFile = CreateFileMapping(
                INVALID_HANDLE_VALUE,    // use paging file
                NULL,                    // default security
                PAGE_READWRITE,          // read/write access
                0,                       // maximum object size (high-order DWORD)
                BUF_FFD_SIZE,                // maximum object size (low-order DWORD)
                ffdDataName);                 // name of mapping object
  modelicaDataMapFile = CreateFileMapping(
                INVALID_HANDLE_VALUE,    // use paging file
                NULL,                    // default security
                PAGE_READWRITE,          // read/write access
                0,                       // maximum object size (high-order DWORD)
                BUF_MODELICA_SIZE,                // maximum object size (low-order DWORD)
                modelicaDataName);                 // name of mapping object

  // Send warning if can not create shared memory
  if(ffdDataMapFile==NULL)
  {
    printf("Could not create file mapping object (%d).\n", 
            GetLastError());
    return 1;
  }
  if(modelicaDataMapFile==NULL)
  {
    printf("Could not create file mapping object (%d).\n", 
            GetLastError());
    return 1;
  }

  // Fixme: The folowing are testing only
  /*---------------------------------------------------------------------------
  | Mps a view of a file mapping into the address space of a calling process
  ---------------------------------------------------------------------------*/
  ffdDataBuf = (LPTSTR) MapViewOfFile(ffdDataMapFile,   // handle to map object
                      FILE_MAP_ALL_ACCESS, // read/write permission
                      0,
                      0,
                      BUF_FFD_SIZE);
  modelicaDataBuf = (LPTSTR) MapViewOfFile(ffdDataMapFile,   // handle to map object
                      FILE_MAP_ALL_ACCESS, // read/write permission
                      0,
                      0,
                      BUF_MODELICA_SIZE);

  if(ffdDataBuf == NULL)
  {
    printf("Could not map view of file (%d).\n",
            GetLastError());
    CloseHandle(ffdDataMapFile);
    return 1;
  }

  if(modelicaDataBuf == NULL) 
  {
    printf("Could not map view of file (%d).\n",
            GetLastError());
    CloseHandle(modelicaDataMapFile);
    return 1;
  }

  getchar();
  // Copy a block of memory from szMsg to pBuf
  CopyMemory(&ffdData, (PVOID)ffdDataBuf, sizeof(ffdSharedData));
  CopyMemory((PVOID)modelicaDataBuf, &modelicaData, sizeof(modelicaData));


  printf("ffdData:\n");
  printf("number[0]=%f, number[1]=%f, number[2]=%f\n", ffdData.number[0], ffdData.number[1], ffdData.number[2]);
  printf("command=%d\n", ffdData.command);
  printf("message=%s\n", ffdData.message);

  getchar();
  UnmapViewOfFile(ffdDataBuf);
  UnmapViewOfFile(modelicaDataBuf);
  CloseHandle(ffdDataMapFile);
  CloseHandle(modelicaDataMapFile);

  return 0;
}