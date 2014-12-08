/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/

#include "Common.h"
#include "UserIPCPipeClient.h"
#include "IPCServiceMain.h"

#ifdef NIX
#include "IPCServerI.h"
typedef void* (*FactoryFn)(const char*);
#endif

using namespace UserCore;


UserIPCPipeClient::UserIPCPipeClient(const char* user, const char* appDataPath, bool uploadDumps)
	: IPC::PipeClient("DesuraIS")
	, m_szUser(user)
	, m_szAppDataPath(appDataPath)
	, m_bUploadDumps(uploadDumps)
{
#ifdef WIN32
	onDisconnectEvent += delegate(this, &UserIPCPipeClient::onDisconnect);
#else
	m_pServer = nullptr;
#endif
}

UserIPCPipeClient::~UserIPCPipeClient()
{
	m_pServiceMain.reset();

#ifdef WIN32
	try
	{
		stopService();
	}
	catch (...)
	{
	}
#else
	IPC::PipeClient::disconnect(false);
	if (m_pServer)
	{
		m_pServer->destroy();
		m_pServer = nullptr;
	}
#endif
}

#ifdef WIN32
	void UserIPCPipeClient::restart()
	{
		m_pServiceMain.reset();
		IPC::PipeClient::restart();
		start();
	}

	void UserIPCPipeClient::onDisconnect()
	{
		Warning("Desura Service Disconnected unexpectedly! Attempting restart.\n");
	}

	void UserIPCPipeClient::start()
	{
		try
		{
			startService();

			setUpPipes();
			IPC::PipeClient::start();

			m_pServiceMain = IPC::CreateIPCClass<IPCServiceMain>(this, "IPCServiceMain");

			if (!m_pServiceMain)
				throw gcException(ERR_IPC, "Failed to create service main");

			m_pServiceMain->dispVersion();
			m_pServiceMain->setCrashSettings(m_szUser.c_str(), m_bUploadDumps);
			m_pServiceMain->setAppDataPath(m_szAppDataPath.c_str());
		}
		catch (gcException &e)
		{
			throw gcException((ERROR_ID)e.getErrId(), e.getSecErrId(), gcString("Failed to start desura service: {0}", e));
		}
	}


	void UserIPCPipeClient::startService()
	{

	#ifdef DEBUG
	#if 0
		//service started via debugger
		return;
	#endif
	#endif

		uint32 res = UTIL::WIN::queryService(SERVICE_NAME);

		if (res != SERVICE_STATUS_STOPPED)
		{
			try
			{
				UTIL::WIN::stopService(SERVICE_NAME);
				gcSleep(500);
			}
			catch (gcException &)
			{
			}
		}

		std::vector<std::string> args;

		args.push_back("-wdir");
		args.push_back(gcString(UTIL::OS::getCurrentDir()));

		UTIL::WIN::startService(SERVICE_NAME, args);

		uint32 count = 0;
		while (UTIL::WIN::queryService(SERVICE_NAME) != SERVICE_STATUS_RUNNING)
		{
			//wait five seconds
			if (count > 50)
				throw gcException(ERR_SERVICE, "Failed to start desura Service (PipeManager).");

			gcSleep(100);
			count++;
		}
	}

	void UserIPCPipeClient::stopService()
	{
		UTIL::WIN::stopService(SERVICE_NAME);
	}
#else
	void UserIPCPipeClient::start()
	{
		if (!m_hServiceDll.load("libservicecore.so"))
			throw gcException(ERR_INVALID, gcString("Failed to load service core: {0}", dlerror()));

		FactoryFn factory = m_hServiceDll.getFunction<FactoryFn>("FactoryBuilderSC");

		if (!factory)
			throw gcException(ERR_INVALID, "Failed to get factory function");

		m_pServer = (IPCServerI*)factory(IPC_SERVER);

		if (!factory)
			throw gcException(ERR_INVALID, "Failed to create server");

		m_pServer->setSendCallback((void*)this, &UserIPCPipeClient::recvMessage);
		setSendCallback((void*)this, &UserIPCPipeClient::sendMessage);

		IPC::PipeClient::start();

		m_pServiceMain = IPC::CreateIPCClass<IPCServiceMain>(this, "IPCServiceMain");

		if (!m_pServiceMain)
			throw gcException(ERR_IPC, "Failed to create service main");

		m_pServiceMain->dispVersion();
		m_pServiceMain->setCrashSettings(m_szUser.c_str(), m_bUploadDumps);
		m_pServiceMain->setAppDataPath(m_szAppDataPath.c_str());
	}

	void UserIPCPipeClient::setUpPipes()
	{

	}

	void UserIPCPipeClient::recvMessage(void* obj, const char* buffer, size_t size)
	{
		UserIPCPipeClient* c = (UserIPCPipeClient*)obj;
		c->recvMessage(buffer, size);
	}

	void UserIPCPipeClient::recvMessage(const char* buffer, size_t size)
	{
		IPC::PipeBase::recvMessage(buffer, size);
	}

	void UserIPCPipeClient::sendMessage(void* obj, const char* buffer, size_t size)
	{
		UserIPCPipeClient* c = (UserIPCPipeClient*)obj;
		c->sendMessage(buffer, size);
	}

	void UserIPCPipeClient::sendMessage(const char* buffer, size_t size)
	{
		m_pServer->sendMessage(buffer, size);
	}
#endif
