#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

typedef unsigned int __u32;
typedef unsigned long __u64;
#define PHYDEV_UUID_LEN 8
#define   START_LEN_2_END(start, len) ((start)+(len)-1)

struct map_ext_t{
	__u32 map_blk_id;
	__u32 length;
	char phy_uuid[PHYDEV_UUID_LEN];
	__u64 phy_blk_id;

	map_ext_t(__u64 lbid, __u64 pbid, __u32 len, char *puuid) 
	{
		map_blk_id = lbid;
		phy_blk_id = pbid;
		length = len;
		if(puuid)
			memcpy(phy_uuid, puuid, PHYDEV_UUID_LEN);
	}

	map_ext_t(__u64 lbid, __u32 len)
	{
		map_blk_id = lbid;
		length = len;
	}

	map_ext_t( struct map_ext_t *ext)
	{
		memcpy(this, ext, sizeof(struct map_ext_t));
	}

	map_ext_t( )
	{
		//nothing to do;
	}

	bool operator < (const struct map_ext_t & ext) const{  
		return ( START_LEN_2_END(map_blk_id,length) < ext.map_blk_id);  
	} 		


};

int main()
{
	cout << sizeof(map_ext_t) <<endl;
}
