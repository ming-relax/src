#include <iostream>
using namespace std;
class parent{
	int data;
public:
	parent()
	{
		cout << "parent constructor called" << endl;
	}
	virtual ~parent()
	{
		cout << "parent destructor called" << endl;
	}
};

class child: public parent
{
public:
	child()
	{
		cout << "child constructor called" << endl;
	}
	~child()
	{
		cout << "child destructor called" << endl;
	}
};
int main()
{
	child * cobj=new child();
	parent *pobj;
	pobj=cobj;
	delete pobj;
}
