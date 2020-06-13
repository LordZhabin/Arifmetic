#include <iostream>
#include <vector>																//������
#include <list>																	//������ 
#include <map>																	//������������� ������
#include <fstream>

using namespace std;


struct Range {																	///��������� ��������
	int a, L, R;																//�����, ����� � ������ ������� ���������
	char c;
};

struct Compare {																//���������, ������� �������� ���������� ��������� 
	bool operator() (Range l, Range r) const
	{
		return l.a > r.a;
	}
};

int main() {
	setlocale(LC_ALL, "Russian");												//������� �� �������
	char c;																		//��� ������ �������� � ����������� ������
	ifstream f;																	//�������� ����� ��� ������
	ofstream g;																	//�������� ����� ��� ��������������� ������

	f.open("output.txt", ios::binary);												//��������� ���� ��� ������
	g.open("gotov.txt", ios::binary);											//��������� ���� ��� ���������

	list<Range> t;																//c����� ���������� �� Range
	map< char, int>::iterator i;

	map< char, int> m;															//������������� ������ � ����������� ������-���
	int a, b;
	f.read((char*)&a, sizeof(a));												//��������� ������ ����� ��� ����������� �������																//�������� �������� int
	while (a > 0)
	{
		f.read((char*)&c, sizeof(c));											//��������� ������
		f.read((char*)&b, sizeof(b));											//��������� ��� ��������
		a -= 40;																//�������� 8 ��� ������� � 32 ��� ��������
		m[c] = b;                                                            //������ �������
	}

	for (i = m.begin(); i != m.end(); i++) {									//���������� �������� � ��������� ������
		Range p;																//������� ����� �������� � ��������� ���
		p.c = i->first;
		p.a = i->second;
		t.push_back(p);															//��������� ��������� � list
	}

	t.sort(Compare());															//���������
	t.begin()->R = t.begin()->a;												//���������� �������� ��� �������� "������"
	t.begin()->L = 0;

	list<Range>::iterator it = t.begin();
	list<Range>::iterator ii = it;
	it++;

	for (; it != t.end(); it++)
	{
		it->L = ii->R;
		it->R = it->L + it->a;
		ii++;
	}
	
	int l0 = 0, h0 = 65535, delitel = t.back().R;
	int First_qtr = (h0 + 1) / 4;						// = 16384
	int Half = First_qtr * 2;							// = 32768
	int Thride_qtr = First_qtr * 3;						// = 49152
	int value = (f.get() << 8) | f.get();
	char zap = f.get();
	int count = 0;
	while (!f.eof())
	{
		int freq = ((value - l0 + 1) * delitel - 1) / (h0 - l0 + 1);
		for (it = t.begin(); it->R <= freq; it++);		// ����� �������
		int l = l0;
		l0 = l0 + (it->L) * (h0 - l0 + 1) / delitel;
		h0 = l + (it->R) * (h0 - l + 1) / delitel - 1;
		for (;;)										// ������������ ��������
		{								
			if (h0 < Half)								// ������������
				;										//������
			else if (l0 >= Half)
			{
				value -= Half;
				l0 -= Half;
				h0 -= Half;
			}
			else if ((l0 >= First_qtr) && (h0 < Thride_qtr))
			{
				value -= First_qtr;
				l0 -= First_qtr;
				h0 -= First_qtr;
			}
			else break;
			l0 += l0;
			h0 += h0 + 1;
			value += value + (((short)zap >> (7 - count)) & 1);
			count++;
			
			if (count == 8)
			{
				zap = f.get();
				count = 0;
			}
		}
		g << it->c;
	}
	f.close();
	g.close();
	return 0;
}