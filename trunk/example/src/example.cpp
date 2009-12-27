/*
Copyright (c) 2009, Piotr Wach, Polidea
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Polidea nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY PIOTR WACH, POLIDEA ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL PIOTR WACH, POLIDEA BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//  Include Files  

#include "example.h"
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>			// Console

#include "Json.h"

//  Constants

_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d");
_LIT(KTextPressAnyKey, " [press any key]\n");

//  Global Variables

LOCAL_D CConsoleBase* console; // write all messages to this

_LIT(KTestFormatedJson, "{\t\"message\":\"test1\r\ntest2\r\n\t\t\ttest3\"}");


//  Local Functions
LOCAL_C void MainL()
	{
	// to json
	RBuf jsonString;
		{
		CJsonObject* root = new (ELeave) CJsonObject();
		
		CJsonObject* project = new (ELeave) CJsonObject();
		// this will transfer ownership of project to root object
		root->AddL(_L("project"), project);
			project->AddL(_L("name"), _L("s60-json-library"));
			root->AddBoolL(_L("booleanVariable"), ETrue);
			root->AddL(_L("nullValue"), (CJsonObject*)NULL);
			
		root->AddBoolL(_L("booleanVariable"), ETrue);
		
		CJsonArray* values = new (ELeave) CJsonArray();
		// this will transfer ownership of values to root object
		root->AddL(_L("arrayOfValues"), values);
			values->AddBoolL(ETrue);
			values->AddIntL(123);
			values->AddL(_L("string"));
			values->AddReal32L(1.23);
			values->AddL((CJsonObject*)NULL);
			
		jsonString.Create(256);
		
		// convert in memory structure to json string format
		root->ToStringL(jsonString);
		
		// this will release all objects
		delete root;
		}
	
	// from json
		{
		CJsonBuilder* jsonBuilder = CJsonBuilder::NewL();
		
		// this will create json string representation in memory
		jsonBuilder->BuildFromJsonStringL(jsonString);
		
		CJsonObject* rootObject;
		jsonBuilder->GetDocumentObject(rootObject);
		
		if(rootObject)
			{
			CJsonObject* project;
			// this will not transfer ownership, owner of project is rootObject
			rootObject->GetObjectL(_L("project"), project);
			
			if(project)
				{
				TBuf<256> name;
				project->GetStringL(_L("name"), name);
				}
			}
		
		// we need manually release created object
		delete rootObject;
		
		// releases only jsonBuilder object, not objects which was created by him
		delete jsonBuilder;
		}
		
	// from formatted json
		{
		CJsonBuilder* jsonBuilder = CJsonBuilder::NewL();
		
		// this will create json string representation in memory
		jsonBuilder->BuildFromJsonStringL(KTestFormatedJson);
		
		CJsonObject* rootObject;
		jsonBuilder->GetDocumentObject(rootObject);
		
		if(rootObject)
			{
			TBuf<256> message;
			rootObject->GetStringL(_L("message"), message);
			}
		
		// we need manually release created object
		delete rootObject;
		
		// releases only jsonBuilder object, not objects which was created by him
		delete jsonBuilder;
		}
	
	// release json string
	jsonString.Close();
	}

LOCAL_C void DoStartL()
	{
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	MainL();

	// Delete active scheduler
	CleanupStack::PopAndDestroy(scheduler);
	}

//  Global Functions
GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Create output console
	TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(
			KConsFullScreen, KConsFullScreen)));
	if (createError)
		return createError;

	// Run application code inside TRAP harness, wait keypress when terminated
	TRAPD(mainError, DoStartL());
	if (mainError)
		console->Printf(KTextFailed, mainError);
	console->Printf(KTextPressAnyKey);
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

