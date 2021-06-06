/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef PERMS_H
#define PERMS_H

#include <SupportDefs.h>

// NAME, SUBJECT, ROLECHANGE, BAN, KICK, DEAFEN, MUTE, NICK, READ, WRITE
// Set name of room, set subject, change user's "role" (permission presets
// defined by the protocol), etc…

// NSRBKDMNRW
// 0000000000

#define PERM_WRITE			0x01
#define PERM_READ			0x02
#define PERM_NICK			0x04
#define PERM_MUTE			0x08
#define PERM_DEAFEN			0x016
#define PERM_KICK			0x032
#define PERM_BAN			0x064
#define PERM_ROLECHANGE		0x0128
#define PERM_ROOM_SUBJECT	0x0256
#define PERM_ROOM_NAME		0x0512
#define PERM_ALL			1023


class Role {
public:
	Role()
		: fTitle("Default"), fPerms(0 | PERM_WRITE | PERM_READ), fPriority(0)
	{
	}

	Role(BString title, uint32 perms, uint32 priority)
		: fTitle(title), fPerms(perms), fPriority(priority)
	{
	}

	BString fTitle;
	uint32 fPerms;		// Permissions afforded to role, as described above.
	uint32 fPriority;	// 'Rank' of role, with higher being greater priority.
						// I.E., a user with a priority of 11 can't kick a user
						// with a priority of 12, but can one with 10.
						// This sort of hierarchy might not be universal in
						// chat protocols, but I think it can be adequately
						// simulated in add-ons.
};


#endif // PERMS_H

