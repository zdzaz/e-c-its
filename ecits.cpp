#include "Consensus.h"

// #include "/usr/local/include/boost/thread/thread.hpp"
#include <mutex>

using namespace std;


long mIdCounter;
  

main (int argc, char *argv [])
{
    
    string feature = "";
    cout << "Insert Serialized feature: " << endl;
    cin >> feature;

    /* 
        Consensus Instance
     */
    Consensus *consensus = new Consensus();

    do{      

        consensus->init(feature);

        /* 
            This simulates the "sensors" from the ITS node,
            belonging to the node's TCB,
            detecting a given feature
         */

        std::cin.clear();
        fflush(stdin);
        cout << "Insert Serialized feature: " << endl;
        cin >> feature;
        
    } while (1);

}