/*
 * Copyright 2009, Pier Luigi Fiorini. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Pier Luigi Fiorini, pierluigi.fiorini@gmail.com
 */

#include "RosterListView.h"

#include <string.h>
#include <stdio.h>

#include <Catalog.h>
#include <Looper.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <SeparatorItem.h>

#include "ChatProtocolMessages.h"
#include "Contact.h"
#include "ProtocolLooper.h"
#include "RosterItem.h"
#include "TheApp.h"
#include "UserInfoWindow.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "RosterListView"


const int32 kAddPeople	= 'ADPL';
const int32 kSendFile	= 'SDFL';
const int32 kShowLogs	= 'SHLG';
const int32	kStartConv	= 'SRCV';
const int32 kGetInfo	= 'GINF';


static int
compare_by_name(const void* _item1, const void* _item2)
{
	BListItem* item1 = *(BListItem**)_item1;
	BListItem* item2 = *(BListItem**)_item2;
	RosterItem* roster1 = dynamic_cast<RosterItem*>(item1);
	RosterItem* roster2 = dynamic_cast<RosterItem*>(item2);

	if (roster1 == NULL && roster2 == NULL)
		return 0;
	if (roster1 == NULL)
		return 1;
	if (roster2 == NULL)
		return -1;
	return strcasecmp(roster1->GetContact()->GetName().String(),
		roster2->GetContact()->GetName().String());
}


static int
compare_by_status(const void* _item1, const void* _item2)
{
	BListItem* item1 = *(RosterItem**)_item1;
	BListItem* item2 = *(RosterItem**)_item2;
	RosterItem* roster1 = dynamic_cast<RosterItem*>(item1);
	RosterItem* roster2 = dynamic_cast<RosterItem*>(item2);

	if (roster1 == NULL && roster2 == NULL)
		return 0;
	if (roster1 == NULL || roster1->Status() < roster2->Status())
		return 1;
	if (roster2 == NULL || roster1->Status() > roster2->Status())
		return -1;
	return 0;
}


RosterListView::RosterListView(const char* name)
	: BOutlineListView(name, B_SINGLE_SELECTION_LIST,
		B_WILL_DRAW | B_FRAME_EVENTS |
		B_NAVIGABLE | B_FULL_UPDATE_ON_RESIZE),
	fPrevItem(NULL)
{
	// Context menu
	fPopUp = new BPopUpMenu("contextMenu", false, false);
	BMenuItem* item = NULL;

	fPopUp->AddItem(new BMenuItem(B_TRANSLATE("Start a chat"),
		new BMessage(kStartConv)));
	item = new BMenuItem(B_TRANSLATE("Send a file" B_UTF8_ELLIPSIS),
		new BMessage(kSendFile));
	item->SetEnabled(false);
	fPopUp->AddItem(item);

	fPopUp->AddItem(new BSeparatorItem());

	fPopUp->AddItem(new BMenuItem(B_TRANSLATE("User info" B_UTF8_ELLIPSIS),
		new BMessage(kGetInfo)));

	fPopUp->SetTargetForItems(this);
}


//	#pragama mark -


void
RosterListView::AttachedToWindow()
{
	fPopUp->SetTargetForItems(this);
	SetTarget(this);
}


void
RosterListView::MessageReceived(BMessage* msg)
{
	BListItem* item = ItemAt(CurrentSelection());
	RosterItem* ritem = reinterpret_cast<RosterItem*>(item);

	switch (msg->what) {
		case kGetInfo:
		{
			if (ritem == NULL)
				return;

			_InfoWindow(ritem->GetContact());
			break;
		}

		case kStartConv:
		{
			User* user;
			if (ritem == NULL || (user = ritem->GetContact()) == NULL)
				return;

			BMessage* start = new BMessage(IM_MESSAGE);
			start->AddInt32("im_what", IM_CREATE_CHAT);
			start->AddString("user_id", user->GetId());
			ProtocolLooper* looper = user->GetProtocolLooper();

			if (looper != NULL)
				looper->PostMessage(start);

			break;
		}

		default:
			BListView::MessageReceived(msg);
	}
}


void
RosterListView::MouseMoved(BPoint where, uint32 code, const BMessage* msg)
{
	BListView::MouseMoved(where, code, msg);
	return;

	switch (code) {
		case B_INSIDE_VIEW:
		{
			// Mouse cursor is inside this view, hide last item's popup
			// and show current item's popup
			BListItem* item = ItemAt(IndexOf(where));
			RosterItem* ritem = reinterpret_cast<RosterItem*>(item);

			if (ritem == NULL)
				return;

			// Hide previous item's popup
			if ((fPrevItem != NULL) && (fPrevItem != ritem))
				fPrevItem->GetContact()->HidePopUp();

			// Show current item's popup
			ritem->GetContact()->ShowPopUp(ConvertToScreen(where));

			// This will be the previous item
			fPrevItem = ritem;
			break;
		}
		case B_EXITED_VIEW:
			// Mouse cursor leaved this view, hide last item's popup
			if (fPrevItem != NULL)
				fPrevItem->GetContact()->HidePopUp();
			break;
	}
}


void
RosterListView::MouseDown(BPoint where)
{
	BMessage* message = Looper()->CurrentMessage();

	int32 buttons = 0;
	(void)message->FindInt32("buttons", &buttons);

	if (buttons == B_SECONDARY_MOUSE_BUTTON) {
		int32 index = IndexOf(where);
		if (index >= 0) {
			// Select list item
			Select(index);

			// Show context menu if right button is clicked
			(void)fPopUp->Go(ConvertToScreen(where), true, true, false);
		}
	} else {
		// Call original MouseDown()
		BListView::MouseDown(where);
	}
}


void
RosterListView::Draw(BRect updateRect)
{
	int32 count = CountItems();
	if (count == 0)
		return;

	BRect itemFrame(0, 0, Bounds().right, -1);
	for (int32 i = 0; i < count; i++) {
		BListItem* item = ItemAt(i);
		RosterItem* rosterItem = reinterpret_cast<RosterItem*>(item);

		if (!rosterItem->IsVisible())
			continue;

		itemFrame.bottom = itemFrame.top + ceilf(item->Height()) - 1;

		if (itemFrame.Intersects(updateRect))
			rosterItem->DrawItem(this, itemFrame);

		itemFrame.top = itemFrame.bottom + 1;
	}
}


bool
RosterListView::AddItem(BListItem* item)
{
	item->Deselect();
	bool ret = false;
	if (HasItem(item) == false)
		ret = BListView::AddItem(item);
	Sort();
	return ret;
}


bool
RosterListView::RemoveItem(BListItem* item)
{
	item->Deselect();
	return BListView::RemoveItem(item);
}


RosterItem*
RosterListView::RosterItemAt(int32 index)
{
	return dynamic_cast<RosterItem*>(ItemAt(index));
}


void
RosterListView::Sort()
{
	SortItems(compare_by_name);
}


void
RosterListView::_InfoWindow(Contact* linker)
{
	UserInfoWindow* win = new UserInfoWindow(linker);
	win->Show();
}
