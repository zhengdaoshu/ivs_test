#include <iostream>
#include <map>
using namespace std;
  
int main()  
{  
    /************************************************************************/  
    /*                       map的常用方法示例                              */  
    /************************************************************************/  
  
  
    // map是一种关联容器类，里面存储的元素类型为pair<const KEY, DATA>。不同的元素KEY值不同。     
    // 定义map及其对应的迭代器  
    map<char, int> mapTest;  
    map<char, int>::iterator iterTest;  
      
    // 在map中插入元素  
    // 这种利用下标值的插入方式，当map中没有对应键值的元素时，插入。当map中存在对应键值的元素时，修改其值或者获取其值。  
    mapTest['a'] = 10;  
    mapTest['b'] = 20;  
    mapTest['c'] = 30;  
    mapTest['a'] = 15;  
  
    // 这种使用insert的插入方式，当map中没有对应键值的元素时，插入。当map中存在对应键值的元素时，不插入元素。  
    pair<map<char, int>::iterator, bool> ret;  
    mapTest.insert(map<char, int>::value_type('d', 40));  
    ret = mapTest.insert(make_pair('d', 50));  
    mapTest.insert(make_pair('e', 50));  
  
    // 当使用insert函数后会返回pair<map<char, int>::iterator, bool>类型的值，bool值表示是否插入成功。迭代器指向插入的元素。  
    cout << ret.second << endl;  
      
  
    // map中查找某个指定键值的元素,查找成功则返回对应的迭代器。查找失败则返回.end()对应的容器边界迭代器。  
    iterTest = mapTest.find('f');  
    cout << (iterTest == mapTest.end()) << " find: 0 means success, 1 means failure"<< endl;

    iterTest = mapTest.find('b');
    if(iterTest != mapTest.end())
    {
    	cout <<"ok "<<iterTest->first << " " << iterTest->second << endl;
    	iterTest++;
    	cout <<"ok2 "<<iterTest->first << " " << iterTest->second << endl;

    	iterTest--;
    	cout <<"ok3 "<<iterTest->first << " " << iterTest->second << endl;

    	cout<<"mapTest['c']="<<mapTest['c']<<endl;
    }
  
    // 正向遍历  
    cout << "zhengxiang" << endl;
    for (iterTest = mapTest.begin(); iterTest != mapTest.end(); iterTest++)  
    {  
        cout << iterTest->first << " " << iterTest->second << endl;  
    }  
      
    // 反向遍历  
    cout << "fanxiang" << endl;
    map<char, int>::reverse_iterator iter;  
    for (iter = mapTest.rbegin(); iter != mapTest.rend(); iter++)  
    {  
        cout << iter->first << " " << iter->second << endl;  
    }  
  
    // 使用size获取容器中元素个数  
    int num;  
    num = (int)mapTest.size();  
    cout << num << endl;  
  
    // 使用clear清空容器  
    mapTest.clear();  
  
    // 使用empty判断容器是否为空  
    if (mapTest.empty())  
    {  
        cout << "The map is empty" << endl;  
    }  
      
    return 0;  
} 
