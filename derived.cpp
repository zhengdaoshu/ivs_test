#include<iostream>
using namespace std;
class Base
{
	public:
	virtual void f(float x) { cout<< "Base:: f(float)" << x << endl; }
	void g(float x) { cout<< "Base:: g(float)" << x << endl; }
	void h(float x) { cout<< "Base:: h(float)" << x << endl; }
};

class Derived : public Base
{
	public:
	virtual void f(float x) { cout<< "Derived:: f(float)" << x << endl; }
	void g(int x) { cout<< "Derived:: g(int)" << x << endl; }
	void h(float x) { cout<< "Derived:: h(float)" << x << endl; }
};

int main()
{
	Derived d;
	Base *pb = &d;
	Derived *pd = &d;
	// Good : behavi or depends solely on type of the object
	pb->f(3.14f) ;  // Derived:: f ( float ) 3.14
	pd->f(3.14f) ;  // Derived:: f (float ) 3.14
	// Bad : behavi or depends on t ype of t hepointer
	pb->g(3.14f) ;  // Base::g(float) 3. 14
	pd->g(3.14f) ;  // Derived:: g( int ) 3 (surprise!)
	// Bad : behavi or depends on type of the pointer
	pb->h(3.14f) ;  // Base:: h( float ) 3. 14 ( surprise!)
	pd->h(3.14f) ;  // Derived:: h( float ) 3. 14
	return 0;
}
