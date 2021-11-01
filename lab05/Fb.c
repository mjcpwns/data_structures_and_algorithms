
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Fb.h"
#include "Map.h"
#include "Queue.h"
#include "List.h"

#define MAX_PEOPLE 128

struct fb {
    int   numPeople;

    char *names[MAX_PEOPLE]; // the id of a person is simply the index
                             // that contains their name in this array
    
    Map   nameToId; // maps names to ids
                    // question to think about: why do we have this when
                    // the names array already provides this information?
                    // --> to improve time complexity?

    bool  friends[MAX_PEOPLE][MAX_PEOPLE];
};

static char *myStrdup(char *s);
static int   nameToId(Fb fb, char *name);

////////////////////////////////////////////////////////////////////////

// Creates a new instance of FriendBook
Fb   FbNew(void) {
    Fb fb = malloc(sizeof(*fb));
    if (fb == NULL) {
        fprintf(stderr, "Insufficient memory!\n");
        exit(EXIT_FAILURE);
    }

    fb->numPeople = 0;
    fb->nameToId = MapNew();

    for (int i = 0; i < MAX_PEOPLE; i++) {
        for (int j = 0; j < MAX_PEOPLE; j++) {
            fb->friends[i][j] = false;
        }
    }

    return fb;
}

void FbFree(Fb fb) {
    for (int i = 0; i < fb->numPeople; i++) {
        free(fb->names[i]);
    }

    MapFree(fb->nameToId);
    free(fb);
}

bool FbAddPerson(Fb fb, char *name) {
    if (fb->numPeople == MAX_PEOPLE) {
        fprintf(stderr, "error: could not add more people\n");
        exit(EXIT_FAILURE);
    }

    if (!MapContains(fb->nameToId, name)) {
        int id = fb->numPeople++;
        fb->names[id] = myStrdup(name);
        MapSet(fb->nameToId, name, id);
        return true;
    } else {
        return false;
    }
}

bool FbHasPerson(Fb fb, char *name) {
    return MapContains(fb->nameToId, name);
}

List FbGetPeople(Fb fb) {
    List l = ListNew();
    for (int id = 0; id < fb->numPeople; id++) {
        ListAppend(l, fb->names[id]);
    }
    return l;
}

bool FbFriend(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    assert(id1 != id2);

    if (!fb->friends[id1][id2]) {
        fb->friends[id1][id2] = true;
        fb->friends[id2][id1] = true;
        return true;
    } else {
        return false;
    }
}

bool FbIsFriend(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    return fb->friends[id1][id2];
}

List FbGetFriends(Fb fb, char *name) {
    int id1 = nameToId(fb, name);
    
    List l = ListNew();
    for (int id2 = 0; id2 < fb->numPeople; id2++) {
        if (fb->friends[id1][id2]) {
            ListAppend(l, fb->names[id2]);
        }
    }
    return l;
}

int  FbNumFriends(Fb fb, char *name) {
    int id1 = nameToId(fb, name);
    
    int numFriends = 0;
    for (int id2 = 0; id2 < fb->numPeople; id2++) {
        if (fb->friends[id1][id2]) {
            numFriends++;
        }
    }
    return numFriends;
}

////////////////////////////////////////////////////////////////////////
// Your tasks

bool FbUnfriend(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    if (fb->friends[id1][id2]) {
        fb->friends[id1][id2] = 0;
        return true;
    }
    return false;
}

List FbMutualFriends(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    
    List l = ListNew();
    for (int id = 0; id < fb->numPeople; id++) {
        if (fb->friends[id][id1] && fb->friends[id][id2]) {
            ListAppend(l, fb->names[id]);
        }
    }
    return l;
}


void FbFriendRecs1(Fb fb, char *name) {
    int id1 = nameToId(fb, name);
    // MAX Friends is N - 1, N being the total number of people
    for (int mutual = MAX_PEOPLE - 1; mutual > 0; mutual--) {
        for (int id2 = 0; id2 < fb->numPeople; id2++) {
            List mf = FbMutualFriends(fb, name, fb->names[id2]);
            // If not the user, user's friend and mutual friends match mutual
            if ((id1 != id2) && !FbIsFriend(fb, name, fb->names[id2]) && 
                ListSize(mf) == mutual) 
                printf("\t%-20s%4d mutual friends\n", fb->names[id2], mutual);
            ListFree(mf);
        }
    }
}

////////////////////////////////////////////////////////////////////////
// Optional task

void FbFriendRecs2(Fb fb, char *name) {
    // TODO: Add your code here
}

////////////////////////////////////////////////////////////////////////
// Helper Functions

static char *myStrdup(char *s) {
    char *copy = malloc((strlen(s) + 1) * sizeof(char));
    if (copy == NULL) {
        fprintf(stderr, "Insufficient memory!\n");
        exit(EXIT_FAILURE);
    }
    return strcpy(copy, s);
}

// Converts a name to an ID. Raises an error if the name doesn't exist.
static int nameToId(Fb fb, char *name) {
    if (!MapContains(fb->nameToId, name)) {
        fprintf(stderr, "error: person '%s' does not exist!\n", name);
        exit(EXIT_FAILURE);
    }
    return MapGet(fb->nameToId, name);
}

// bool checkItemInList(List l, char *name) {
//     ListIterator i = ListItNew(l);
//     while (ListItHasNext(i)) {
//         if (!strcmp(ListItNext(i), name)) { // Item is in List
//             return true; 
//         };
//     }
//     ListItFree(i);
//     return false; // Item is not in List
// }
// List friend_list = FbGetFriends(fb, name);
    // List friend_recs = ListNew();
    // ListIterator l1 = ListItNew(friend_list); 
    // while (ListItHasNext(l1)) {
    //     char *friend = ListItNext(l1); // Get Friend
    //     List friend_friend_list = FbGetFriends(fb, friend); // Get Friend's Friend List
    //     ListIterator l2 = ListItNew(friend_friend_list);
    //     while (ListItHasNext(l2)) {
    //         char *friend_contact = ListItNext(l2); // Get Friend's Friend
    //         // If not user contact, not already a friend and if not already inserted
    //         // in friend recommendations
    //         if (strcmp(friend_contact, name) && !FbIsFriend(fb, name, friend_contact) && 
    //             !checkItemInList(friend_recs, friend_contact)) {
    //                 ListAppend(friend_recs, friend_contact);
    //         }
    //     }
    //     ListItFree(l2);
    //     ListFree(friend_friend_list);
    // }
    // ListItFree(l1);
    // ListFree(friend_list);
    // ListIterator i = ListItNew(friend_recs);
    // while (ListItHasNext(i)) {
    //     char *rec_name = ListItNext(i);
    //     List mutual_friends = FbMutualFriends(fb, name, rec_name);
    //     printf("\t%-20s%4d mutual friends\n", rec_name, ListSize(mutual_friends));
    //     ListFree(mutual_friends);
    // }
    // ListItFree(i);
    // ListFree(friend_recs);