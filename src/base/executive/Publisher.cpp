//$Header$
//------------------------------------------------------------------------------
//                            Publisher
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/09/21
//
/**
 *  Implementation code for the Publisher class.
 */
//------------------------------------------------------------------------------

#include "Publisher.hpp"
#include <string>


Publisher::Publisher(void)
{
}
/*Publisher::Publisher(void) :
    subs                (NULL)
{
}
*/

Publisher::~Publisher(void)
{
    subs.clear();
}


bool Publisher::Subscribe(Subscriber * s)
{
    if (!s)
        return false;

    subs.push_back(s);
    return true;
}


bool Publisher::Unsubscribe(Subscriber * s)
{
    if (!s)
        return false;

    subs.remove(s);
    return true;
}


bool Publisher::Publish(Real * data, Integer count)
{
    // No subscribers
    if (subs.empty())
        return false;

    // Convert the data into a string for distribution
    char stream[4096] = "";
    
    for (int i = 0; i < count; ++i) {
        sprintf(stream, "%s%12lf", stream, data[i]);
        if (i < count - 1)
            strcat(stream, ", ");
        else
            strcat(stream, "\n");
    }

    std::list<Subscriber*>::iterator current = subs.begin();
    while (current != subs.end())
    {
        if (!(*current)->ReceiveData(stream))
            return false;
        if (!(*current)->ReceiveData(data, count))
            return false;
        current++;
    }

    return true;
}


bool Publisher::Publish(char * data, int count)
{
    int i;
    
    // No subscribers
    if (subs.empty())
        return false;

    // Convert the data into a string for distribution
    char stream[4096];

    if (count) {
        for (i = 0; i < count; ++i)
            stream[i] = data[i];
        stream[i] = '\0';
    }
    else
        strcpy(stream, data);

    strcat(stream, "\n");

    std::list<Subscriber*>::iterator current = subs.begin();
    while (current != subs.end())
    {
        if (!(*current)->ReceiveData(stream))
            return false;
        current++;
    }

    return true;
}


bool Publisher::Publish(int * data, int count)
{
    // No subscribers
    if (subs.empty())
        return false;

    // Convert the data into a string for distribution
    char stream[4096];

    for(int i = 0; i < count; ++i) {
        sprintf(stream, "%s%d", stream, data[i]);
        if (i < count - 1)
            strcat(stream, ", ");
        else
            strcat(stream, "\n");
    }

    std::list<Subscriber*>::iterator current = subs.begin();
    while (current != subs.end())
    {
        if (!(*current)->ReceiveData(stream))
            return false;
        current++;
    }

    return true;
}

