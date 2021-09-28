/*
Copyright (c) 2017-2018 Adubbz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "NSP/install/nsp.hpp"

#include <threads.h>
#include "NSP/data/buffered_placeholder_writer.hpp"
#include "NSP/extra/title_util.hpp"
#include "NSP/extra/error.hpp"
#include "NSP/extra/debug.h"
#include "NSP/nx/nca_writer.h"

#include "NSP/install/nca.hpp"
#include "NSP/nx/fs.hpp"
#include "NSP/nx/ncm.hpp"
#include "NSP/extra/crypto.hpp"
#include "NSP/extra/file_util.hpp"
#include "NSP/nx/ipc/es.h"


namespace tin::install::nsp
{
    NSP::NSP() {}

    // TODO: Do verification: PFS0 magic, sizes not zero
    void NSP::RetrieveHeader()
    {
        LOG_DEBUG("Retrieving remote NSP header...\n");

        // Retrieve the base header
        m_headerBytes.resize(sizeof(PFS0BaseHeader), 0);
        this->BufferData(m_headerBytes.data(), 0x0, sizeof(PFS0BaseHeader));

        LOG_DEBUG("Base header: \n");
        printBytes(m_headerBytes.data(), sizeof(PFS0BaseHeader), true);

        // Retrieve the full header
        size_t remainingHeaderSize = this->GetBaseHeader()->numFiles * sizeof(PFS0FileEntry) + this->GetBaseHeader()->stringTableSize;
        m_headerBytes.resize(sizeof(PFS0BaseHeader) + remainingHeaderSize, 0);
        this->BufferData(m_headerBytes.data() + sizeof(PFS0BaseHeader), sizeof(PFS0BaseHeader), remainingHeaderSize);

        LOG_DEBUG("Full header: \n");
        printBytes(m_headerBytes.data(), m_headerBytes.size(), true);
    }

    const PFS0FileEntry* NSP::GetFileEntry(unsigned int index)
    {
        if (index >= this->GetBaseHeader()->numFiles)
            THROW_FORMAT("File entry index is out of bounds\n");

        size_t fileEntryOffset = sizeof(PFS0BaseHeader) + index * sizeof(PFS0FileEntry);

        if (m_headerBytes.size() < fileEntryOffset + sizeof(PFS0FileEntry))
            THROW_FORMAT("Header bytes is too small to get file entry!");

        return reinterpret_cast<PFS0FileEntry*>(m_headerBytes.data() + fileEntryOffset);
    }

    std::vector<const PFS0FileEntry*> NSP::GetFileEntriesByExtension(std::string extension)
    {
        std::vector<const PFS0FileEntry*> entryList;

        for (unsigned int i = 0; i < this->GetBaseHeader()->numFiles; i++)
        {
            const PFS0FileEntry* fileEntry = this->GetFileEntry(i);
            std::string name(this->GetFileEntryName(fileEntry));
            auto foundExtension = name.substr(name.find(".") + 1); 

            if (foundExtension == extension)
                entryList.push_back(fileEntry);
        }

        return entryList;
    }

    const PFS0FileEntry* NSP::GetFileEntryByName(std::string name)
    {
        for (unsigned int i = 0; i < this->GetBaseHeader()->numFiles; i++)
        {
            const PFS0FileEntry* fileEntry = this->GetFileEntry(i);
            std::string foundName(this->GetFileEntryName(fileEntry));

            if (foundName == name)
                return fileEntry;
        }

        return nullptr;
    }

    const PFS0FileEntry* NSP::GetFileEntryByNcaId(const NcmContentId& ncaId)
    {
        const PFS0FileEntry* fileEntry = nullptr;
        std::string ncaIdStr = tin::util::GetNcaIdString(ncaId);

        if ((fileEntry = this->GetFileEntryByName(ncaIdStr + ".nca")) == nullptr)
        {
            if ((fileEntry = this->GetFileEntryByName(ncaIdStr + ".cnmt.nca")) == nullptr)
            {
                    if ((fileEntry = this->GetFileEntryByName(ncaIdStr + ".ncz")) == nullptr)
                    {
                         if ((fileEntry = this->GetFileEntryByName(ncaIdStr + ".cnmt.ncz")) == nullptr)
                         {
                              return nullptr;
                         }
                    }
            }
        }

        return fileEntry;
    }

    const char* NSP::GetFileEntryName(const PFS0FileEntry* fileEntry)
    {
        u64 stringTableStart = sizeof(PFS0BaseHeader) + this->GetBaseHeader()->numFiles * sizeof(PFS0FileEntry);
        return reinterpret_cast<const char*>(m_headerBytes.data() + stringTableStart + fileEntry->stringTableOffset);
    }

    const PFS0BaseHeader* NSP::GetBaseHeader()
    {
        if (m_headerBytes.empty())
            THROW_FORMAT("Cannot retrieve header as header bytes are empty. Have you retrieved it yet?\n");

        return reinterpret_cast<PFS0BaseHeader*>(m_headerBytes.data());
    }

    u64 NSP::GetDataOffset()
    {
        if (m_headerBytes.empty())
            THROW_FORMAT("Cannot get data offset as header is empty. Have you retrieved it yet?\n");

        return m_headerBytes.size();
    }

	{//SDMC class	
		SDMCNSP::SDMCNSP(std::string path)
		{
			m_nspFile = fopen((path).c_str(), "rb");
			if (!m_nspFile)
				THROW_FORMAT("can't open file at %s\n", path.c_str());
		}

		SDMCNSP::~SDMCNSP()
		{
			fclose(m_nspFile);
		}

		void SDMCNSP::StreamToPlaceholder(std::shared_ptr<nx::ncm::ContentStorage>& contentStorage, NcmContentId ncaId)
		{
			const PFS0FileEntry* fileEntry = this->GetFileEntryByNcaId(ncaId);
			std::string ncaFileName = this->GetFileEntryName(fileEntry);

			LOG_DEBUG("Retrieving %s\n", ncaFileName.c_str());
			size_t ncaSize = fileEntry->fileSize;

			NcaWriter writer(ncaId, contentStorage);

			float progress;

			u64 fileStart = GetDataOffset() + fileEntry->dataOffset;
			u64 fileOff = 0;
			size_t readSize = 0x400000; // 4MB buff
			auto readBuffer = std::make_unique<u8[]>(readSize);

			try
			{
			   // inst::ui::instPage::setInstInfoText("inst.info_page.top_info0"_lang + ncaFileName + "...");
				//inst::ui::instPage::setInstBarPerc(0);
				while (fileOff < ncaSize)
				{
					progress = (float) fileOff / (float) ncaSize;

					if (fileOff % (0x400000 * 3) == 0) {
						LOG_DEBUG("> Progress: %lu/%lu MB (%d%s)\r", (fileOff / 1000000), (ncaSize / 1000000), (int)(progress * 100.0), "%");
					   // inst::ui::instPage::setInstBarPerc((double)(progress * 100.0));
					}

					if (fileOff + readSize >= ncaSize) readSize = ncaSize - fileOff;

					this->BufferData(readBuffer.get(), fileOff + fileStart, readSize);
					writer.write(readBuffer.get(), readSize);

					fileOff += readSize;
				}
				//inst::ui::instPage::setInstBarPerc(100);
			}
			catch (std::exception& e)
			{
				LOG_DEBUG("something went wrong: %s\n", e.what());
			}

			writer.close();
		}

		void SDMCNSP::BufferData(void* buf, off_t offset, size_t size)
		{
			fseeko(m_nspFile, offset, SEEK_SET);
			fread(buf, 1, size, m_nspFile);
		}		
	}
	{//class NSPInstal
		NSPInstall::NSPInstall(NcmStorageId destStorageId, bool ignoreReqFirmVersion, const std::shared_ptr<NSP>& remoteNSP) :
			Install(destStorageId, ignoreReqFirmVersion), m_NSP(remoteNSP)
		{
			m_NSP->RetrieveHeader();
		}

		std::vector<std::tuple<nx::ncm::ContentMeta, NcmContentInfo>> NSPInstall::ReadCNMT()
		{
			std::vector<std::tuple<nx::ncm::ContentMeta, NcmContentInfo>> CNMTList;

			for (const PFS0FileEntry* fileEntry : m_NSP->GetFileEntriesByExtension("cnmt.nca")) {
				std::string cnmtNcaName(m_NSP->GetFileEntryName(fileEntry));
				NcmContentId cnmtContentId = tin::util::GetNcaIdFromString(cnmtNcaName);
				size_t cnmtNcaSize = fileEntry->fileSize;

				nx::ncm::ContentStorage contentStorage(m_destStorageId);

				LOG_DEBUG("CNMT Name: %s\n", cnmtNcaName.c_str());

				// We install the cnmt nca early to read from it later
				this->InstallNCA(cnmtContentId);
				std::string cnmtNCAFullPath = contentStorage.GetPath(cnmtContentId);

				NcmContentInfo cnmtContentInfo;
				cnmtContentInfo.content_id = cnmtContentId;
				*(u64*)&cnmtContentInfo.size = cnmtNcaSize & 0xFFFFFFFFFFFF;
				cnmtContentInfo.content_type = NcmContentType_Meta;

				CNMTList.push_back( { tin::util::GetContentMetaFromNCA(cnmtNCAFullPath), cnmtContentInfo } );
			}

			return CNMTList;
		}

		void NSPInstall::InstallNCA(const NcmContentId& ncaId)
		{
			const PFS0FileEntry* fileEntry = m_NSP->GetFileEntryByNcaId(ncaId);
			std::string ncaFileName = m_NSP->GetFileEntryName(fileEntry);

			#ifdef NXLINK_DEBUG
			size_t ncaSize = fileEntry->fileSize;
			LOG_DEBUG("Installing %s to storage Id %u\n", ncaFileName.c_str(), m_destStorageId);
			#endif

			std::shared_ptr<nx::ncm::ContentStorage> contentStorage(new nx::ncm::ContentStorage(m_destStorageId));

			// Attempt to delete any leftover placeholders
			try {
				contentStorage->DeletePlaceholder(*(NcmPlaceHolderId*)&ncaId);
			}
			catch (...) {}

			LOG_DEBUG("Size: 0x%lx\n", ncaSize);
	/*
			if (false && !m_declinedValidation)
			{
				tin::install::NcaHeader* header = new NcaHeader;
				m_NSP->BufferData(header, m_NSP->GetDataOffset() + fileEntry->dataOffset, sizeof(tin::install::NcaHeader));

				Crypto::AesXtr crypto(Crypto::Keys().headerKey, false);
				crypto.decrypt(header, header, sizeof(tin::install::NcaHeader), 0, 0x200);

				if (header->magic != MAGIC_NCA3)
					THROW_FORMAT("Invalid NCA magic");

				if (!Crypto::rsa2048PssVerify(&header->magic, 0x200, header->fixed_key_sig, Crypto::NCAHeaderSignature))
				{
					m_declinedValidation = true;
				}
				delete header;
			}
	*/
			m_NSP->StreamToPlaceholder(contentStorage, ncaId);

			LOG_DEBUG("Registering placeholder...\n");

			try
			{
				contentStorage->Register(*(NcmPlaceHolderId*)&ncaId, ncaId);
			}
			catch (...)
			{
				LOG_DEBUG(("Failed to register " + ncaFileName + ". It may already exist.\n").c_str());
			}

			try
			{
				contentStorage->DeletePlaceholder(*(NcmPlaceHolderId*)&ncaId);
			}
			catch (...) {}
		}

		void NSPInstall::InstallTicketCert()
		{
			// Read the tik files and put it into a buffer
			std::vector<const PFS0FileEntry*> tikFileEntries = m_NSP->GetFileEntriesByExtension("tik");
			std::vector<const PFS0FileEntry*> certFileEntries = m_NSP->GetFileEntriesByExtension("cert");

			for (size_t i = 0; i < tikFileEntries.size(); i++)
			{
				if (tikFileEntries[i] == nullptr) {
					LOG_DEBUG("Remote tik file is missing.\n");
					THROW_FORMAT("Remote tik file is not present!");
				}

				u64 tikSize = tikFileEntries[i]->fileSize;
				auto tikBuf = std::make_unique<u8[]>(tikSize);
				LOG_DEBUG("> Reading tik\n");
				m_NSP->BufferData(tikBuf.get(), m_NSP->GetDataOffset() + tikFileEntries[i]->dataOffset, tikSize);

				if (certFileEntries[i] == nullptr)
				{
					LOG_DEBUG("Remote cert file is missing.\n");
					THROW_FORMAT("Remote cert file is not present!");
				}

				u64 certSize = certFileEntries[i]->fileSize;
				auto certBuf = std::make_unique<u8[]>(certSize);
				LOG_DEBUG("> Reading cert\n");
				m_NSP->BufferData(certBuf.get(), m_NSP->GetDataOffset() + certFileEntries[i]->dataOffset, certSize);

				// Finally, let's actually import the ticket
				ASSERT_OK(esImportTicket(tikBuf.get(), tikSize, certBuf.get(), certSize), "Failed to import ticket");
			}
		}
	}
}