#include <iostream>
#include <map>
using namespace std;
  
int main()  
{  
    /************************************************************************/  
    /*                       map�ĳ��÷���ʾ��                              */  
    /************************************************************************/  
  
  
    // map��һ�ֹ��������࣬����洢��Ԫ������Ϊpair<const KEY, DATA>����ͬ��Ԫ��KEYֵ��ͬ��     
    // ����map�����Ӧ�ĵ�����  
    map<char, int> mapTest;  
    map<char, int>::iterator iterTest;  
      
    // ��map�в���Ԫ��  
    // ���������±�ֵ�Ĳ��뷽ʽ����map��û�ж�Ӧ��ֵ��Ԫ��ʱ�����롣��map�д��ڶ�Ӧ��ֵ��Ԫ��ʱ���޸���ֵ���߻�ȡ��ֵ��  
    mapTest['a'] = 10;  
    mapTest['b'] = 20;  
    mapTest['c'] = 30;  
    mapTest['a'] = 15;  
  
    // ����ʹ��insert�Ĳ��뷽ʽ����map��û�ж�Ӧ��ֵ��Ԫ��ʱ�����롣��map�д��ڶ�Ӧ��ֵ��Ԫ��ʱ��������Ԫ�ء�  
    pair<map<char, int>::iterator, bool> ret;  
    mapTest.insert(map<char, int>::value_type('d', 40));  
    ret = mapTest.insert(make_pair('d', 50));  
    mapTest.insert(make_pair('e', 50));  
  
    // ��ʹ��insert������᷵��pair<map<char, int>::iterator, bool>���͵�ֵ��boolֵ��ʾ�Ƿ����ɹ���������ָ������Ԫ�ء�  
    cout << ret.second << endl;  
      
  
    // map�в���ĳ��ָ����ֵ��Ԫ��,���ҳɹ��򷵻ض�Ӧ�ĵ�����������ʧ���򷵻�.end()��Ӧ�������߽��������  
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
  
    // �������  
    cout << "zhengxiang" << endl;
    for (iterTest = mapTest.begin(); iterTest != mapTest.end(); iterTest++)  
    {  
        cout << iterTest->first << " " << iterTest->second << endl;  
    }  
      
    // �������  
    cout << "fanxiang" << endl;
    map<char, int>::reverse_iterator iter;  
    for (iter = mapTest.rbegin(); iter != mapTest.rend(); iter++)  
    {  
        cout << iter->first << " " << iter->second << endl;  
    }  
  
    // ʹ��size��ȡ������Ԫ�ظ���  
    int num;  
    num = (int)mapTest.size();  
    cout << num << endl;  
  
    // ʹ��clear�������  
    mapTest.clear();  
  
    // ʹ��empty�ж������Ƿ�Ϊ��  
    if (mapTest.empty())  
    {  
        cout << "The map is empty" << endl;  
    }  
      
    return 0;  
} 
