/* 
 * Quickly find any files without the expected number of entries
 */


#include "TString.h"
#include "TFile.h"
#include "TTree.h"

void quick_check()
{
    const int expected_entries = 43000;
    for(int file_id = 0; file_id < 75; ++file_id)
    {
        TString name("long_");
        name+=file_id;
        name+=".root";
        TFile* file = new TFile(name.Data(), "READ");
	if (file==NULL)
	{
	    printf("%s missing.\n", name.Data());
 	    continue;
	}
        TTree* tree = (TTree*) file->Get("mppc");
	if (tree==NULL)
	{
	    file->Close();
	    printf("%s mppc tree missing\n", name.Data());
	    continue;
	}
        const int n_entries = tree->GetEntries();
        if (n_entries != expected_entries)
	{
	    printf("%s has an unexpected number of entries: %i\n",name.Data(), n_entries);
	}
        printf("%s has %i entries\n",name.Data(), n_entries);
        file->Close();
    }
    printf ("Quick check finished\n");
}
