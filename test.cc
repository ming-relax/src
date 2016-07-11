#include <iostream>
using namespace std;

typedef struct{
	unsigned long start;
	unsigned long len;
	unsigned int	pdevid;
	unsigned long pbid;
}extent_t;

class CExtent{
private:
	unsigned long start;
	unsigned long len;
	unsigned int	pdevid;
	unsigned long pbid;
public:
	unsigned long get_len(){
		return len;
	}
	
};

int main()
{
	extent_t ext;
	CExtent  cext;
	cout << sizeof(ext) << endl;
	cout << sizeof(cext) << endl;
}
