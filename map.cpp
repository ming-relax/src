#include <map>
#include <iostream>
#include <string>

using namespace std;
typedef unsigned long __u64;

class CSubTreeKey
{
	public: 
		CSubTreeKey (__u64 start, __u64 ts)
		{
			this->subtree_start = start;
			this->timestamp = ts;
		};

		__u64 subtree_start, timestamp;
		/*Whichever comparison function we use must define a strict weak ordering over the key type. We can think of a strict weak ordering as "less than," although we might choose to define a more complicated comparison function. However we define it, such a comparison function must always yield false when we compare a key with itself. Moreover, if we compare two keys, they cannot both be "less than" each other, and if k1 is "less than" k2, which in turn is "less than" k3, then k1 must be "less than" k3. If we have two keys, neither of which is "less than" the other, the container will treat them as equal. When used as a key to a map, either value could be used to access the corresponding element*/
		/*http://stackoverflow.com/questions/3277172/using-pair-as-key-in-a-map-c-stl*/
		bool operator<(const CSubTreeKey &k) const
		{
			if(this->subtree_start == k.subtree_start)
			{
				return (this->timestamp < k.timestamp);
			} else {
				return (this->subtree_start < k.subtree_start);
			}
		}
};

int main()
{
	map< CSubTreeKey, string > maps;
	typeof(maps.end()) iter;
	CSubTreeKey key1=CSubTreeKey(10, 100);
	CSubTreeKey key2=CSubTreeKey(20, 100);
	maps.insert(make_pair(key1, string("haha")));
	maps.insert(make_pair(key2, string("hello")));
	iter = maps.find(key1);
	cout << (string)iter->second;

}

