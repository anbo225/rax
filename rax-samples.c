/*
 * rax_samples.c
 *
 *  Created on: 2019年6月27日
 *      Author: anbo
 */
#include <stdio.h>

#include "rax.h"
#include "rc4rand.h"

/* Test the random walk function. */
int randomWalkTest(void) {
    rax *t = raxNew();
    char *toadd[] = {"alligator","alien","baloon","chromodynamic","romane","romanus","romulus","rubens","ruber","rubicon","rubicundus","all","rub","ba",NULL};

    long numele;
    for (numele = 0; toadd[numele] != NULL; numele++) {
        raxInsert(t,(unsigned char*)toadd[numele],
                    strlen(toadd[numele]),(void*)numele,NULL);
    }

    raxIterator iter;
    raxStart(&iter,t);
    raxSeek(&iter,"^",NULL,0);
    int maxloops = 100000;
    while(raxRandomWalk(&iter,0) && maxloops--) {
        int nulls = 0;
        for (long i = 0; i < numele; i++) {
            if (toadd[i] == NULL) {
                nulls++;
                continue;
            }
            if (strlen(toadd[i]) == iter.key_len &&
                memcmp(toadd[i],iter.key,iter.key_len) == 0)
            {
                toadd[i] = NULL;
                nulls++;
            }
        }
        if (nulls == numele) break;
    }
    if (maxloops == 0) {
        printf("randomWalkTest() is unable to report all the elements "
               "after 100k iterations!\n");
        return 1;
    }
    raxStop(&iter);
    raxFree(t);
    return 0;
}

/* Test the iterator function */
int iteratorUnitTests(void) {
    rax *t = raxNew();
    char *toadd[] = {"alligator","alien","baloon","chromodynamic","romane","romanus","romulus","rubens","ruber","rubicon","rubicundus","all","rub","ba",NULL};

    for (int x = 0; x < 10000; x++) rc4rand();

    long items = 0;
    while(toadd[items] != NULL) items++;

    for (long i = 0; i < items; i++)
        raxInsert(t,(unsigned char*)toadd[i],strlen(toadd[i]),(void*)i,NULL);

    raxIterator iter;
    raxStart(&iter,t);

    struct {
        char *seek;
        size_t seeklen;
        char *seekop;
        char *expected;
    } tests[] = {
        /* Seek value. */       /* Expected result. */
        {"rpxxx",5,"<=",         "romulus"},
        {"rom",3,">=",           "romane"},
        {"rub",3,">=",           "rub"},
        {"rub",3,">",            "rubens"},
        {"rub",3,"<",            "romulus"},
        {"rom",3,">",            "romane"},
        {"chro",4,">",           "chromodynamic"},
        {"chro",4,"<",           "baloon"},
        {"chromz",6,"<",         "chromodynamic"},
        {"",0,"^",               "alien"},
        {"zorro",5,"<=",         "rubicundus"},
        {"zorro",5,"<",          "rubicundus"},
        {"zorro",5,"<",          "rubicundus"},
        {"",0,"$",               "rubicundus"},
        {"ro",2,">=",            "romane"},
        {"zo",2,">",             NULL},
        {"zo",2,"==",            NULL},
        {"romane",6,"==",        "romane"}
    };

    for (int i = 0; tests[i].expected != NULL; i++) {
        raxSeek(&iter,tests[i].seekop,(unsigned char*)tests[i].seek,
                tests[i].seeklen);
        int retval = raxNext(&iter);

        if (tests[i].expected != NULL) {
            if (strlen(tests[i].expected) != iter.key_len ||
                memcmp(tests[i].expected,iter.key,iter.key_len) != 0)
            {
                printf("Iterator unit test error: "
                       "test %d, %s expected, %.*s reported\n",
                       i, tests[i].expected, (int)iter.key_len,
                       (char*)iter.key);
                return 1;
            }
        } else {
            if (retval != 0) {
                printf("Iterator unit test error: "
                       "EOF expected in test %d\n", i);
                return 1;
            }
        }
    }
    raxStop(&iter);
    raxFree(t);
    return 0;
}

/* Test that raxInsert() / raxTryInsert() overwrite semantic
 * works as expected. */
int tryInsertUnitTests(void) {
    rax *t = raxNew();
    raxInsert(t,(unsigned char*)"FOO",3,(void*)(long)1,NULL);
    void *old, *val;
    raxTryInsert(t,(unsigned char*)"FOO",3,(void*)(long)2,&old);
    if (old != (void*)(long)1) {
        printf("Old value not returned correctly by raxTryInsert(): %p",
            old);
        return 1;
    }

    val = raxFind(t,(unsigned char*)"FOO",3);
    if (val != (void*)(long)1) {
        printf("FOO value mismatch: is %p intead of 1", val);
        return 1;
    }

    raxInsert(t,(unsigned char*)"FOO",3,(void*)(long)2,NULL);
    val = raxFind(t,(unsigned char*)"FOO",3);
    if (val != (void*)(long)2) {
        printf("FOO value mismatch: is %p intead of 2", val);
        return 1;
    }

    raxFree(t);
    return 0;
}



int main(int argc, char** argv){
	int errors = 0;
	printf("Unit tests: "); fflush(stdout);
	if (randomWalkTest()) errors++;
	if (iteratorUnitTests()) errors++;
    if (tryInsertUnitTests()) errors++;
	if (errors == 0) printf("OK\n");

}
