#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(128)
#define     CACHE_LINE      (16)        //16, 32, 64, 128    // number of bytes per line
#define     NOF_WAYS        (1)         //1, 2, 4, 8    // number of lines per set
#define		NO_OF_Iterations	256		// CHange to 1,000,000


unsigned int NOFSETS = (CACHE_SIZE/CACHE_LINE)/NOF_WAYS;
unsigned int NOFIndexBits = log2(NOFSETS); // number of index bits in the address
unsigned int NOFoffsetBits = log2(CACHE_LINE); //


// tag&index = address >> NOFoffsetBits // tag = tag&index >> NOFIndexBits



enum cacheResType {MISS=0, HIT=1};

/* The following implements a random number generator */
unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr=0;
	return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
	static unsigned int addr=0;
	return  rand_()%(24*1024);
}

unsigned int memGen3()
{
	return rand_()%(DRAM_SIZE);
}

unsigned int memGen4()
{
	static unsigned int addr=0;
	return (addr++)%(4*1024);
}

unsigned int memGen5()
{
	static unsigned int addr=0;
	return (addr++)%(1024*64);
}

unsigned int memGen6()
{
	static unsigned int addr=0;
	return (addr+=32)%(64*4*1024);
}

struct CacheLine
{
    bool validBit = 0;
    unsigned int tag = 0;
};


vector<vector<CacheLine>> cache;


cacheResType CAHCE_SET_ASSOCIATIVE(unsigned int addr)
{
    unsigned int tag_index = addr >> NOFoffsetBits;
    unsigned int SetIndex =  tag_index % NOFSETS;
    unsigned int Tag = tag_index >> NOFIndexBits;

    int i, temp_i;

    temp_i = -1;

    for (i = 0; i < NOF_WAYS; i++)
    {
        if (cache[SetIndex][i].validBit)
            {
                if(cache[SetIndex][i].tag == Tag)
                    return HIT;
            }

        else
        {
            temp_i = i;
        }
    }

    // update an empty line in the set
    if (temp_i > -1)
    {
        cache[SetIndex][temp_i].tag = Tag;
        cache[SetIndex][temp_i].validBit = 1;
    }
    // update a random line in the set
    else 
    {
        srand(time(NULL));
        temp_i  = rand()%(NOF_WAYS);
        cache[SetIndex][temp_i].tag = Tag;
        cache[SetIndex][temp_i].validBit = 1;
    }

    return MISS;

}




char *msg[2] = {"Miss","Hit"};



int main()
{
	unsigned int hit = 0;
    double ratio;
	cacheResType r;
	cache.resize(NOFSETS);
    for (auto &line : cache)
    {
        line.resize(NOF_WAYS);
    }
	uint32_t add;

	cout << "Set associative Cache Simulator:\n";

    int testing [256];

    for (int i = 0; i < 256; i++)
    {
        testing[i] = i;
    }

for (int i = 0 ; i <2; i++)
{
    for(int inst=0; inst<NO_OF_Iterations; inst++)
	{
		// add = memGen6();
        add = testing[inst];
		r = CAHCE_SET_ASSOCIATIVE(add);
		if(r == HIT) hit++;
		// cout <<"0x" << setfill('0') << setw(8) << hex << add  << endl ;// <<" ("<< msg[r] <<")\n";
	}
}



// for testing
//	cout << "Set associative Cache Simulator Testing\n";


    // int test = 0;
    // cout << "Enter addresses to test the cache\n";

    // do
    // {
    //     cin >> add;
    //     r = CAHCE_SET_ASSOCIATIVE(add);
	// 	if(r == HIT) hit++;
	// 	cout <<"0x" << setfill('0') << setw(8) << hex << add <<" ("<< msg[r] <<")\n";
    //     // cout << "Enter '0' if you want to keep testing\n";
    //     // cin >> test;
    //     test++;
    // } while (test < NO_OF_Iterations);

    cout << "\n\nThe Cache:\n\n";

    for (int i = 0 ; i < NOFSETS; i++)
    {
        cout << "Index: " << i << "\t"; 
        for (int j = 0; j < NOF_WAYS; j++)
        {
            cout <<"valid bit: " << cache[i][j].validBit << "\t tag: 0x" << cache[i][j].tag << "\t\t";
        }
        cout << endl;
    }

    ratio = (double)hit/(double)(NO_OF_Iterations*2);
	cout << "Hit ratio = " << ratio*100 << "%" << endl;
	cout << "Miss ratio = " << (1-ratio)*100 << "%" << endl;

    return 0;
}