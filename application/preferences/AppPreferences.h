/*
 * Copyright 2010, Oliver Ruiz Dorantes. All rights reserved.
 * Copyright 2012, Casalinuovo Dario. All rights reserved.
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _APP_PREFERENCES_H
#define _APP_PREFERENCES_H

#include <Rect.h>
#include <SupportDefs.h>


class AppPreferences {
public:
 static AppPreferences*	Get();

			void	Load();
			void	Save();

			bool 	MoveToCurrentWorkspace;
			bool 	RaiseOnMessageReceived;
			bool 	RaiseUserIsTyping;
			bool	MarkUnreadWindow;
			bool 	NotifyProtocolStatus;
			bool 	NotifyContactStatus;
			bool	NotifyNewMessage;

			bool 	HideDeskbar;
			bool 	DisableReplicant;
			bool	DisableQuitConfirm;

			bool 	IgnoreEmoticons;
			
			bool	HideOffline;

			float	MainWindowListWeight;
			float	MainWindowChatWeight;

			BRect	MainWindowRect;

private:
	 const char*	_PreferencesPath();

	static AppPreferences* fInstance;
};

#endif // _APP_PREFERENCES_H
