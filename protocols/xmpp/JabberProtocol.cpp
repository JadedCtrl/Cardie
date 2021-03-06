/*
 * Copyright 2010, Pier Luigi Fiorini. All rights reserved.
 * Distributed under the terms of the GPL v2 License.
 *
 * Authors:
 *		Pier Luigi Fiorini, pierluigi.fiorini@gmail.com
 */

#include "JabberProtocol.h"

#include <Catalog.h>
#include <Resources.h>

#include <libinterface/BitmapUtils.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "JabberProtocol"


JabberProtocol::JabberProtocol()
	: JabberHandler()
{
}


JabberProtocol::~JabberProtocol()
{
}


const char*
JabberProtocol::Signature() const
{
	return "xmpp";
}


const char*
JabberProtocol::FriendlySignature() const
{
	return "XMPP";
}


BBitmap*
JabberProtocol::Icon() const
{
	return ReadNodeIcon(fPath.Path(), B_LARGE_ICON, true);
}


BMessage
JabberProtocol::SettingsTemplate(const char* name)
{
	if (strcmp(name, "account") == 0)
		return JabberHandler::_SettingsTemplate(B_TRANSLATE("XMPP identifier:"),
			true);
	if (strcmp(name, "join_room") == 0 || strcmp(name, "create_room") == 0)
		return JabberHandler::_RoomTemplate();
	if (strcmp(name, "roster") == 0)
		return JabberHandler::_RosterTemplate();
	else
		return BMessage();
}


void
JabberProtocol::OverrideSettings()
{
}


BString
JabberProtocol::ComposeJID() const
{
	BString jid(fUsername);
	if (jid.FindLast("@") < 0)
		jid << "@" << fServer;
	jid << "/" << fResource;
	return jid;
}
