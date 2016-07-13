#include "stdafx.h"
#include "CoreFx.h"

#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <share.h>

//--------------------------------------------------------------------------------------
bool LoadDDSDataFromFile(const std::string & fileName,
	std::unique_ptr<uint8_t[]>& ddsData,
	DDS_HEADER** header,
	uint8_t** bitData,
	size_t* bitSize
)
{
	int file = -1;
	bool ret = false;

	errno_t err = _sopen_s(&file, fileName.c_str(), _O_BINARY | _O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (err != 0)
	{
		PRINT_ERROR("Loading '%s' failed : cannot open the file ! (errno = %i)\n", fileName.c_str(), err);
		return false;
	}

	__int64 fileSize = _filelength(file);

	// File is too big for 32-bit allocation, so reject read
	if (fileSize > ULONG_MAX)
	{
		PRINT_ERROR("Loading '%s' failed : the file is too big!\n", fileName.c_str());
		goto EXIT_PROC;
	}

	// Need at least enough data to fill the header and magic number to be a valid DDS
	if (fileSize < (sizeof(DDS_HEADER) + sizeof(DWORD)))
	{
		PRINT_ERROR("Loading '%s' failed : the file is too small!\n", fileName.c_str());
		goto EXIT_PROC;
	}

	// create enough space for the file data
	ddsData.reset(new uint8_t[fileSize]);
	if (!ddsData)
	{
		PRINT_ERROR("Loading '%s' failed : out of memory!\n", fileName.c_str());
		goto EXIT_PROC;
	}

	// read the data in
	uint32_t bytesread = 0;
	if ((bytesread = _read(file, ddsData.get(), (uint32_t)fileSize)) != fileSize)
	{
		PRINT_ERROR("Loading '%s' failed : read error (%li/%li byte(s) read)!\n", fileName.c_str(), bytesread, fileSize);
		goto EXIT_PROC;
	}

	// DDS files always start with the same magic number ("DDS ")
	uint32_t dwMagicNumber = *(const uint32_t*)(ddsData.get());
	if (dwMagicNumber != DDS_MAGIC)
	{
		PRINT_ERROR("Loading '%s' failed : the file does not contain the DDS signature!\n", fileName.c_str());
		goto EXIT_PROC;
	}

	DDS_HEADER* hdr = reinterpret_cast<DDS_HEADER*>(ddsData.get() + sizeof(uint32_t));

	// Verify header to validate DDS file
	if (hdr->dwSize != sizeof(DDS_HEADER)
		|| hdr->ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
	{
		PRINT_ERROR("Loading '%s' failed : wrong DDS header!\n", fileName.c_str());
		goto EXIT_PROC;
	}

	ptrdiff_t offset = sizeof(uint32_t) + sizeof(DDS_HEADER);

	if (hdr->ddspf.dwFlags & DDS_FOURCC)
	{
		// Check for DX10 extension
		if (MAKEFOURCC('D', 'X', '1', '0') == hdr->ddspf.dwFourCC)
		{
			// Must be long enough for both headers and magic value
			if (fileSize < (sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10)))
			{
				PRINT_ERROR("Loading '%s' failed : the file should have the DXT10 header!\n", fileName.c_str());
				goto EXIT_PROC;
			}

			offset += sizeof(DDS_HEADER_DXT10);
		}
	}

	// setup the pointers in the process request
	*header = hdr;
	*bitData = ddsData.get() + offset;
	*bitSize = fileSize - offset;

	ret = true;

EXIT_PROC:
	if (file != -1)
		_close(file);

	return ret;
}