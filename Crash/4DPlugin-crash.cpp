/* --------------------------------------------------------------------------------
 #
 #  4DPlugin-crash.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : crash
 #	author : miyako
 #	2025/03/13
 #  
 # --------------------------------------------------------------------------------*/

#include "4DPlugin-crash.h"

#pragma mark -

void PluginMain(PA_long32 selector, PA_PluginParameters params) {
    
	try
	{
        switch(selector)
        {
			// --- crash
            
			case 1 :
                PA_RunInMainProcess((PA_RunInMainProcessProcPtr)CRASH_4D, NULL);
				break;

        }

	}
	catch(...)
	{

	}
}

#pragma mark -

void CRASH_4D(PA_PluginParameters params) {

    int *ptr = NULL;
    *ptr = 42;  // Crash: Writing to NULL pointer
    return;
    
}
