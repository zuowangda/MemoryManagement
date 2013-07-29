///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256

#define BUF_FFD_SIZE (sizeof(ffdSharedData))
#define BUF_MODELICA_SIZE (sizeof(ModelicaSharedData))


TCHAR ffdDataName[] = TEXT("FFDDataMappingObject");
TCHAR modelicaDataName[] = TEXT("ModelicaDataMappingObject");


typedef struct {
  float t;
  int status;
  float number[3];
  char message[20];
}ffdSharedData;

typedef struct {
  float t;
  int status;
  float arr[3];
  char message[30];
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
  //LPCTSTR modelicaDataBuf;
  ModelicaSharedData *modelicaDataBuf;

  ffdSharedData ffdData;
  ModelicaSharedData modelicaData;

  /*---------------------------------------------------------------------------
 | Initialize the memory
  ---------------------------------------------------------------------------*/
  modelicaData.t = 0;
  modelicaData.status = -1;
  modelicaData.arr[0] = -1;
  modelicaData.arr[1] = -1;
  modelicaData.arr[2] = -2;

  strcpy(modelicaData.message, "This is feak Modelica data\0");

  ffdData.t = 0;
  ffdData.status = -1;
  ffdData.number[0] = -1;
  ffdData.number[1] = -1;
  ffdData.number[2] = -2;

  strcpy(ffdData.message, "This is feak FFD data\0");

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

  /*---------------------------------------------------------------------------
  | Mps a view of a file mapping into the address space of a calling process
  ---------------------------------------------------------------------------*/
  ffdDataBuf = (LPTSTR) MapViewOfFile(ffdDataMapFile,   // handle to map object
                      FILE_MAP_ALL_ACCESS, // read/write permission
                      0,
                      0,
                      BUF_FFD_SIZE);
  modelicaDataBuf = (ModelicaSharedData *) MapViewOfFile(modelicaDataMapFile,   // handle to map object
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

  // Initialize the memory
  CopyMemory((PVOID)modelicaDataBuf, &modelicaData, sizeof(modelicaData));

  printf("Created memory for data\n");
  // Initialize the memory
  // Copy a block of memory from szMsg to pBuf
  CopyMemory((PVOID)ffdDataBuf, &ffdData, sizeof(ffdSharedData));

  // If no termination command, keep running
  while(modelicaDataBuf->status != -2)
    Sleep(10);

  UnmapViewOfFile(ffdDataBuf);
  UnmapViewOfFile(modelicaDataBuf);
  CloseHandle(ffdDataMapFile);
  CloseHandle(modelicaDataMapFile);

  return 0;
}


