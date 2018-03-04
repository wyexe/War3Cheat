#ifndef __LIBTOOLS_INJECTORLIB_MODULEINJECTOR_NOTRACEMODULEINJECTOR_H__
#define __LIBTOOLS_INJECTORLIB_MODULEINJECTOR_NOTRACEMODULEINJECTOR_H__

#define WIN32_NO_STATUS
#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>
#include <string>


namespace libTools
{
	class CNoTraceModuleInjector
	{
	private:
#define FLAG_ON(_V, _F)     (!!((_V) & (_F)))
#define _TAG4(s) ( \
	(((s) >> 24) & 0xFF)       | \
	(((s) >> 8 ) & 0xFF00)     | \
	(((s) << 24) & 0xFF000000) | \
	(((s) << 8 ) & 0x00FF0000) \
	)
#define TAG4(s) _TAG4((UINT32)(s))

#define SET_FLAG(_V, _F)    ((_V) |= (_F))
#define CLEAR_FLAG(_V, _F)  ((_V) &= ~(_F))


#define LOAD_MEM_DLL_INFO_MAGIC  TAG4('LDFM')
#define LMD_REMOVE_PE_HEADER        0x00000001
#define LMD_REMOVE_IAT              0x00000002
#define LMD_REMOVE_EAT              0x00000004
#define LMD_REMOVE_RES              0x00000008
#define LMD_MAPPED_DLL              0x10000000

		typedef enum _SECTION_INHERIT
		{
			ViewShare = 1,
			ViewUnmap = 2
		} SECTION_INHERIT;


		typedef enum _SECTION_INFORMATION_CLASS
		{
			SectionBasicInformation,
			SectionImageInformation,
			SectionRelocationInformation,   // ret = now_base - desire_base
		} SECTION_INFORMATION_CLASS, *PSECTION_INFORMATION_CLASS;


		typedef struct _SECTION_BASIC_INFORMATION
		{
			PVOID           BaseAddress;
			ULONG           Attributes;
			LARGE_INTEGER   Size;
		} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;


		typedef struct _SECTION_IMAGE_INFORMATION
		{
			PVOID   TransferAddress;                    // 0x00
			ULONG   ZeroBits;                           // 0x04
			SIZE_T  MaximumStackSize;                   // 0x08
			SIZE_T  CommittedStackSize;                 // 0x0C
			ULONG   SubSystemType;                      // 0x10

			union
			{
				struct SubSystemVersionConten
				{
					USHORT SubSystemMinorVersion;
					USHORT SubSystemMajorVersion;
				}SubSystemVersionContent;
				ULONG SubSystemVersion;                 // 0x14
			};

			ULONG   GpValue;                            // 0x18
			USHORT  ImageCharacteristics;               // 0x1C
			USHORT  DllCharacteristics;                 // 0x1E
			USHORT  Machine;                            // 0x20
			UCHAR   ImageContainsCode;                  // 0x22
			union
			{
				UCHAR ImageFlags;                       // 0x23
				struct struc
				{
					UCHAR ComPlusNativeReady : 1;
					UCHAR ComPlusILOnly : 1;
					UCHAR ImageDynamicallyRelocated : 1;
					UCHAR ImageMappedFlat : 1;
				};
			} ImageFlags;

			ULONG   LoaderFlags;                        // 0x24
			ULONG   ImageFileSize;                      // 0x28
			ULONG   CheckSum;                           // 0x2C
		} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;


		typedef NTSTATUS(WINAPI *NtUnmapViewOfSectionPtr)(_In_ HANDLE  ProcessHandle, _In_ PVOID  BaseAddress);

		typedef NTSTATUS(WINAPI *NtOpenSectionPtr)(_Out_ PHANDLE SectionHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes);

		typedef NTSTATUS(WINAPI *NtOpenDirectoryObjectPtr)(_Out_ PHANDLE DirectoryHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes);

		typedef NTSTATUS(WINAPI *NtMapViewOfSectionPtr)(
			_In_ HANDLE  SectionHandle,
			_In_ HANDLE  ProcessHandle,
			_In_ _Out_ PVOID  *BaseAddress,
			_In_ ULONG  ZeroBits,
			_In_ ULONG  CommitSize,
			_In_ _Out_ PLARGE_INTEGER  SectionOffset,	// optional
			_In_ _Out_ PULONG  ViewSize,
			_In_ SECTION_INHERIT  InheritDisposition,
			_In_ ULONG  AllocationType,
			_In_ ULONG  Protect
			);

		typedef NTSTATUS(NTAPI *ZwLoadDriverPtr)(
			_In_ PUNICODE_STRING RegistryPath
			);

		typedef VOID(WINAPI *RtlInitUnicodeStringPtr)(
			_In_ _Out_ PUNICODE_STRING  DestinationString,
			_In_ PCWSTR  SourceString
			);

		typedef ULONG(WINAPI *RtlNtStatusToDosErrorPtr) (
			_In_ NTSTATUS Status
			);
		typedef NTSTATUS(WINAPI *NtClosePtr)(
			_In_ HANDLE  Object
			);
		typedef NTSTATUS(WINAPI *NtCreateFilePtr)(
			_Out_ PHANDLE FileHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ PLARGE_INTEGER AllocationSize OPTIONAL,
			_In_ ULONG FileAttributes,
			_In_ ULONG ShareAccess,
			_In_ ULONG CreateDisposition,
			_In_ ULONG CreateOptions,
			_In_ PVOID EaBuffer OPTIONAL,
			_In_ ULONG EaLength
			);

		typedef NTSTATUS(WINAPI *NtOpenFilePtr)(
			_Out_ PHANDLE FileHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ ULONG ShareAccess,
			_In_ ULONG OpenOptions
			);

		typedef NTSTATUS(WINAPI *NtCreateSectionPtr)(
			_Out_ PHANDLE SectionHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes,  // Optional
			_In_ PLARGE_INTEGER MaximumSize,           // Optional
			_In_ ULONG SectionPageProtection,
			_In_ ULONG AllocationAttributes,
			_In_ HANDLE FileHandle                     // Optional
			);
		typedef NTSTATUS(WINAPI *NtQuerySectionPtr)(
			_In_ HANDLE SectionHandle,
			_In_ SECTION_INFORMATION_CLASS SectionInformationClass,
			_Out_ PVOID SectionInformation,
			_In_ ULONG SectionInformationLength,
			_Out_ PULONG ResultLength OPTIONAL
			);

		typedef NTSTATUS(WINAPI *NtProtectVirtualMemoryPtr)(
			_In_ HANDLE ProcessHandle,
			_In_ _Out_ PVOID *BaseAddress,
			_In_ _Out_ PULONG ProtectSize,
			_In_ ULONG NewProtect,
			_Out_ PULONG OldProtect
			);

		typedef NTSTATUS(NTAPI *LdrLoadDllPtr)(
			_In_ PWCHAR               PathToFile OPTIONAL,
			_In_ PULONG                Flags OPTIONAL,
			_In_ PUNICODE_STRING      ModuleFileName,
			_Out_ PHANDLE             ModuleHandle);

		typedef LONG(NTAPI *RtlCompareUnicodeStringPtr)(
			PCUNICODE_STRING    String1,
			PCUNICODE_STRING    String2,
			BOOLEAN             CaseInSensitive
			);

		enum
		{
			LDM_STATE_QUERY_ATTRIBUTES,
			LDM_STATE_OPEN_FILE,
			LDM_STATE_CREATE_SECTION,
			LDM_STATE_MAP_SECTION,
			LDM_STATE_MANIFESOPEN_FILE,
			LDM_STATE_FINISH,

			LDM_STATE_MANIFESMASK = 0xFFFF0000,
			LDM_STATE_FLAG_MANIFESOPEN_FILE = 0x00010000,
		};


		typedef struct TEB_ACTIVE_FRAME_CONTEXT
		{
			/* 0x000 */ ULONG   Flags;
			/* 0x004 */ PSTR    FrameName;

		} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;


		typedef struct TEB_ACTIVE_FRAME
		{
			/* 0x000 */ ULONG                       Context;  // Flags;
			/* 0x004 */ struct TEB_ACTIVE_FRAME    *Previous;
			/* 0x008 */	PTEB_ACTIVE_FRAME_CONTEXT   pContext;
		} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;


		struct LOAD_MEM_DLL_INFO : public TEB_ACTIVE_FRAME
		{
			ULONG           Flags;
			PVOID           MappedBase;
			PVOID           MemDllBase;
			SIZE_T          DllBufferSize;
			SIZE_T          ViewSize;
			UNICODE_STRING  Lz32Path;

			union
			{
				HANDLE DllFileHandle;
				HANDLE SectionHandle;
			};

			UNICODE_STRING  MemDllFullPath;
		};


		typedef  VOID(NTAPI *RtlPushFramePtr)(PTEB_ACTIVE_FRAME Frame);
		typedef PTEB_ACTIVE_FRAME(NTAPI *RtlGetFramePtr)(VOID);
		typedef VOID(NTAPI *RtlPopFramePtr)(PTEB_ACTIVE_FRAME Frame);


		typedef struct _FILE_BASIC_INFORMATION
		{
			LARGE_INTEGER CreationTime;
			LARGE_INTEGER LastAccessTime;
			LARGE_INTEGER LastWriteTime;
			LARGE_INTEGER ChangeTime;
			ULONG FileAttributes;
		} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;


		typedef NTSTATUS(NTAPI *NtQueryAttributesFilePtr)(
			_In_  POBJECT_ATTRIBUTES      ObjectAttributes,
			_Out_ PFILE_BASIC_INFORMATION FileInformation
			);


		typedef BOOLEAN(NTAPI *RtlDosPathNameToNtPathName_UPtr)(
			_In_  PCWSTR                DosName,
			_Out_ PUNICODE_STRING      NtName,
			_Out_ PCWSTR              *DosFilePath OPTIONAL,
			_Out_ PVOID FileName OPTIONAL
			);


		typedef struct _LDR_FILE_
		{
			DWORD HeadTag;
			DWORD DosHeaderOffset;
			DWORD DosHeaderSize;
			DWORD PeHeaderOffset;
			DWORD PeHeaderSize;
			DWORD OtherDataOffset;
			DWORD OtherDataSize;
			BYTE Key[8];
			DWORD LdrFlags;
		}LDR_FILE, *PLDR_FILE;
	public:
		CNoTraceModuleInjector();
		~CNoTraceModuleInjector();

		BOOL SetModuleContent(_In_ CONST std::wstring& wsDllPath);
		VOID SetModuleContent(_In_ LPVOID pvFileContent, _In_ SIZE_T uSize);

		BOOL LoadModule();
	private:
		//
		BOOL InitLdr();
		
		//
		BOOL Hook();

		//
		BOOL UnHook();

		//
		NTSTATUS WINAPI LoadDllFromMemory(_In_ PUNICODE_STRING ModuleFileName, _In_ ULONG Flags);
	private:
		static PTEB_ACTIVE_FRAME FindThreadFrameByContext(_In_ ULONG_PTR Context);

		static LOAD_MEM_DLL_INFO* GetMemoryModuleInstance();

		static NTSTATUS WINAPI _NtOpenFile(_Out_ PHANDLE FileHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes, _Out_ PIO_STATUS_BLOCK IoStatusBlock, _In_ ULONG ShareAccess, _In_ ULONG OpenOptions);

		static NTSTATUS WINAPI _NtCreateSection(_Out_ PHANDLE SectionHandle, _In_ ACCESS_MASK DesiredAccess, _In_ POBJECT_ATTRIBUTES ObjectAttributes, _In_ PLARGE_INTEGER MaximumSize, _In_ ULONG SectionPageProtection, _In_ ULONG AllocationAttributes, _In_ HANDLE FileHandle);

		static NTSTATUS WINAPI _NtQuerySection(_In_ HANDLE SectionHandle, _In_ SECTION_INFORMATION_CLASS SectionInformationClass, _Out_ PVOID SectionInformation, _In_ ULONG SectionInformationLength, _Out_ PULONG ResultLength);

		static NTSTATUS WINAPI _NtMapViewOfSection(_In_ HANDLE SectionHandle, _In_ HANDLE ProcessHandle, _In_ _Out_ PVOID *BaseAddress, _In_ ULONG ZeroBits, _In_ ULONG CommitSize, _In_ _Out_ PLARGE_INTEGER SectionOffset, _In_ _Out_ PULONG ViewSize, _In_ SECTION_INHERIT InheritDisposition, _In_ ULONG AllocationType, _In_ ULONG Protect);

		static NTSTATUS WINAPI _NtClose(_In_ HANDLE Handle);

		static NTSTATUS WINAPI _NtQueryAttributesFile(_In_ POBJECT_ATTRIBUTES ObjectAttributes, _In_ PFILE_BASIC_INFORMATION FileInformation);
	private:
		LPVOID			_pvFileContent;
		SIZE_T			_uFileSize;
		BOOL			_IsAllocMemory;

		static RtlInitUnicodeStringPtr			_RtlInitUnicodeStringPtr;
		static NtUnmapViewOfSectionPtr			_NtUnmapViewOfSectionPtr;
		static NtOpenFilePtr					_NtOpenFilePtr;
		static NtOpenDirectoryObjectPtr			_NtOpenDirectoryObjectPtr;
		static NtOpenSectionPtr					_NtOpenSectionPtr;
		static NtMapViewOfSectionPtr			_NtMapViewOfSectionPtr;
		static RtlNtStatusToDosErrorPtr			_RtlNtStatusToDosErrorPtr;
		static NtClosePtr						_NtClosePtr;
		static NtCreateFilePtr					_NtCreateFilePtr;
		static NtCreateSectionPtr				_NtCreateSectionPtr;
		static NtQuerySectionPtr				_NtQuerySectionPtr;
		static LdrLoadDllPtr					_LdrLoadDllPtr;
		static RtlCompareUnicodeStringPtr		_RtlCompareUnicodeStringPtr;
		static RtlPushFramePtr					_RtlPushFramePtr;
		static RtlGetFramePtr					_RtlGetFramePtr;
		static RtlPopFramePtr					_RtlPopFramePtr;
		static NtQueryAttributesFilePtr			_NtQueryAttributesFilePtr;
		static RtlDosPathNameToNtPathName_UPtr	_RtlDosPathNameToNtPathName_UPtr;
		static NtOpenFilePtr					_NtOpenFilePtr_Old;
		static NtCreateSectionPtr				_NtCreateSectionPtr_Old;
		static NtQuerySectionPtr				_NtQuerySectionPtr_Old;
		static NtMapViewOfSectionPtr			_NtMapViewOfSectionPtr_Old;
		static NtClosePtr						_NtClosePtr_Old;
		static NtQueryAttributesFilePtr			_NtQueryAttributesFilePtr_Old;
	};
}

#endif // !__LIBTOOLS_INJECTORLIB_MODULEINJECTOR_NOTRACEMODULEINJECTOR_H__
