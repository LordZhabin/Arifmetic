#include <iostream>
#include <vector>																//вектор
#include <list>																	//список 
#include <map>																	//ассициативный массив
#include <fstream>

using namespace std;

struct Range {																	///структура диапозон
	int a, L, R;																//число, левая и правая граница диапозона
	char с;																		//элемент	
};

struct Compare {																//структура, которая помогает сравнивать диапозоны 
	bool operator() (Range l, Range r)
	{
		return l.a > r.a;
	}
};

int main() {
	setlocale(LC_ALL, "Russian");												//консоль на русский
																		
	ifstream f;																	//созданем файла для чтения
	ofstream g;																	//созданем файла для записи закодированного текста

	f.open("1.txt", ios::binary);												//открываем файл для чтения
	g.open("output.txt", ios::binary);											//открываем файл для записи закодированного текста

	list<Range> t;																//cписок указателей на Range
	map< char, int>::iterator i;
	int count = 0;																//размер массива и последующий вывод			
	cout << "Вывод символов: количество символов в текстовом документе" << endl;

	map< char, int> m;															//ассоциативный массив с параметрами символ - код
	while (!f.eof()) {															//пока не конец файла, считаем кол-во элементов
		char c = f.get();
		m[c]++;																	//считываем из файла текст (включая пробелы)
	}
	for (i = m.begin(); i != m.end(); i++) count++;								//подсчет кол-ва разных элементов

	cout << count << endl;

	for (i = m.begin(); i != m.end(); i++)										//вывод символов, номеров, количесва в файле
		cout << i->first << " (" << (int)(i->first) << ") " << ":" << i->second << endl;


	for (i = m.begin(); i != m.end(); ++i) {									//записываем диапозон в начальный список
		Range p;																//создаем новый диапозон и заполняем его
		p.с = i->first;
		p.a = i->second;
		t.push_back(p);															//указатель скидываем в list
	}

	t.sort(Compare());															//сортируем
	t.begin()->R = t.begin()->a;												//определяем диапозон для подсчета "частот"
	t.begin()->L = 0;

	list<Range>::iterator it = t.begin();
	list<Range>::iterator ii=it;
	it++;
	
	for (; it != t.end(); it++)													//проходим итератором по нашим границам
	{
		it->L = ii->R;
		it->R = it->L + it->a;
		ii++;
	}

	int chet = 0;
	for (i = m.begin(); i != m.end(); i++) {									// с помощью итератора проходим по элементам
		if (i->second != 0) {
			chet += 40;															//прбавляем к таблице 40 символов стоко раз, скоко частот. так как 8 бит занимает символ и 32 бита - его год		
		}
	}

//забивка таблицы в кодированный файл
	g.write((char*)(&chet), sizeof(chet));										//записывает кол-во символов элементов (количество частот)
	for (int l = 0; l < 256; l++) {												//т.к 256 символа
		if (m[char(l)] > 0) {
			char c = char(l);
			g.write((char*)(&c), sizeof(c));									//забивка символа
			g.write((char*)(&m[char(l)]), sizeof(m[char(l)]));					//забивка значения символа
		}
	}

	f.clear();																	//сброс, чтоб не возникло ошибок
	f.seekg(0);																	//перемещение указателя на ноль
//алгоритм взят из учебника, включая название переменных.
	int l0 = 0, h0 = 65535, e = 0, delitel = t.back().R;						//delitel=10
	int First_qtr = (h0 + 1) / 4;												// = 16384
	int Half = First_qtr * 2;													// = 32768
	int Thride_qtr = First_qtr * 3;												// = 49152
	int bits_to_follow = 0;														// Сколько битов сбрасывать
	char zap = 0;
	count = 0;																	// вспомогательная переменная

	while (!f.eof()) {															//записываем наш текст в файл
		char c = f.get();														//читаем символ и находим его индекс
		e++;												
		for (it = t.begin(); it != t.end(); it++)
			if (c == it -> с) break;

		if (c != it -> с)
		{
			cout << "Error!" << endl;
			break;
		}

		int l = l0;																//предыдущий элемен
		l0 = l0 + it->L * (h0 - l0 + 1) / delitel;
		h0 = l + it->R * (h0 - l + 1) / delitel - 1;
		for (;;)																//обрабатываем варианты
		{
			if (h0 < Half)														//переполнение
			{
				count++;
				if (count == 8)													//если достигли 8, то обновляем значение и записываем в файл значение
				{
					g << zap;
					zap = 0;
					count = 0;
				}
				for (; bits_to_follow > 0; bits_to_follow--,count++)			//пока кол-во сбрасываемых бит не станет равно 0, будем заполнять 1 оставшийся биайт
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
			else if (l0 >= Half)												//1ый бит заполняется 1 а отсавшийся байт - 0
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
	g << zap;																	//запихиваем остаток
	cout << "Ваш текстовый файл закодирован." << endl;

	t.clear();
	m.clear();
	f.close();																	//закрываем файлы
	g.close();

	return 0;
}