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

#include <cstring>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <thread>
#include <memory>
#include "NSP/sdInstall.hpp"
#include "NSP/install/nsp.hpp"
#include "NSP/nx/fs.hpp"
#include "NSP/extra/error.hpp"
#include "NSP/extra/debug.h"
#include "NSP/nx/ipc/tin_ipc.h"

namespace nspInstStuff {

    void initInstallServices() {
        ncmInitialize();
        nsextInitialize();
        esInitialize();
        splCryptoInitialize();
        splInitialize();
    }

    void deinitInstallServices() {
        ncmExit();
        nsextExit();
        esExit();
        splCryptoExit();
        splExit();
    }

    bool installNspFromFile(std::vector<std::filesystem::path> ourTitleList, int whereToInstall)
    {
        initInstallServices();
        bool nspInstalled = true;
        NcmStorageId m_destStorageId = NcmStorageId_SdCard;

        if (whereToInstall) m_destStorageId = NcmStorageId_BuiltInUser;
        unsigned int titleItr;

        try
        {
            for (titleItr = 0; titleItr < ourTitleList.size(); titleItr++) {
                std::unique_ptr<tin::install::Install> installTask;

                auto sdmcNSP = std::make_shared<tin::install::nsp::SDMCNSP>(ourTitleList[titleItr]);
                installTask = std::make_unique<tin::install::nsp::NSPInstall>(m_destStorageId, true, sdmcNSP);

                LOG_DEBUG("%s\n", "Preparing installation");
                installTask->Prepare();
                installTask->Begin();
            }
        }
        catch (std::exception& e)
        {
            LOG_DEBUG("Failed to install");
            LOG_DEBUG("%s", e.what());
            fprintf(stdout, "%s", e.what());
            nspInstalled = false;
        }

        if(nspInstalled){
			LOG_DEBUG("Done\n");
		} else{
			LOG_DEBUG("fail\n");
		}
        deinitInstallServices();
        return nspInstalled;
    }
}
