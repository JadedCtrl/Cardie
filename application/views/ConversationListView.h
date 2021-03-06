/*
 * Copyright 2021, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _CONVERSATION_LIST_H
#define _CONVERSATION_LIST_H

#include <OutlineListView.h>

class BPopUpMenu;
class Conversation;
class ConversationAccountItem;


class ConversationListView : public BOutlineListView {
public:
	ConversationListView(const char* name);

	virtual void	MessageReceived(BMessage* msg);
	virtual	void	SelectionChanged();
	virtual void	MouseDown(BPoint where);

			// After removing item, select another
			void	RemoveItemSelecting(BListItem* item);

			void	AddConversation(Conversation* chat);
			void	RemoveConversation(Conversation* chat);
			void	AddAccount(int64 instance);
			void	RemoveAccount(int64 instance);
			void	SortConversation(Conversation* chat);

private:
	  BPopUpMenu*	_ConversationPopUp();
			void	_AddDefaultItems(BPopUpMenu* menu, Conversation* chat);
	  BPopUpMenu*	_BlankPopUp();

	ConversationAccountItem*
					_EnsureAccountItem(Conversation* chat);
};

#endif // _CONVERSATION_LIST_H
