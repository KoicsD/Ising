// Ising.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define INVPMUT 4 //itt k�nnyen be�ll�that� a mut�ci�s val�sz�n�s�g (reciproka)

class coupler
{
	//Ez a csatol�si m�trixokat tartalmaz� objektum(ok) oszt�ly�nak defin�ci�ja.
public:
	int **Vertical=NULL, **Horizontal=NULL;//pointerek a dinamikusan foglaland� m�trixokhoz
	int Size = 0;//A m�trix m�rete (N)
	bool Valid = false;//�rv�nyess�get jelz� v�ltoz� sikertelen inicializ�ci� eset�re
	coupler(){}//param�tereket nem v�r�, nem inicializ�l� konstruktor
	coupler(int size)//megadott m�ret� csatol�si m�trixokat l�trehoz� konstruktor 
	{
		int i;
		try
		{
			//Dinamikusan mem�ri�t foglalunk, s kit�ltj�k �ket v�letlenszer�en +-1 �rt�kekkel.
			Vertical = new int*[size];
			Horizontal = new int*[size];
			for (i = 0; i < size; ++i)
			{
				Vertical[i] = new int[size];
				Horizontal[i] = new int[size];
				for (int j = 0; j < size; ++j)
				{
					Vertical[i][j] = 2 * (rand() % 2) - 1;
					Horizontal[i][j] = 2 * (rand() % 2) - 1;
					//(A rand() gener�tor 0-t�l piszoknagy �rt�kig ad egy v�letlen sz�mot.
					//Modul� nem t�l nagy sz�mmal eset�n 0-t�l nem t�l nagy sz�m m�nusz egyig terjed� k�zel uniform sz�mot ad.
					//Eset�nkben nem t�l nagy = 2, �gy a kimenet 0 vagy 1, amit 2-vel szorozva, s 1-gyel cs�kkentve az eredm�ny +-1.)
				}
			}
			Size = size;
			Valid = true;
		}
		catch (bad_alloc)
		{
			//Sikertelen mem�riafoglal�s eset�n a m�r lefoglalt mem�ria felszabad�t�sa
			cerr << "Failed to inicialise coupler object because of unsuccesfull memory allocation!\n";
			if (Vertical!=NULL)
			{
				if (Horizontal!=NULL)
				{
					for (; i >= 0; --i)//a pillanatnyi helyzett�l visszafel� halad� for ciklussal
					{
						if (Vertical[i] != NULL)
						{
							if (Horizontal[i] != NULL)
							{
								delete[] Horizontal[i];
								Horizontal[i] = NULL;
							}
							delete[] Vertical[i];
							Vertical[i] = NULL;
						}
					}
					delete[] Horizontal;
					Horizontal=NULL;
				}
				delete[] Vertical;
				Vertical=NULL;
			}
		}
	}
	coupler(coupler& Old)//M�sol� konstruktor, hogy a dinamikusan foglalt mem�riater�let is m�sol�djon dinamikusan
	{//en�lk�l a m�solatok megsz�ntekor megh�v�d� destruktor az eredeti mem�ri�t szabad�tan� fel, amib�l a k�s�bbiekben acces violation lenne
		int size = Old.Size;
		if (Old.Valid || size > 0)//Csak �rv�nyes objektumot hajland� m�solni
		{
			int i;
			try//Itt is sz�ks�ges a dinamikus mem�riafoglal�s,
			{
				Vertical = new int*[size];
				Horizontal = new int*[size];
				for (i = 0; i < size; ++i)
				{
					Vertical[i] = new int[size];
					Horizontal[i] = new int[size];
					for (int j = 0; j < size; ++j)
					{
						Vertical[i][j] = Old.Vertical[i][j];
						Horizontal[i][j] = Old.Horizontal[i][j];
					}
				}
				Size = size;
				Valid = true;
			}
			catch (bad_alloc)//�s kudarc eset�n a lefoglalt ter�letet itt is fel kell szabad�tani a ford�tott for ciklussal.
			{
				cerr << "Failed to inicialise copy coupler object because of unsuccesfull memory allocation!\n";
				if (Vertical != NULL)
				{
					if (Horizontal != NULL)
					{
						for (; i >= 0; --i)
						{
							if (Vertical[i] != NULL)
							{
								if (Horizontal[i] != NULL)
								{
									delete[] Horizontal[i];
									Horizontal[i] = NULL;
								}
								delete[] Vertical[i];
								Vertical[i] = NULL;
							}
						}
						delete[] Horizontal;
						Horizontal = NULL;
					}
					delete[] Vertical;
					Vertical = NULL;
				}
			}
		}
		else
			cerr << "Trying to copy invalid coupler object!\n";
	}
	coupler operator=(coupler& rhs)//�rt�kad� egyenl�s�gjel oper�tor defin�ci�ja
	{//ennek hi�ny�ban nem tudn�k m�lys�geben egyenl�v� tenni k�t csator� objektumot egym�ssal
		if (rhs.Valid)//Ez is csak �rv�nyes objektumokkal foglalkozik.
		{
			int size = rhs.Size;
			if (Valid && Size == size)//Ha a bal oldal eleve �rv�nyes, �s a jobb oldallal azonos m�ret�,
			{//akkor el�g csak sorra �tm�solni a dinamikus mem�riater�letet.
				for (int i = 0; i < Size; ++i)
					for (int j = 0; j < Size; ++j)
					{
						Vertical[i][j] = rhs.Vertical[i][j];
						Horizontal[i][j] = rhs.Horizontal[i][j];
					};
			}
			else//M�sk�l�nben a bal oldal �ltal kor�bban foglalt ter�letet fel kell szabad�tani,
			{//�s �jra mem�ri�t kell foglalni imm�r a megfelel� m�retben.
				Valid = false;
				if (Vertical != NULL || Horizontal != NULL)
				{
					for (int i = Size - 1; i >= 0; --i)
					{
						if (Vertical[i] != NULL)
						{
							delete[] Vertical[i];
							Vertical[i] = NULL;
						}
						if (Horizontal[i] != NULL)
						{
							delete[] Horizontal[i];
							Horizontal[i] = NULL;
						}
					}
					if (Vertical != NULL)
					{
						delete[] Vertical;
						Vertical = NULL;
					}
					if (Horizontal != NULL)
					{
						delete[] Horizontal;
						Horizontal = NULL;
					}
				}
				Size = 0;
				int i;
				try
				{
					Vertical = new int*[size];
					Horizontal = new int*[size];
					for (i = 0; i < size; ++i)
					{
						Vertical[i] = new int[size];
						Horizontal[i] = new int[size];
						for (int j = 0; j < size; ++j)
						{
							Vertical[i][j] = rhs.Vertical[i][j];
							Horizontal[i][j] = rhs.Horizontal[i][j];
						}
					}
					Size = size;
					Valid = true;
				}
				catch (bad_alloc)//Persze most is gondolni kell a mem�riafoglal�s kudarc�ra.
				{
					cerr << "Failed to reinicialize entity object because of unsuccesfull memory allocation!\n";
					if (Vertical != NULL || Horizontal != NULL)
					{
						for (; i >= 0; --i)
						{
							if (Vertical[i] != NULL)
							{
								delete[] Vertical[i];
								Vertical[i] = NULL;
							}
							if (Horizontal[i] != NULL)
							{
								delete[] Horizontal[i];
								Horizontal[i] = NULL;
							}
						}
						if (Vertical != NULL)
						{
							delete[] Vertical;
							Vertical = NULL;
						}
					}
				}
			}
		}
		else//�rv�nytelen jobb oldal eset�n hiba�zenet:
			cerr << "Trying to assign to invalid coupler object!\n";
		return *this;
	}
	void WriteM(ostream& out=cout)//M�trixki�r�. Alapesetben a parancssorba �r,
	{//de a f�jlba val� �r�st is ez v�gzi megfelel� ostream objektumot megad�s�val.
		if (Valid)//Ez is csak �rv�nyes csatol�objektummal foglalkozik.
		{
			out << "Vertical:\n";
			for (int i = 0; i < Size; ++i)
			{
				for (int j = 0; j < Size; ++j)
					out << Vertical[i][j] << " ";
				out << endl;
			}
			out << "Horizontal:\n";
			for (int i = 0; i < Size; ++i)
			{
				for (int j = 0; j < Size; ++j)
					out << Horizontal[i][j] << " ";
				out << endl;
			}
		}
		else
			out << "<Invalid object.>\n";
	}
	~coupler()//Destruktor. Felszabad�tja a dinamikusan foglalt mem�riater�letet.
	{
		Valid = false;
		if (Vertical != NULL || Horizontal != NULL)//B�rmelyik 2 csillagosra van foglal�s, -- biztos, ami biztos --
		{//v�gign�zi a for ciklus az 1 csillagosokat.
			for (int i = Size - 1; i >= 0; --i)
			{
				if (Horizontal != NULL && Horizontal[i] != NULL)
				{
					delete[] Horizontal[i];
					Horizontal[i] = NULL;
				}
				if (Vertical != NULL && Vertical[i] != NULL)
				{
					delete[] Vertical[i];
					Vertical[i] = NULL;
				}
			}
			if (Horizontal != NULL)
			{
				delete[] Horizontal;
				Horizontal = NULL;
			}
			if (Vertical != NULL)
			{
				delete[] Vertical;
				Vertical = NULL;
			}
		}
		Size = 0;
	}
};

class entity
{
	//A spinm�trix egyedek oszt�ly�nak defin�ci�ja.
public:
	int** Matrix = NULL;//Pointer a dinamikus mem�riater�lethez.
	int Size = 0;//M�ret (N).
	bool Valid = false;//�rv�nyess�gi v�ltoz�.
	double Eps;//Egy r�cspontra jut� energia.
	entity* Up = NULL;//Pointer a nagyobb energi�j� egyedek k�z�l a legkisebbre. (A konstruktor null�n hagyja.)
	entity* Down = NULL;//Pointer a kisebb energi�j� egyedek k�z�l a legnagyobbra. (A konstruktor null�n hagyja.)
	entity(){}//Alap�rtelmezett konstruktor (En�lk�l nem tudn�m, hogy kell dinamikus t�mb�t csin�lni bel�l�k.)
	entity(coupler& MyCoupler)//A csatol�si m�trix objektum alapj�n fut� konstruktor.
	{//Az nulladik gener�ci� ez �ltal j�n l�tre. Automatikusan megh�vja az energiasz�m�t� f�ggv�nyt.
		if (!MyCoupler.Valid)//Csak �rv�nyes csatol�si m�trix objektumra m�kszik.
		{
			cerr << "Failed to initialize entity object because of using invalid coupler object!\n";
			return;
		}
		int size = MyCoupler.Size;
		int i;
		try//Megint a szok�sos dinamikus mem�riafoglal�s.
		{
			Matrix = new int*[size];
			for (i = 0; i < size; ++i)
			{
				Matrix[i] = new int[size];
				for (int j = 0; j < size; ++j)
				{
					Matrix[i][j] = 2 * (rand() % 2) - 1;
				}
			}
			Size = size;
			Valid = true;
			Eps = Hamilton(MyCoupler) / (Size * Size);
		}
		catch (bad_alloc)//�s a hozz� tartoz� kiv�telkezel�s.
		{
			cerr << "Failed to inicialize entity object because of unsuccesfull memory allocation!\n";
			if (Matrix != NULL)
			{
				for (--i; i >= 0; --i)
				{
					delete[] Matrix[i];
					Matrix[i] = NULL;
				}
				delete[] Matrix;
				Matrix = NULL;
			}
		}
	}
	entity(entity& Mother, entity& Father, coupler& Coupler)//"Ivaros" konstruktor.
	{//A nulladik ut�ni gener�ci�k ezzel j�nnek l�tre.
		if (!Mother.Valid || !Father.Valid)//Cink, ha anya vagy apa �rv�nytelen.
		{
			cerr << "Failed to initialize entity object because of invalid parent entity object(s)!\n";
			return;
		}
		if (!Coupler.Valid || Coupler.Size <= 0)//Cink, ha a csatol�si m�trix �rv�nytelen.
		{
			cerr << "Failed to initialize entity object because of invalid coupler object.";
			return;
		}
		int size = Coupler.Size;
		if (Mother.Size != size || Father.Size != size)//Ahogyan az is cink, ha minden �rv�nyes, de a m�retek elt�r�ek.
		{//Elv�gre k�l�nb�z� fajok se szaporodhatnak egym�s k�zt.
			cerr << "Failed to inicialize entity object because of dimension mismatch.\n";
			return;
		}
		//V�letlen�l sorsolunk egy crossover pontot a m�trixban
		int crossi = rand() % size;
		int crossj = rand() % size;
		int i;
		try//Elkezdj�k foglalni a mem�ri�t...
		{
			Matrix = new int*[size];
			for (i = 0; i < size; ++i)
			{
				Matrix[i] = new int[size];
				for (int j = 0; j < size; ++j)
				{
					if (i < crossi)//ha a sorindex kisebb mint crossi, az anyai f�lben vagyunk
						Matrix[i][j] = Mother.Matrix[i][j];
					else if (i > crossi)//ha nagyobb, mint crossi, az apai f�lben vagyunk
						Matrix[i][j] = Father.Matrix[i][j];
					else if (j < crossj)//ha azonosak, a crossover pont sor�ban vagyunk, s ekkor az oszlopindex a m�rvad�
						Matrix[i][j] = Mother.Matrix[i][j];
					else
						Matrix[i][j] = Father.Matrix[i][j];
				}
			}
			int mut1i, mut1j, mut2i, mut2j;
			do//Rejekci�val kiv�lasztunk k�t k�l�nb�z� pontot mut�ci�ra
			{
				mut1i = rand() % size;
				mut1j = rand() % size;
				mut2i = rand() % size;
				mut2j = rand() % size;
			} while (mut1i == mut2i && mut1j == mut2j);
			//S egyik �s m�sik eset�ben is (egym�st�l f�ggetlen�l) p val�sz�n�s�ggel �gy d�nt�nk, dobunk r� egy fej vagy �r�st.
			if (rand() % INVPMUT == 0)
				Matrix[mut1i][mut1j] = 2 * (rand() % 2) - 1;
			if (rand() % INVPMUT == 0)
				Matrix[mut2i][mut2j] = 2 * (rand() % 2) - 1;
			Size = size;
			Valid = true;
			Eps = Hamilton(Coupler) / (Size * Size);
		}
		catch (bad_alloc)//V�g�l kezelj�k a mem�riahib�t.
		{
			cerr << "Failed to inicialize entity object because of unsuccesfull memory allocation!\n";
			if (Matrix != NULL)
			{
				for (--i; i >= 0; --i)
				{
					delete[] Matrix[i];
					Matrix[i] = NULL;
				}
				delete[] Matrix;
				Matrix = NULL;
			}
		}
	}
	entity(entity& Old)//M�sol� konstruktor.
	{
		int size = Old.Size;
		if (Old.Valid && size > 0)//Ha stimmel minden,
		{
			int i;
			try//akkor foglal, �s m�sol,
			{
				Matrix = new int*[size];
				for (i = 0; i < size; ++i)
				{
					Matrix[i] = new int[size];
					for (int j = 0; j < size; ++j)
					{
						Matrix[i][j] = Old.Matrix[i][j];
					}
				}
				Size = size;
				Eps = Old.Eps;
				Valid = true;
			}
			catch (bad_alloc)//majd hib�t kezel.
			{
				cerr << "Failed to inicialize entity object because of unsuccesfull memory allocation!\n";
				if (Matrix != NULL)
				{
					for (--i; i >= 0; --i)
					{
						delete[] Matrix[i];
						Matrix[i] = NULL;
					}
					delete[] Matrix;
					Matrix = NULL;
				}
			}
		}
		else//Ha nem stimmel, kiab�l.
			cerr << "Trying to copy invalid entity object!\n";
	}
	entity operator=(entity& rhs)//�rt�kad� egyenl�s�g.
	{
		if (rhs.Valid)
		{
			int size = rhs.Size;
			if (Valid && Size == size)//Ha minden OK �s stimmel a m�ret, mindent m�sol...
			{
				for (int i = 0; i < Size; ++i)
					for (int j = 0; j < Size; ++j)
						Matrix[i][j] = rhs.Matrix[i][j];
				Eps = rhs.Eps;
			}
			else//Ha nem, akkor felszabad�t,
			{
				Valid = false;
				if (Matrix != NULL)
				{
					for (int i = Size - 1; i >= 0; --i)
						if (Matrix[i] != NULL)
						{
							delete[] Matrix[i];
							Matrix[i] = NULL;
						}
					delete[] Matrix;
					Matrix = NULL;
				}
				Size = 0;
				int i;
				try//majd �jra foglal, �s �gy m�sol...
				{
					Matrix = new int*[size];
					for (i = 0; i < size; ++i)
					{
						Matrix[i] = new int[size];
						for (int j = 0; j < size; ++j)
						{
							Matrix[i][j] = 2 * (rand() % 2) - 1;
						}
					}
					Size = size;
					Eps = rhs.Eps;
					Valid = true;
				}
				catch (bad_alloc)//V�g�l hib�t kezel.
				{
					cerr << "Failed to reinicialize entity object because of unsuccesfull memory allocation!\n";
					if (Matrix != NULL)
					{
						for (--i; i >= 0; --i)
						{
							delete[] Matrix[i];
							Matrix[i] = NULL;
						}
						delete[] Matrix;
						Matrix = NULL;
					}
				}
			}
		}
		else//Ha a jobb oldal �rv�nytelen, kiab�l.
			cerr << "Trying to assign to invalid entity object!\n";
		return *this;
	}
	double Hamilton(coupler& Coupler)//Csatol�si m�trix objektumon alapul� energiasz�m�t� f�ggv�ny.
	{
		if (!Valid || !Coupler.Valid || Size != Coupler.Size)//Ez is kiab�l, ha gondot �szlel.
		{
			cerr << "Energy cannot be evaluated.\n";
			return NAN;
		}
		int sum = 0;
		for (int i = 0; i < Size; ++i)
			for (int j = 0; j < Size; ++j)
			{
				sum -= Coupler.Vertical[i][j] * Matrix[i][j] * Matrix[(i + 1) % Size][j];
				sum -= Coupler.Horizontal[i][j] * Matrix[i][j] * Matrix[i][(j + 1) % Size];
			};
		return double(sum);
	}
	int fight(entity**Highest,entity**Lowest)//Harc az �letben marad�s�rt.
	{//Ez egy bubor�k algoritmus r�sze.
		//Az egyed addig t�r a legkisebb energi�j�nak sz�nt poz�ci� fel�, m�g n�la nagyobb energi�j� egyedet l�t k�zvetlen�l maga el�tt.
		//A visszat�r�si �rt�k jelzi, h�ny m�sik egyedet siker�lt leel�zni. Hiba eset�n -1.
		//Az eg�sz pointerek csere-ber�j�r�l sz�l.
		//El�tte egy ellen�rz� algoritmus:
		if (!Valid ||//G�z van, ha az egyed nem �rv�nyes, vagy (
			(Down != NULL && (!(*Down).Valid || (*Down).Up != this ||//van alatta is egyed, de ( az nem �rv�nyes, vagy nem mutat vissza, vagy ( az alatta lev� egyed
			(*Down).Down != NULL && (!(*((*Down).Down)).Valid || (*((*Down).Down)).Up != Down))) ||//l�tezik, de ( nem �rv�nyes, vagy nem mutat vissza ) ) )
			(Up != NULL && (!(*Up).Valid || (*Up).Down != this)))//vagy a felfel� l�v� egyed l�tezik, de ( nem �rv�nyes, esetleg nem mutat vissza ) )
		{
			cerr << "The bubble loop has found an invalid element!\n";
			return -1;
		}
		int counter;//a leel�z�s sz�ml�l� a visszat�r�si �rt�khez
		for (counter = 0; Down != NULL && (*Down).Eps > Eps;//addig fut a ciklus, m�g alkalmasabb egyedet vagy l�ncv�get nem l�t
			++counter)
		{
			//A pointerek cser�i (a l�ncv�geket nullpointerek jelzik):
			if ((*Down).Down != NULL)//"Az als�m als�j�nak (ha van ilyen) fels�je legyen az als�m fels�je (vagyis j�magam)."
				(*((*Down).Down)).Up = (*Down).Up;//ami ugyeb�r this
			else//"Ha nincs, akkor �gy fest, �n vagyok a legkisebb, vagyis az abszol�t nyer�."
				(*Lowest) = this;
			(*Down).Up = Up;//"Az als�m fels�je legyen az �n fels�m."
			if (Up != NULL)//Ha van fels�m, az als�ja legyen az �n als�m.
				(*Up).Down = Down;
			else//"Ha nincs, akkor �gy fest, �n vagyok a legfels�, vagyis biztosan b�cs�zom az �lett�l."
				(*Highest) = Down;
			Up = Down;//"Mostant�l a fels�m az eddigi als�m."
			Down = (*Down).Down;//"Az �j als�m az als�m als�ja lesz."
			(*Up).Down = this;//"A r�gi als�m (�j fels�m) als�ja pedig j�magam leszek.

			//Tov�bbl�p�s el�tt a ciklus el�tti ellen�rz�s ism�tl�se, most azonban m�r a valid v�ltoz�t nem kell ellen�rizni:
			//(Ha a program t�bbi r�sze j�l van meg�rva, m�r a fenti ellen�rz�sre sincs sz�ks�g.)
			if (Down != NULL && (!(*Down).Valid || (*Down).Up != this ||
				((*Down).Down != NULL && (!(*((*Down).Down)).Valid || (*((*Down).Down)).Up != Down))))//elm�letileg ez a sor is f�l�sleges, de a ciklus k�djainak ellen�rz�s�re nem �rt
			{
				cerr << "The bubble loop has found an invalid element!\n";
				return -1;
			}
		}
		return counter;
	}
	void WriteM(ostream& out=cout)//M�trixki�r�.
	{//Alap�rtelmezett a parancssor, de a f�jlba is ez �r.
		if (Valid)
		{
			out << "Matrix =\n";
			for (int i = 0; i < Size; ++i)
			{
				for (int j = 0; j < Size; ++j)
					out << Matrix[i][j] << " ";
				out << endl;
			}
			out << "Eps = " << Eps << endl;
		}
		else
			out << "<Invalid object.>\n";
	}
	~entity()//Destruktor mem�riafelszabad�t�ssal.
	{
		Valid = false;
		if (Matrix != NULL)
		{
			for (int i = Size - 1; i >= 0; --i)
				if (Matrix[i] != NULL)
				{
					delete[] Matrix[i];
					Matrix[i] = NULL;
				}
			delete[] Matrix;
			Matrix = NULL;
		}
		Size = 0;
	}
};

#if 0 //Amikor qsortot akartam haszn�lni...
int compare(const void* a, const void* b)
{
	return ((*(entity*)a).Energy - (*(entity*)b).Energy);
}
#endif

class population
{
	//Ez a popul�ci�t megtestes�t� objektum oszt�lydefin�c�ja.
	//A kor�bbi popul�ci�k/gener�ci�k is ilyen objektumokban foglalnak helyet,
	//mely objektumokat a main f�ggv�ny �ltal els�k�nt l�trehozott objektum hoz l�tre dinamikusan.
	//Ezen objektumok pointerek seg�ts�g�vel �sszel�ncol�dnak.
public:
	//Ponter a m�trixegyedek dinamikusan foglalt t�mbj�hez, valamint a legnagyobb �s a legkisebb energi�j� egyedre mutat� pointer:
	entity * OurEntities = NULL, *Highest = NULL, *Lowest = NULL;
	//Csatol�si m�trix objektum:
	coupler OurCoupler;
	//A popul�ci�kat/gener�ci�kat �sszel�ncol� pointerek:
	population *Last = NULL, *Next = NULL;
	//Az aktu�lis popul�ci� m�rete, az egyes egyedek m�rete, s az eddig l�trehozott gener�ci�k sz�ma:
	int PopSize = 0, EntSize = 0, GenerationsSofar = 0;
	//�rv�nyess�get, s az egyedek energia szerinti rendezetts�g�t jelz� indik�torv�ltoz�k:
	bool Valid = false, Selected = false;
	population(int entSize, int popSize)//Konstruktor, mely param�terk�nt csak az egyedek, �s az egy gener�ci� m�ret�t v�rja.
	{
		try//Lesz egy dinamikus mem�riafoglal�s, melynek buk�s�t kezelni kell.
		{
			OurCoupler = coupler(entSize);
			EntSize = OurCoupler.Size;
			if (!OurCoupler.Valid)//Ha nem siker�lt �rv�nyes csatol�si m�trix objektumot l�trehozni, v�ge a dalnak.
				return;
			OurEntities = new entity[popSize];//Dinamikus t�mbfoglal�s. Ehhez kellett a param�terek n�lk�li konstruktor.
			for (int i = 0; i < popSize; ++i)
			{
				OurEntities[i] = entity(OurCoupler);//Persze elemenk�nt mindj�rt fel is t�lt�dik a t�mb. Ehhez kellett az = oper�tor defin�ci�ja.
				//Azut�n mindj�rt �ssze is l�ncol�dnak az egyedek, igaz, m�g csak a l�trehoz�s sorrendj�ben:
				if (i > 0)
					OurEntities[i - 1].Up = OurEntities + i;
				if (i + 1 < popSize)
					OurEntities[i + 1].Down = OurEntities + i;
			}
			//A legels� �s legutols� c�m�nek r�gz�t�se:
			Lowest = OurEntities;
			Highest = OurEntities + popSize - 1;
			PopSize = popSize;
			//V�g�l a pointerek rendez�se bubor�kalgoritmussal:
			if (!Selection())
				Selected = true;
			Valid = true;
		}
		catch (bad_alloc)
		{
			cerr << "Unable to initialize population object because of bad memory allocation!\n";
		}
	}
	population(population& old)//M�sol� konstruktor a nagy lista l�trehoz�s�hoz.
	{
		try
		{
			if (!old.Valid)//Csak �rv�nyes popul�ci� objektummal foglalkozik.
			{
				cerr << "Trying to copy invalid population object!\n";
				return;
			}
			EntSize = old.EntSize;
			PopSize = old.PopSize;
			OurCoupler = old.OurCoupler;
			OurEntities = new entity[PopSize];//Itt is van dinamikus foglal�s.
			for (int i = 0; i < PopSize; ++i)
			{
				OurEntities[i] = old.OurEntities[i];
				if (old.OurEntities[i].Up != NULL)//A c�mk�l�nbs�gek m�sol�s�val a rendezetts�g is �tm�sol�dik (nincs sz�ks�g �jabb bubor�kra).
					OurEntities[i].Up = OurEntities + (old.OurEntities[i].Up - old.OurEntities);
				if (old.OurEntities[i].Down != NULL)
					OurEntities[i].Down = OurEntities + (old.OurEntities[i].Down - old.OurEntities);
			}
			Lowest = OurEntities + (old.Lowest - old.OurEntities);
			Highest = OurEntities + (old.Highest - old.OurEntities);
			GenerationsSofar = old.GenerationsSofar;
			Valid = true;
			Selected = old.Selected;
			if (!Selected)//Persze ha nem voltak rendezettek az egyedek, a konstruktor megpr�b�lja �ket rendezni, s err�l figyelmeztet�st ad.
			{
				cerr << "Attemption to copy population object with unordered entity objects.\nTrying to reorder them.";
				Selected = !Selection();
				if (!Selected)//Ennek sikeress�g�r�l/sikertelens�g�r�l is figyelmeztet.
					cerr << "Reordering failed!\n";
				else
					cerr << "Succes!\n";
			}
		}
		catch (bad_alloc)
		{
			cerr << "Unable to inicialize population object because of bad memory allocation!\n";
		}
	}
	bool generate(int ngen = 1)//Az �j gener�ci�t l�trehoz� f�ggv�ny.
	{
		//Lehet�s�g van arra, hogy t�bbsz�r is lefusson a gener�l� algoritmus, miel�tt az �j �llapot t�rol�dik.
		//Ezen fut�sok sz�m�t lehet megadni az ngen param�terrel, melynek alap�rtelmezett �rt�ke 1.
		//Visszat�r�si �rt�ke magassal jelzi, ha gond van.
		//Figyelem: ngen > 1 esetben nem minden gener�ci� t�rol�dik!
		if (ngen <= 0)//Cink a nem pozit�v ngen.
		{
			cerr << "Error: The generating function was called for a non-positive integer number of generations.\n";
			return true;
		}
		if (!Valid)//Cink az �rv�nytelen popul�ci� objektum.
		{
			cerr << "Failed to create new generation because of invalid population object.\n";
			return true;
		}
		if (!Selected)//Cink az energia szerint rendezetlen popul�ci�.
		{
			cerr << "Failed to create new generation because of unordered entity objects.\n";
			return true;
		}
		//A gener�ci�k l�nc�nak gyors ellen�rz�se:
		if (Last != NULL && ((*Last).Next != this ||//Cink tov�bb�, ha van m�ltb�li popul�ci� objektum, de ( az pointer�vel nem mutat vissza
			((*Last).Last != NULL && (*((*Last).Last)).Next != Last)))//vagy neki is van vissza nem mutat� el�z�je ).
		{
			cerr << "Invalid chain of population objects!\n";
			return true;
		}
		try//itt is lesz egy dinamikus foglal�s
		{
			//El�sz�r az aktu�lis gener�ci�t �j popul�ci� objektumba mentj�k:
			if (Last == NULL)
			{
				Last = new population(*this);
				(*Last).Next = this;
			}
			else
			{
				population *Tmp = Last;
				Last = new population(*this);
				(*Last).Next = this;
				(*Last).Last = Tmp;
				(*((*Last).Last)).Next = Last;
			}
			//Majd ngen-szer futtatjuk a gener�l� algoritmust:
			for (; ngen > 0; --ngen)
			{//(Eml�ksz�nk az anal�g f�nym�sol� g�pekre, ahol be�ll�tottuk a p�ld�nysz�mot, s azt�n a sz�ml�l� visszafel� sz�molt?)
				if (Mating())//El�sz�r p�ros�t�s.
				{
					cerr << "Failed to create new generation because an error occoured during\nrunning mating algorythm.\n";
					return true;
				}
				++GenerationsSofar;//Jelezz�k, hogy l�trej�tt egy �j gener�ci�.
				//(Eml�ksz�nk, hogy a f�nym�sol�n volt egy mechanikus sz�ml�l�, mely sz�molta, hogy a g�p h�ny m�solatot k�sz�tett addigi �let�ben?)
				if (Selection())//V�g�l rendez�s.
				{
					cerr << "Failed to order entities of new generation.\n";
					Selected = false;
					return true;
				}
			}
			return false;
		}
		catch (bad_alloc)
		{
			cerr << "Failed to create new generation because of unsuccesful memory allocation!\n";
			return true;
		}
	}
	void WriteM()//M�trixki�r�. Csak a parancssorba �r.
	{
		if (Valid)
		{
			cout << "<Parameters>\n";
			cout << "Size of entities = " << EntSize << endl;
			cout << "Number of entities per generations = " << PopSize << endl;
			cout << "Total number of mating-created generations = " << GenerationsSofar << endl;
			cout << "</Parameters>\n" << "<Couplers>\n";
			OurCoupler.WriteM();
			cout << "</Couplers>\n" << "<Population>\n";
			int j = 0;
			population *OurGeneration = this;//V�gighalad a t�rt�nelmi l�ncon, hogy minden r�gz�tett gener�ci�t ki�rjon.
			for (; (*OurGeneration).Last != NULL; ++j)//El�szor csak a l�nc validit�s�nak ellen�rz�si sz�nd�k�val megy v�gig.
			{
				if ((*((*OurGeneration).Last)).Next != OurGeneration)
				{
					cerr << "<Invalid chain of population objects!>\n";
					cout << "</population>\n";
					return;
				}
				OurGeneration = (*OurGeneration).Last;
			}
			for (; j >= 0; --j)//Majd visszafel� j�vet v�grehajtja a ki�r�st.
			{ //(A nulladik gener�ci� a l�nc v�g�n van.)
				cout << "<Generation #" << (*OurGeneration).GenerationsSofar << ">\n";
				for (int i = 0; i < (*OurGeneration).PopSize; ++i)
				{
					cout << "<Entity #" << i + 1 << ">\n";
					(*OurGeneration).OurEntities[i].WriteM();
					cout << "</Entity #" << i + 1 << ">\n";
				}
				cout << "</Generation #" << (*OurGeneration).GenerationsSofar << ">\n";
				OurGeneration = (*OurGeneration).Next;
			}
			cout << "</Population>\n";
		}
		else
			cout << "<Invalid population object.>\n";
	}
	bool Save(wstring Filename)//Ment� f�ggv�ny.
	{
		//Csak a f�jlnevet v�rja argumentumk�nt, s magas visszat�r�si �rt�kkel jelzi, ha gond ad�dik.
		//M�k�d�si elv�t tekintve azonos a WriteM() f�ggv�nnyel.
		ofstream OurFile;
		if (!Valid)
		{
			cerr << "Invalid population object!\n";
			return true;
		}
		try
		{
			OurFile.open(Filename);
			OurFile << "<Parameters>\n";
			OurFile << "Size of entities = " << EntSize << endl;
			OurFile << "Number of entities per generations = " << PopSize << endl;
			OurFile << "Total number of mating-created generations = " << GenerationsSofar << endl;
			OurFile << "</Parameters>\n" << "<Couplers>\n";
			OurCoupler.WriteM(OurFile);
			OurFile << "</Couplers>\n" << "<Population>\n";
			int j = 0;
			population *OurGeneration = this;
			for (; (*OurGeneration).Last != NULL; ++j)
			{
				if ((*((*OurGeneration).Last)).Next != OurGeneration)
				{
					cerr << "Invalid chain of population objects!\n";
					OurFile << "<!Error: Invalid chain of population (generation) objects.>\n" << "</population>\n";
					OurFile.close();
					return true;
				}
				OurGeneration = (*OurGeneration).Last;
			}
			for (; j >= 0; --j)
			{
				OurFile << "<Generation #" << (*OurGeneration).GenerationsSofar << ">\n";
				for (int i = 0; i < (*OurGeneration).PopSize; ++i)
				{
					OurFile << "<Entity #" << i + 1 << ">\n";
					(*OurGeneration).OurEntities[i].WriteM(OurFile);
					OurFile << "</Entity #" << i + 1 << ">\n";
				}
				OurFile << "</Generation #" << (*OurGeneration).GenerationsSofar << ">\n";
				OurGeneration = (*OurGeneration).Next;
			}
			OurFile << "</Population>\n";
			OurFile.close();
			return false;
		}
		catch (exception)
		{
			if (OurFile.is_open())
				OurFile.close();
			cerr << "Stream writing error!\n";
			return true;
		}
	}
	~population()//Destruktor mem�riafelszabad�t�ssal �s pointernull�z�ssal.
	{
		Valid = false;
		if (OurEntities != NULL)
		{
			delete[] OurEntities;
			OurEntities = NULL;
		}
		Highest = NULL;
		Lowest = NULL;
		if (Last != NULL)//Ez az if-�g rekurz�van felszabad�tja az objektum ut�ni l�ncr�szt.
		{
			delete Last;
			Last = NULL;
		}
	}
private:
	//2 f�ggv�ny k�v�lr�l nem l�tszik:
	bool Selection()//A bubor�k motorja.
	{//Magas visszat�r�si �rt�kkel jelzi, ha gond van.
		//A k�plet egyszer�: Addig megh�vni ciklikusan �jra meg �jra mindegyik m�trix egyedre a harcol� f�ggv�nyt,
		//am�g azt nem tapasztaljuk, m�r egyik se tudja leel�zni a m�sikat.
		bool run;
		int ans;
		do
		{
			run = false;
			for (int i = 0; i < PopSize; ++i)
			{
				ans = OurEntities[i].fight(&Highest, &Lowest);
				if (ans > 0)
					run = true;
				else if (ans == -1)//Vagy am�g hib�ba nem botlunk.
					return true;
			}
		} while (run);
		return false;
	}
	bool Mating()//(Szelekt�l� �s) P�ros�t� f�ggv�ny.
	{//Magas visszat�r�si �rt�kkel jelzi, ha gond van.
		if (Lowest == NULL)//Ez lehetetlen nagy kak�t jelent.
		{
			cerr << "Null pointer @ mating fcn.\n";
			return true;
		}
		entity** Order;//Egy pointer t�mbbe rendezve kigy�jtj�k a m�trix egyedek mem�riac�meit.
		try
		{
			Order = new entity*[PopSize];//Ehhez dinamikusan foglalunk pointereket.
			Order[0] = Lowest;
			int i, j, k;
			for (i = 1; i < PopSize; ++i)//A kigy�jt�s ciklusa.
			{
				Order[i] = (*(Order[i - 1])).Up;
				if (Order[i] == NULL)//Ez is lehetetlen nagy kaka.
				{
					cerr << "Null pointer @ mating fcn.\n";
					delete[] Order;
					return true;
				}
			}
			for (i = PopSize / 2; i < PopSize; ++i)//Majd az �j egyedek konstru�l�s�nak ciklusa.
			{
				do//Ehhez el�sz�r rejekci�s m�dszerrel v�lasztunk k�t nem azonos sz�l�t.
				{
					j = rand() % (PopSize / 2);
					k = rand() % (PopSize / 2);
				} while (j != k);
				*(Order[i]) = entity(*(Order[j]), *(Order[k]), OurCoupler);//A p�rosod�s.
			}
			delete[] Order;
			return false;
		}
		catch (bad_alloc)
		{
			cerr << "Bad alloc @ mating fcn.\n";
			return true;
		}
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	//A program k�tf�lek�pp h�vhat� meg a parancssorb�l.
	//A: A n�v ut�n m�g 4 param�terrel, melyek rendre:
	//Egyetlen spinm�trix m�rete, egyetlen gener�ci� m�rete, �sszes gener�ci� sz�ma, kimeneti f�jl neve.
	//B: Csak a n�v, argumentumok n�lk�l. A program ekkor "besz�lgetni" kezd a felhaszn�l�val, hogy megtudja, mi a c�lja.
	//Default: Minden m�s esetben ki�rja, hogy "�n hib�s param�terekkel h�vta meg a programot.", �s azzal t�vozik.
	//Az�rt j� param�terek eset�n is ki�rja, hogyan �rtelmezte a feladatot.
	wstring* commands;
	int msize, gensize, maxgen;
	wstring command, filename;
	string ascicommand;
	commands = new wstring[argc];
	for (int i = 0; i < argc; ++i)
	{
		commands[i] = argv[i];
	}
	switch (argc)
	{
	case 1 :
		wcout << "Adja meg a spinm�trix m�ret�t!\nA kil�p�shez �rja: 0\n";
		while (true)
		{
			wcout << "Ising>";
			wcin >> command;
			try
			{
				ascicommand = string(command.begin(), command.end());
				if (ascicommand.compare("0")==0)
					return 0;
				//Valaki mondja meg, mire val� a wchar, ha nem tudom �ltalam meghat�rozott karakterrel �sszehasonl�tani,
				//de a main (bocs�nat: _tmain) f�ggv�ny ilyen form�ban v�rja az argumentum�t.
				//Ez valami ASCII helyett UNICODE dolog volna? De akkor m�rt nem lehet ASCII-b�l UNICODE-ot csin�lni?
				msize = stoi(command);
				if (msize > 0)
					break;
			}
			catch (invalid_argument){}
			wcout << "A megadott param�ter helytelen. Pr�b�lja �jra!\nA kilp�shez �rja: 0\n";
		}
		wcout << "Adja meg az egy gener�ci� egyedsz�m�t!\nA kil�p�shez �rja: 0\n";
		while (true)
		{
			wcout << "Ising>";
			wcin >> command;
			try
			{
				ascicommand = string(command.begin(), command.end());
				if (ascicommand.compare("0") == 0)
					return 0;
				gensize = stoi(command);
				if (gensize > 0)
					break;
			}
			catch (invalid_argument){}
			wcout << "A megadott param�ter helytelen. Pr�b�lja �jra!\nA kilp�shez �rja: 0\n";
		}
		wcout << "Adja meg a p�rosod�ssal l�trej�v� gener�ci�k sz�m�t!\nA kil�p�shez �rja: 0\n";
		while (true)
		{
			wcout << "Ising>";
			wcin >> command;
			try
			{
				ascicommand = string(command.begin(), command.end());
				if (ascicommand.compare("0") == 0)
					return 0;
				maxgen = stoi(command);
				if (maxgen > 0)
					break;
			}
			catch (invalid_argument){}
			wcout << "A megadott param�ter helytelen. Pr�b�lja �jra!\nA kilp�shez �rja: 0\n";
		}
		wcout << "Adja meg a kimeneti f�jl el�r�si �tj�t �s nev�t!\nA kilp�shez �rja: 0\n";
		while (true)
		{
			wcout << "Ising>";
			wcin >> command;
			try
			{
				ascicommand = string(command.begin(), command.end());
				if (ascicommand.compare("0") == 0)
					return 0;
				filename = command;
				break;
			}
			catch (invalid_argument){}
			wcout << "A megadott param�ter helytelen. Pr�b�lja �jra!\nA kilp�shez �rja: 0\n";
		}
		break;
	case 5 :
		try
		{
			msize = stoi(commands[1]);
			gensize = stoi(commands[2]);
			maxgen = stoi(commands[3]);
			filename = commands[4];
			if (msize <= 0 || gensize <= 0 || maxgen <= 0)
				goto hiba;
		}
		catch (invalid_argument)
		{
			goto hiba;
		}
		break;
	default :
		hiba:
		wcout << "�n helytelen param�terekkel h�vta meg a programot.\n";
		return 0;
	}
	wcout << "�n " << msize << "x" << msize << "-os/-es/-�s m�ret� spinm�trixokkal k�v�n dolgozni.\n";
	wcout << "A v�letlen kezdeti gener�ci� ut�n m�g " << maxgen << " gener�ci�t k�v�n l�trehozni.\n";
	wcout << "Minden egyes gener�ci� " << gensize << " egyedb�l �ll.\n";
	wcout << "A ment�s " << maxgen << " �j gener�ci� ut�n t�rt�nik,\na ment�si f�jl:\n" << filename << endl;
	srand(time(NULL));//Ha nem inicializ�ln�nk a v�letlen sz�mgener�tort, akkor minden egyes futtat�skor ugyanazokat a sz�mokat gener�ln�.
	population OurPopulation(msize, gensize);
	for (int i = 0; i < maxgen; ++i)
		if (OurPopulation.generate())
		{
			wcout << "A genetikai algoritmus futtat�sakor hiba t�rt�nt!\n";
			return 0;
		}
	wcout << "A genetikai algoritmus lefutott.\n";
	if (OurPopulation.Save(filename))
		wcout << "Az eredm�nyek ment�se sor�n hiba t�rt�nt!\n";
	else
		wcout << "Az eredm�nyek ment�se sikeres.\n";
	return 0;
}

