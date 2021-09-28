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

#pragma once
#include <switch.h>
#include <string>
#include "NSP/install/install.hpp"

#include <functional>
#include <vector>

#include <switch/types.h>
#include "NSP/install/pfs0.hpp"

#include "NSP/nx/ncm.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
namespace tin::install::nsp
{
    class NSP
    {
        protected:
            std::vector<u8> m_headerBytes;

            NSP();

        public:
            virtual void StreamToPlaceholder(std::shared_ptr<nx::ncm::ContentStorage>& contentStorage, NcmContentId placeholderId) = 0;
            virtual void BufferData(void* buf, off_t offset, size_t size) = 0;

            virtual void RetrieveHeader();
            virtual const PFS0BaseHeader* GetBaseHeader();
            virtual u64 GetDataOffset();

            virtual const PFS0FileEntry* GetFileEntry(unsigned int index);
            virtual const PFS0FileEntry* GetFileEntryByName(std::string name);
            virtual const PFS0FileEntry* GetFileEntryByNcaId(const NcmContentId& ncaId);
            virtual std::vector<const PFS0FileEntry*> GetFileEntriesByExtension(std::string extension);

            virtual const char* GetFileEntryName(const PFS0FileEntry* fileEntry);
    };
    class SDMCNSP : public NSP
    {
    public:
        SDMCNSP(std::string path);
        ~SDMCNSP();

        virtual void StreamToPlaceholder(std::shared_ptr<nx::ncm::ContentStorage>& contentStorage, NcmContentId ncaId) override;
        virtual void BufferData(void* buf, off_t offset, size_t size) override;
    private:
        FILE* m_nspFile;
    };
    class NSPInstall : public Install
    {
        private:
            const std::shared_ptr<NSP> m_NSP;

        protected:
            std::vector<std::tuple<nx::ncm::ContentMeta, NcmContentInfo>> ReadCNMT() override;
            void InstallNCA(const NcmContentId& ncaId) override;
            void InstallTicketCert() override;

        public:
            NSPInstall(NcmStorageId destStorageId, bool ignoreReqFirmVersion, const std::shared_ptr<NSP>& remoteNSP);
    };

}