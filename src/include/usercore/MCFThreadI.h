/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.
*/


#ifndef DESURA_MCFTHREADI_H
#define DESURA_MCFTHREADI_H
#ifdef _WIN32
#pragma once
#endif


#include "mcfcore/ProgressInfo.h"
#include "webcore/WebCoreI.h"
#include "usercore/UserCoreI.h"
#include "usercore/UploadManagerI.h"
#include "usercore/GuiDownloadProvider.h"
#include "usercore/DownloadInfo.h"
#include "usercore/UploadInfo.h"

namespace UserCore
{
	namespace Thread
	{
		class MCFThreadI : virtual public gcRefBase
		{
		public:
			virtual ~MCFThreadI(){ ; }

			virtual void setThreadManager(gcRefPtr<UserCore::UserThreadManagerI> tm) = 0;
			virtual void setWebCore(gcRefPtr<WebCore::WebCoreI> wc) = 0;
			virtual void setUserCore(gcRefPtr<UserCore::UserI> uc) = 0;

			//events
			virtual Event<uint32>& getCompleteEvent() = 0;
			virtual Event<gcException>& getErrorEvent() = 0;
			virtual Event<WCSpecialInfo>& getNeedWCEvent() = 0;
			virtual Event<uint32>& getProgressEvent() = 0;

			//BaseThread
			virtual void start() = 0;
			virtual void stop() = 0;

			virtual void nonBlockStop() = 0;

			virtual void unpause() = 0;
			virtual void pause() = 0;

			//! Sets the Upload Manager for Mcf threads
			//!
			//! @param um Upload manager
			//!
			virtual void setUpLoadManager(gcRefPtr<UserCore::UploadManagerI> &um) = 0;

			virtual Event<MCFCore::Misc::ProgressInfo>& getMcfProgressEvent() = 0;
			virtual Event<gcString>&					getCompleteStringEvent() = 0;
			virtual Event<gcString>&					getNewItemEvent() = 0;
			virtual Event<DesuraId>&					getItemFoundEvent() = 0;
			virtual Event<UserCore::Misc::GuiDownloadProvider>&	getDownloadProviderEvent() = 0;
			virtual Event<UserCore::Misc::UploadInfo>&			getUploadProgressEvent() = 0;
		};
	}
}

#endif //DESURA_MCFTHREADI_H
