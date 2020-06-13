#include <iostream>
#include <vector>																//������
#include <list>																	//������ 
#include <map>																	//������������� ������
#include <fstream>

using namespace std;

struct Range {																	///��������� ��������
	int a, L, R;																//�����, ����� � ������ ������� ���������
	char �;																		//�������	
};

struct Compare {																//���������, ������� �������� ���������� ��������� 
	bool operator() (Range l, Range r)
	{
		return l.a > r.a;
	}
};

int main() {
	setlocale(LC_ALL, "Russian");												//������� �� �������
																		
	ifstream f;																	//�������� ����� ��� ������
	ofstream g;																	//�������� ����� ��� ������ ��������������� ������

	f.open("1.txt", ios::binary);												//��������� ���� ��� ������
	g.open("output.txt", ios::binary);											//��������� ���� ��� ������ ��������������� ������

	list<Range> t;																//c����� ���������� �� Range
	map< char, int>::iterator i;
	int count = 0;																//������ ������� � ����������� �����			
	cout << "����� ��������: ���������� �������� � ��������� ���������" << endl;

	map< char, int> m;															//������������� ������ � ����������� ������ - ���
	while (!f.eof()) {															//���� �� ����� �����, ������� ���-�� ���������
		char c = f.get();
		m[c]++;																	//��������� �� ����� ����� (������� �������)
	}
	for (i = m.begin(); i != m.end(); i++) count++;								//������� ���-�� ������ ���������

	cout << count << endl;

	for (i = m.begin(); i != m.end(); i++)										//����� ��������, �������, ��������� � �����
		cout << i->first << " (" << (int)(i->first) << ") " << ":" << i->second << endl;


	for (i = m.begin(); i != m.end(); ++i) {									//���������� �������� � ��������� ������
		Range p;																//������� ����� �������� � ��������� ���
		p.� = i->first;
		p.a = i->second;
		t.push_back(p);															//��������� ��������� � list
	}

	t.sort(Compare());															//���������
	t.begin()->R = t.begin()->a;												//���������� �������� ��� �������� "������"
	t.begin()->L = 0;

	list<Range>::iterator it = t.begin();
	list<Range>::iterator ii=it;
	it++;
	
	for (; it != t.end(); it++)													//�������� ���������� �� ����� ��������
	{
		it->L = ii->R;
		it->R = it->L + it->a;
		ii++;
	}

	int chet = 0;
	for (i = m.begin(); i != m.end(); i++) {									// � ������� ��������� �������� �� ���������
		if (i->second != 0) {
			chet += 40;															//��������� � ������� 40 �������� ����� ���, ����� ������. ��� ��� 8 ��� �������� ������ � 32 ���� - ��� ���		
		}
	}

//������� ������� � ������������ ����
	g.write((char*)(&chet), sizeof(chet));										//���������� ���-�� �������� ��������� (���������� ������)
	for (int l = 0; l < 256; l++) {												//�.� 256 �������
		if (m[char(l)] > 0) {
			char c = char(l);
			g.write((char*)(&c), sizeof(c));									//������� �������
			g.write((char*)(&m[char(l)]), sizeof(m[char(l)]));					//������� �������� �������
		}
	}

	f.clear();																	//�����, ���� �� �������� ������
	f.seekg(0);																	//����������� ��������� �� ����
//�������� ���� �� ��������, ������� �������� ����������.
	int l0 = 0, h0 = 65535, e = 0, delitel = t.back().R;						//delitel=10
	int First_qtr = (h0 + 1) / 4;												// = 16384
	int Half = First_qtr * 2;													// = 32768
	int Thride_qtr = First_qtr * 3;												// = 49152
	int bits_to_follow = 0;														// ������� ����� ����������
	char zap = 0;
	count = 0;																	// ��������������� ����������

	while (!f.eof()) {															//���������� ��� ����� � ����
		char c = f.get();														//������ ������ � ������� ��� ������
		e++;												
		for (it = t.begin(); it != t.end(); it++)
			if (c == it -> �) break;

		if (c != it -> �)
		{
			cout << "Error!" << endl;
			break;
		}

		int l = l0;																//���������� ������
		l0 = l0 + it->L * (h0 - l0 + 1) / delitel;
		h0 = l + it->R * (h0 - l + 1) / delitel - 1;
		for (;;)																//������������ ��������
		{
			if (h0 < Half)														//������������
			{
				count++;
				if (count == 8)													//���� �������� 8, �� ��������� �������� � ���������� � ���� ��������
				{
					g << zap;
					zap = 0;
					count = 0;
				}
				for (; bits_to_follow > 0; bits_to_follow--,count++)			//���� ���-�� ������������ ��� �� ������ ����� 0, ����� ��������� 1 ���������� �����
				{
					zap = zap | (1 << (7 - count));
					if (count == 8)
					{
						g << zap;
						zap = 0;
						count = 0;
					}
				}
			}
			else if (l0 >= Half)												//1�� ��� ����������� 1 � ���������� ���� - 0
			{
				zap = zap | (1 << (7 - count));
				count++;
				if (count == 8)
				{
					g << zap;
					zap = 0;
					count = 0;
				}
				for (; bits_to_follow > 0; bits_to_follow--, count++)
				{
					if (count == 8)
					{
						g << zap;
						zap = 0;
						count = 0;
					}
				}
				l0 -= Half;
				h0 -= Half;
			}
			else if ((l0 >= First_qtr) && (h0 < Thride_qtr))
			{
				l0 -= First_qtr;
				h0 -= First_qtr;
				bits_to_follow++;
			}
			else break;
			l0 += l0;
			h0 += h0+1;
		}
	}
	g << zap;																	//���������� �������
	cout << "��� ��������� ���� �����������." << endl;

	t.clear();
	m.clear();
	f.close();																	//��������� �����
	g.close();

	return 0;
}