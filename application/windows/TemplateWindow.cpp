/*
 * Copyright 2009-2010, Pier Luigi Fiorini. All rights reserved.
 * Copyright 2021, Jaidyn Levesque. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Pier Luigi Fiorini, pierluigi.fiorini@gmail.com
 *		Jaidyn Levesque, jadedctrl@teknik.io
 */

#include "TemplateWindow.h"

#include <Alert.h>
#include <Button.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>
#include <TextControl.h>
#include <String.h>

#include "CayaProtocolMessages.h"
#include "CayaUtils.h"
#include "TemplateView.h"


const uint32 kOK		= 'save';
const uint32 kAccSelected = 'JWas';



TemplateWindow::TemplateWindow(const char* title, const char* templateType,
	BMessage* msg, Server* server, bigtime_t instance)
	:
	BWindow(BRect(0, 0, 400, 100), title, B_FLOATING_WINDOW,
		B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE),
	fServer(server),
	fAccounts(server->GetAccounts()),
	fSelectedAcc(0),
	fTemplate(NULL),
	fTemplateType(templateType),
	fMessage(msg),
	fTarget(NULL)
{
	_InitInterface(instance);
	_LoadTemplate();
	CenterOnScreen();
}


TemplateWindow::TemplateWindow(const char* title, ProtocolTemplate* temp,
	BMessage* msg, Server* server, bigtime_t instance)
	:
	BWindow(BRect(0, 0, 400, 100), title, B_FLOATING_WINDOW,
		B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE),
	fServer(server),
	fAccounts(server->GetAccounts()),
	fSelectedAcc(0),
	fTemplate(temp),
	fMessage(msg)
{
	_InitInterface(instance);
	CenterOnScreen();

	fTemplate->Load(fTemplateView);
	fTemplateView->AttachedToWindow();
	fTemplateView->MakeFocus(true);
}


void
TemplateWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case IM_MESSAGE: {
			// If IM_MESSAGE, assume it should be treated as current settings
			if (fTemplate == NULL)
				break;
			for (int i = 0; fTemplateView->CountChildren(); i++)
				fTemplateView->RemoveChild(fTemplateView->ChildAt(i));
			fTemplate->Load(fTemplateView, msg);
			break;
		}
		case kOK: {
			// Save account settings
			if (fTemplate == NULL || fTemplateView == NULL)
				break;
			BString error = "Some items are empty. Please make sure to fill "
				"out every item.";
			BMessage* settings = new BMessage(*fMessage);
			status_t result = fTemplate->Save(fTemplateView, settings, &error);

			if (result != B_OK) {
				BAlert* alert = new BAlert("", error.String(), "OK", NULL, NULL,
					B_WIDTH_AS_USUAL, B_WARNING_ALERT);
				alert->Go();
				break;
			}
			ProtocolLooper* looper
				= fServer->GetProtocolLooper(fAccounts.ValueAt(fSelectedAcc));
			if (looper == NULL)
				break;
			looper->PostMessage(settings);
			Close();
			break;
		}
		case kAccSelected:
		{
			int32 index;
			if (msg->FindInt32("index", &index) == B_OK)
				fSelectedAcc = index;
			_LoadTemplate();
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}


void
TemplateWindow::SetTarget(BHandler* target)
{
	fTarget = target;
}


void
TemplateWindow::_InitInterface(bigtime_t instance)
{
	fTemplateView = new TemplateView("template");
	BMenu* menu = CreateAccountMenu(fAccounts, BMessage(kAccSelected));
	fMenuField = new BMenuField("accountMenuField", NULL, menu);

	if (instance > -1) {
		for (int i = 0; i < fAccounts.CountItems(); i++)
			if (fAccounts.ValueAt(i) == instance) {
				menu->ItemAt(i)->SetMarked(true);
				break;
			}
		fMenuField->SetEnabled(false);
	}

	BButton* fOkButton = new BButton("OK", new BMessage(kOK));
	if (fAccounts.CountItems() <= 0)
		fOkButton->SetEnabled(false);
	fOkButton->MakeDefault(true);

	const float spacing = be_control_look->DefaultItemSpacing();

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fTemplateView)
		.AddGroup(B_HORIZONTAL)
			.Add(fMenuField)
			.AddGlue()
			.Add(new BButton("Cancel", new BMessage(B_QUIT_REQUESTED)))
			.Add(fOkButton)
		.End()
	.End();
}


void
TemplateWindow::_LoadTemplate()
{
	if (fAccounts.CountItems() == 0 || fTemplateType.IsEmpty() == true)
		return;

	ProtocolLooper* looper
		= fServer->GetProtocolLooper(fAccounts.ValueAt(fSelectedAcc));
	if (looper == NULL)
		return;

	fTemplate = new ProtocolTemplate(looper->Protocol(), fTemplateType.String());

	for (int i = 0; fTemplateView->CountChildren(); i++)
		fTemplateView->RemoveChild(fTemplateView->ChildAt(i));

	fTemplate->Load(fTemplateView);
	fTemplateView->AttachedToWindow();
	fTemplateView->MakeFocus(true);
}