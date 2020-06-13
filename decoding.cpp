#include <iostream>
#include <vector>																//вектор
#include <list>																	//список 
#include <map>																	//ассициативный массив
#include <fstream>

using namespace std;


struct Range {																	///структура диапозон
	int a, L, R;																//число, левая и правая граница диапозона
	char c;
};

struct Compare {																//структура, которая помогает сравнивать диапозоны 
	bool operator() (Range l, Range r) const
	{
		return l.a > r.a;
	}
};

int main() {
	setlocale(LC_ALL, "Russian");												//консоль на русский
	char c;																		//для чтения символов и последующей записи
	ifstream f;																	//созданем файла для чтения
	ofstream g;																	//созданем файла для закодированного текста

	f.open("output.txt", ios::binary);												//открываем файл для чтения
	g.open("gotov.txt", ios::binary);											//открываем файл для кодировки

	list<Range> t;																//cписок указателей на Range
	map< char, int>::iterator i;

	map< char, int> m;															//ассоциативный массив с параметрами символ-код
	int a, b;
	f.read((char*)&a, sizeof(a));												//считываем первое число для расшифровки таблциы																//отнимаем значение int
	while (a > 0)
	{
		f.read((char*)&c, sizeof(c));											//считываем символ
		f.read((char*)&b, sizeof(b));											//считываем его значение
		a -= 40;																//отнимаем 8 бит символа и 32 его значения
		m[c] = b;                                                            //строим таблицу
	}

	for (i = m.begin(); i != m.end(); i++) {									//записываем диапозон в начальный список
		Range p;																//создаем новый диапозон и заполняем его
		p.c = i->first;
		p.a = i->second;
		t.push_back(p);															//указатель скидываем в list
	}

	t.sort(Compare());															//сортируем
	t.begin()->R = t.begin()->a;												//определяем диапозон для подсчета "частот"
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
		for (it = t.begin(); it->R <= freq; it++);		// Поиск символа
		int l = l0;
		l0 = l0 + (it->L) * (h0 - l0 + 1) / delitel;
		h0 = l + (it->R) * (h0 - l + 1) / delitel - 1;
		for (;;)										// Обрабатываем варианты
		{								
			if (h0 < Half)								// переполнения
				;										//ничего
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