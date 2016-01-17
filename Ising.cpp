// Ising.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define INVPMUT 4 //itt könnyen beállítható a mutációs valószínûség (reciproka)

class coupler
{
	//Ez a csatolási mátrixokat tartalmazó objektum(ok) osztályának definíciója.
public:
	int **Vertical=NULL, **Horizontal=NULL;//pointerek a dinamikusan foglalandó mátrixokhoz
	int Size = 0;//A mátrix mérete (N)
	bool Valid = false;//érvényességet jelzõ változó sikertelen inicializáció esetére
	coupler(){}//paramétereket nem váró, nem inicializáló konstruktor
	coupler(int size)//megadott méretû csatolási mátrixokat létrehozó konstruktor 
	{
		int i;
		try
		{
			//Dinamikusan memóriát foglalunk, s kitöltjük õket véletlenszerûen +-1 értékekkel.
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
					//(A rand() generátor 0-tól piszoknagy értékig ad egy véletlen számot.
					//Moduló nem túl nagy számmal esetén 0-tól nem túl nagy szám mínusz egyig terjedõ közel uniform számot ad.
					//Esetünkben nem túl nagy = 2, így a kimenet 0 vagy 1, amit 2-vel szorozva, s 1-gyel csökkentve az eredmény +-1.)
				}
			}
			Size = size;
			Valid = true;
		}
		catch (bad_alloc)
		{
			//Sikertelen memóriafoglalás esetén a már lefoglalt memória felszabadítása
			cerr << "Failed to inicialise coupler object because of unsuccesfull memory allocation!\n";
			if (Vertical!=NULL)
			{
				if (Horizontal!=NULL)
				{
					for (; i >= 0; --i)//a pillanatnyi helyzettõl visszafelé haladó for ciklussal
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
	coupler(coupler& Old)//Másoló konstruktor, hogy a dinamikusan foglalt memóriaterület is másolódjon dinamikusan
	{//enélkül a másolatok megszûntekor meghívódó destruktor az eredeti memóriát szabadítaná fel, amibõl a késõbbiekben acces violation lenne
		int size = Old.Size;
		if (Old.Valid || size > 0)//Csak érvényes objektumot hajlandó másolni
		{
			int i;
			try//Itt is szükséges a dinamikus memóriafoglalás,
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
			catch (bad_alloc)//és kudarc esetén a lefoglalt területet itt is fel kell szabadítani a fordított for ciklussal.
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
	coupler operator=(coupler& rhs)//Értékadó egyenlõségjel operátor definíciója
	{//ennek hiányában nem tudnék mélységeben egyenlõvé tenni két csatoró objektumot egymással
		if (rhs.Valid)//Ez is csak érvényes objektumokkal foglalkozik.
		{
			int size = rhs.Size;
			if (Valid && Size == size)//Ha a bal oldal eleve érvényes, és a jobb oldallal azonos méretû,
			{//akkor elég csak sorra átmásolni a dinamikus memóriaterületet.
				for (int i = 0; i < Size; ++i)
					for (int j = 0; j < Size; ++j)
					{
						Vertical[i][j] = rhs.Vertical[i][j];
						Horizontal[i][j] = rhs.Horizontal[i][j];
					};
			}
			else//Máskülönben a bal oldal által korábban foglalt területet fel kell szabadítani,
			{//és újra memóriát kell foglalni immár a megfelelõ méretben.
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
				catch (bad_alloc)//Persze most is gondolni kell a memóriafoglalás kudarcára.
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
		else//Érvénytelen jobb oldal esetén hibaüzenet:
			cerr << "Trying to assign to invalid coupler object!\n";
		return *this;
	}
	void WriteM(ostream& out=cout)//Mátrixkiíró. Alapesetben a parancssorba ír,
	{//de a fájlba való írást is ez végzi megfelelõ ostream objektumot megadásával.
		if (Valid)//Ez is csak érvényes csatolóobjektummal foglalkozik.
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
	~coupler()//Destruktor. Felszabadítja a dinamikusan foglalt memóriaterületet.
	{
		Valid = false;
		if (Vertical != NULL || Horizontal != NULL)//Bármelyik 2 csillagosra van foglalás, -- biztos, ami biztos --
		{//végignézi a for ciklus az 1 csillagosokat.
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
	//A spinmátrix egyedek osztályának definíciója.
public:
	int** Matrix = NULL;//Pointer a dinamikus memóriaterülethez.
	int Size = 0;//Méret (N).
	bool Valid = false;//Érvényességi változó.
	double Eps;//Egy rácspontra jutó energia.
	entity* Up = NULL;//Pointer a nagyobb energiájú egyedek közül a legkisebbre. (A konstruktor nullán hagyja.)
	entity* Down = NULL;//Pointer a kisebb energiájú egyedek közül a legnagyobbra. (A konstruktor nullán hagyja.)
	entity(){}//Alapértelmezett konstruktor (Enélkül nem tudnám, hogy kell dinamikus tömböt csinálni belõlük.)
	entity(coupler& MyCoupler)//A csatolási mátrix objektum alapján futó konstruktor.
	{//Az nulladik generáció ez által jön létre. Automatikusan meghívja az energiaszámító függvényt.
		if (!MyCoupler.Valid)//Csak érvényes csatolási mátrix objektumra mûkszik.
		{
			cerr << "Failed to initialize entity object because of using invalid coupler object!\n";
			return;
		}
		int size = MyCoupler.Size;
		int i;
		try//Megint a szokásos dinamikus memóriafoglalás.
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
		catch (bad_alloc)//És a hozzá tartozó kivételkezelés.
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
	{//A nulladik utáni generációk ezzel jönnek létre.
		if (!Mother.Valid || !Father.Valid)//Cink, ha anya vagy apa érvénytelen.
		{
			cerr << "Failed to initialize entity object because of invalid parent entity object(s)!\n";
			return;
		}
		if (!Coupler.Valid || Coupler.Size <= 0)//Cink, ha a csatolási mátrix érvénytelen.
		{
			cerr << "Failed to initialize entity object because of invalid coupler object.";
			return;
		}
		int size = Coupler.Size;
		if (Mother.Size != size || Father.Size != size)//Ahogyan az is cink, ha minden érvényes, de a méretek eltérõek.
		{//Elvégre különbözõ fajok se szaporodhatnak egymás közt.
			cerr << "Failed to inicialize entity object because of dimension mismatch.\n";
			return;
		}
		//Véletlenül sorsolunk egy crossover pontot a mátrixban
		int crossi = rand() % size;
		int crossj = rand() % size;
		int i;
		try//Elkezdjük foglalni a memóriát...
		{
			Matrix = new int*[size];
			for (i = 0; i < size; ++i)
			{
				Matrix[i] = new int[size];
				for (int j = 0; j < size; ++j)
				{
					if (i < crossi)//ha a sorindex kisebb mint crossi, az anyai félben vagyunk
						Matrix[i][j] = Mother.Matrix[i][j];
					else if (i > crossi)//ha nagyobb, mint crossi, az apai félben vagyunk
						Matrix[i][j] = Father.Matrix[i][j];
					else if (j < crossj)//ha azonosak, a crossover pont sorában vagyunk, s ekkor az oszlopindex a mérvadó
						Matrix[i][j] = Mother.Matrix[i][j];
					else
						Matrix[i][j] = Father.Matrix[i][j];
				}
			}
			int mut1i, mut1j, mut2i, mut2j;
			do//Rejekcióval kiválasztunk két különbözõ pontot mutációra
			{
				mut1i = rand() % size;
				mut1j = rand() % size;
				mut2i = rand() % size;
				mut2j = rand() % size;
			} while (mut1i == mut2i && mut1j == mut2j);
			//S egyik és másik esetében is (egymástól függetlenül) p valószínûséggel úgy döntünk, dobunk rá egy fej vagy írást.
			if (rand() % INVPMUT == 0)
				Matrix[mut1i][mut1j] = 2 * (rand() % 2) - 1;
			if (rand() % INVPMUT == 0)
				Matrix[mut2i][mut2j] = 2 * (rand() % 2) - 1;
			Size = size;
			Valid = true;
			Eps = Hamilton(Coupler) / (Size * Size);
		}
		catch (bad_alloc)//Végül kezeljük a memóriahibát.
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
	entity(entity& Old)//Másoló konstruktor.
	{
		int size = Old.Size;
		if (Old.Valid && size > 0)//Ha stimmel minden,
		{
			int i;
			try//akkor foglal, és másol,
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
			catch (bad_alloc)//majd hibát kezel.
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
		else//Ha nem stimmel, kiabál.
			cerr << "Trying to copy invalid entity object!\n";
	}
	entity operator=(entity& rhs)//Értékadó egyenlõség.
	{
		if (rhs.Valid)
		{
			int size = rhs.Size;
			if (Valid && Size == size)//Ha minden OK és stimmel a méret, mindent másol...
			{
				for (int i = 0; i < Size; ++i)
					for (int j = 0; j < Size; ++j)
						Matrix[i][j] = rhs.Matrix[i][j];
				Eps = rhs.Eps;
			}
			else//Ha nem, akkor felszabadít,
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
				try//majd újra foglal, és úgy másol...
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
				catch (bad_alloc)//Végül hibát kezel.
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
		else//Ha a jobb oldal érvénytelen, kiabál.
			cerr << "Trying to assign to invalid entity object!\n";
		return *this;
	}
	double Hamilton(coupler& Coupler)//Csatolási mátrix objektumon alapuló energiaszámító függvény.
	{
		if (!Valid || !Coupler.Valid || Size != Coupler.Size)//Ez is kiabál, ha gondot észlel.
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
	int fight(entity**Highest,entity**Lowest)//Harc az életben maradásért.
	{//Ez egy buborék algoritmus része.
		//Az egyed addig tör a legkisebb energiájúnak szánt pozíció felé, míg nála nagyobb energiájú egyedet lát közvetlenül maga elõtt.
		//A visszatérési érték jelzi, hány másik egyedet sikerült leelõzni. Hiba esetén -1.
		//Az egész pointerek csere-beréjérõl szól.
		//Elõtte egy ellenõrzõ algoritmus:
		if (!Valid ||//Gáz van, ha az egyed nem érvényes, vagy (
			(Down != NULL && (!(*Down).Valid || (*Down).Up != this ||//van alatta is egyed, de ( az nem érvényes, vagy nem mutat vissza, vagy ( az alatta levõ egyed
			(*Down).Down != NULL && (!(*((*Down).Down)).Valid || (*((*Down).Down)).Up != Down))) ||//létezik, de ( nem érvényes, vagy nem mutat vissza ) ) )
			(Up != NULL && (!(*Up).Valid || (*Up).Down != this)))//vagy a felfelé lévõ egyed létezik, de ( nem érvényes, esetleg nem mutat vissza ) )
		{
			cerr << "The bubble loop has found an invalid element!\n";
			return -1;
		}
		int counter;//a leelõzés számláló a visszatérési értékhez
		for (counter = 0; Down != NULL && (*Down).Eps > Eps;//addig fut a ciklus, míg alkalmasabb egyedet vagy láncvéget nem lát
			++counter)
		{
			//A pointerek cseréi (a láncvégeket nullpointerek jelzik):
			if ((*Down).Down != NULL)//"Az alsóm alsójának (ha van ilyen) felsõje legyen az alsóm felsõje (vagyis jómagam)."
				(*((*Down).Down)).Up = (*Down).Up;//ami ugyebár this
			else//"Ha nincs, akkor úgy fest, én vagyok a legkisebb, vagyis az abszolút nyerõ."
				(*Lowest) = this;
			(*Down).Up = Up;//"Az alsóm felsõje legyen az én felsõm."
			if (Up != NULL)//Ha van felsõm, az alsója legyen az én alsóm.
				(*Up).Down = Down;
			else//"Ha nincs, akkor úgy fest, én vagyok a legfelsõ, vagyis biztosan búcsúzom az élettõl."
				(*Highest) = Down;
			Up = Down;//"Mostantól a felsõm az eddigi alsóm."
			Down = (*Down).Down;//"Az új alsóm az alsóm alsója lesz."
			(*Up).Down = this;//"A régi alsóm (új felsõm) alsója pedig jómagam leszek.

			//Továbblépés elõtt a ciklus elõtti ellenõrzés ismétlése, most azonban már a valid változót nem kell ellenõrizni:
			//(Ha a program többi része jól van megírva, már a fenti ellenõrzésre sincs szükség.)
			if (Down != NULL && (!(*Down).Valid || (*Down).Up != this ||
				((*Down).Down != NULL && (!(*((*Down).Down)).Valid || (*((*Down).Down)).Up != Down))))//elméletileg ez a sor is fölösleges, de a ciklus kódjainak ellenõrzésére nem árt
			{
				cerr << "The bubble loop has found an invalid element!\n";
				return -1;
			}
		}
		return counter;
	}
	void WriteM(ostream& out=cout)//Mátrixkiíró.
	{//Alapértelmezett a parancssor, de a fájlba is ez ír.
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
	~entity()//Destruktor memóriafelszabadítással.
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

#if 0 //Amikor qsortot akartam használni...
int compare(const void* a, const void* b)
{
	return ((*(entity*)a).Energy - (*(entity*)b).Energy);
}
#endif

class population
{
	//Ez a populációt megtestesítõ objektum osztálydefinícója.
	//A korábbi populációk/generációk is ilyen objektumokban foglalnak helyet,
	//mely objektumokat a main függvény által elsõként létrehozott objektum hoz létre dinamikusan.
	//Ezen objektumok pointerek segítségével összeláncolódnak.
public:
	//Ponter a mátrixegyedek dinamikusan foglalt tömbjéhez, valamint a legnagyobb és a legkisebb energiájú egyedre mutató pointer:
	entity * OurEntities = NULL, *Highest = NULL, *Lowest = NULL;
	//Csatolási mátrix objektum:
	coupler OurCoupler;
	//A populációkat/generációkat összeláncoló pointerek:
	population *Last = NULL, *Next = NULL;
	//Az aktuális populáció mérete, az egyes egyedek mérete, s az eddig létrehozott generációk száma:
	int PopSize = 0, EntSize = 0, GenerationsSofar = 0;
	//Érvényességet, s az egyedek energia szerinti rendezettségét jelzõ indikátorváltozók:
	bool Valid = false, Selected = false;
	population(int entSize, int popSize)//Konstruktor, mely paraméterként csak az egyedek, és az egy generáció méretét várja.
	{
		try//Lesz egy dinamikus memóriafoglalás, melynek bukását kezelni kell.
		{
			OurCoupler = coupler(entSize);
			EntSize = OurCoupler.Size;
			if (!OurCoupler.Valid)//Ha nem sikerült érvényes csatolási mátrix objektumot létrehozni, vége a dalnak.
				return;
			OurEntities = new entity[popSize];//Dinamikus tömbfoglalás. Ehhez kellett a paraméterek nélküli konstruktor.
			for (int i = 0; i < popSize; ++i)
			{
				OurEntities[i] = entity(OurCoupler);//Persze elemenként mindjárt fel is töltõdik a tömb. Ehhez kellett az = operátor definíciója.
				//Azután mindjárt össze is láncolódnak az egyedek, igaz, még csak a létrehozás sorrendjében:
				if (i > 0)
					OurEntities[i - 1].Up = OurEntities + i;
				if (i + 1 < popSize)
					OurEntities[i + 1].Down = OurEntities + i;
			}
			//A legelsõ és legutolsó címének rögzítése:
			Lowest = OurEntities;
			Highest = OurEntities + popSize - 1;
			PopSize = popSize;
			//Végül a pointerek rendezése buborékalgoritmussal:
			if (!Selection())
				Selected = true;
			Valid = true;
		}
		catch (bad_alloc)
		{
			cerr << "Unable to initialize population object because of bad memory allocation!\n";
		}
	}
	population(population& old)//Másoló konstruktor a nagy lista létrehozásához.
	{
		try
		{
			if (!old.Valid)//Csak érvényes populáció objektummal foglalkozik.
			{
				cerr << "Trying to copy invalid population object!\n";
				return;
			}
			EntSize = old.EntSize;
			PopSize = old.PopSize;
			OurCoupler = old.OurCoupler;
			OurEntities = new entity[PopSize];//Itt is van dinamikus foglalás.
			for (int i = 0; i < PopSize; ++i)
			{
				OurEntities[i] = old.OurEntities[i];
				if (old.OurEntities[i].Up != NULL)//A címkülönbségek másolásával a rendezettség is átmásolódik (nincs szükség újabb buborékra).
					OurEntities[i].Up = OurEntities + (old.OurEntities[i].Up - old.OurEntities);
				if (old.OurEntities[i].Down != NULL)
					OurEntities[i].Down = OurEntities + (old.OurEntities[i].Down - old.OurEntities);
			}
			Lowest = OurEntities + (old.Lowest - old.OurEntities);
			Highest = OurEntities + (old.Highest - old.OurEntities);
			GenerationsSofar = old.GenerationsSofar;
			Valid = true;
			Selected = old.Selected;
			if (!Selected)//Persze ha nem voltak rendezettek az egyedek, a konstruktor megpróbálja õket rendezni, s errõl figyelmeztetést ad.
			{
				cerr << "Attemption to copy population object with unordered entity objects.\nTrying to reorder them.";
				Selected = !Selection();
				if (!Selected)//Ennek sikerességérõl/sikertelenségérõl is figyelmeztet.
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
	bool generate(int ngen = 1)//Az új generációt létrehozó függvény.
	{
		//Lehetõség van arra, hogy többször is lefusson a generáló algoritmus, mielõtt az új állapot tárolódik.
		//Ezen futások számát lehet megadni az ngen paraméterrel, melynek alapértelmezett értéke 1.
		//Visszatérési értéke magassal jelzi, ha gond van.
		//Figyelem: ngen > 1 esetben nem minden generáció tárolódik!
		if (ngen <= 0)//Cink a nem pozitív ngen.
		{
			cerr << "Error: The generating function was called for a non-positive integer number of generations.\n";
			return true;
		}
		if (!Valid)//Cink az érvénytelen populáció objektum.
		{
			cerr << "Failed to create new generation because of invalid population object.\n";
			return true;
		}
		if (!Selected)//Cink az energia szerint rendezetlen populáció.
		{
			cerr << "Failed to create new generation because of unordered entity objects.\n";
			return true;
		}
		//A generációk láncának gyors ellenõrzése:
		if (Last != NULL && ((*Last).Next != this ||//Cink továbbá, ha van múltbéli populáció objektum, de ( az pointerével nem mutat vissza
			((*Last).Last != NULL && (*((*Last).Last)).Next != Last)))//vagy neki is van vissza nem mutató elõzõje ).
		{
			cerr << "Invalid chain of population objects!\n";
			return true;
		}
		try//itt is lesz egy dinamikus foglalás
		{
			//Elõször az aktuális generációt új populáció objektumba mentjük:
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
			//Majd ngen-szer futtatjuk a generáló algoritmust:
			for (; ngen > 0; --ngen)
			{//(Emlékszünk az analóg fénymásoló gépekre, ahol beállítottuk a példányszámot, s aztán a számláló visszafelé számolt?)
				if (Mating())//Elõször párosítás.
				{
					cerr << "Failed to create new generation because an error occoured during\nrunning mating algorythm.\n";
					return true;
				}
				++GenerationsSofar;//Jelezzük, hogy létrejött egy új generáció.
				//(Emlékszünk, hogy a fénymásolón volt egy mechanikus számláló, mely számolta, hogy a gép hány másolatot készített addigi életében?)
				if (Selection())//Végül rendezés.
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
	void WriteM()//Mátrixkiíró. Csak a parancssorba ír.
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
			population *OurGeneration = this;//Végighalad a történelmi láncon, hogy minden rögzített generációt kiírjon.
			for (; (*OurGeneration).Last != NULL; ++j)//Elõszor csak a lánc validitásának ellenõrzési szándékával megy végig.
			{
				if ((*((*OurGeneration).Last)).Next != OurGeneration)
				{
					cerr << "<Invalid chain of population objects!>\n";
					cout << "</population>\n";
					return;
				}
				OurGeneration = (*OurGeneration).Last;
			}
			for (; j >= 0; --j)//Majd visszafelé jövet végrehajtja a kiírást.
			{ //(A nulladik generáció a lánc végén van.)
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
	bool Save(wstring Filename)//Mentõ függvény.
	{
		//Csak a fájlnevet várja argumentumként, s magas visszatérési értékkel jelzi, ha gond adódik.
		//Mûködési elvét tekintve azonos a WriteM() függvénnyel.
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
	~population()//Destruktor memóriafelszabadítással és pointernullázással.
	{
		Valid = false;
		if (OurEntities != NULL)
		{
			delete[] OurEntities;
			OurEntities = NULL;
		}
		Highest = NULL;
		Lowest = NULL;
		if (Last != NULL)//Ez az if-ág rekurzívan felszabadítja az objektum utáni láncrészt.
		{
			delete Last;
			Last = NULL;
		}
	}
private:
	//2 függvény kívülrõl nem látszik:
	bool Selection()//A buborék motorja.
	{//Magas visszatérési értékkel jelzi, ha gond van.
		//A képlet egyszerû: Addig meghívni ciklikusan újra meg újra mindegyik mátrix egyedre a harcoló függvényt,
		//amíg azt nem tapasztaljuk, már egyik se tudja leelõzni a másikat.
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
				else if (ans == -1)//Vagy amíg hibába nem botlunk.
					return true;
			}
		} while (run);
		return false;
	}
	bool Mating()//(Szelektáló és) Párosító függvény.
	{//Magas visszatérési értékkel jelzi, ha gond van.
		if (Lowest == NULL)//Ez lehetetlen nagy kakát jelent.
		{
			cerr << "Null pointer @ mating fcn.\n";
			return true;
		}
		entity** Order;//Egy pointer tömbbe rendezve kigyûjtjük a mátrix egyedek memóriacímeit.
		try
		{
			Order = new entity*[PopSize];//Ehhez dinamikusan foglalunk pointereket.
			Order[0] = Lowest;
			int i, j, k;
			for (i = 1; i < PopSize; ++i)//A kigyûjtés ciklusa.
			{
				Order[i] = (*(Order[i - 1])).Up;
				if (Order[i] == NULL)//Ez is lehetetlen nagy kaka.
				{
					cerr << "Null pointer @ mating fcn.\n";
					delete[] Order;
					return true;
				}
			}
			for (i = PopSize / 2; i < PopSize; ++i)//Majd az új egyedek konstruálásának ciklusa.
			{
				do//Ehhez elõször rejekciós módszerrel választunk két nem azonos szülõt.
				{
					j = rand() % (PopSize / 2);
					k = rand() % (PopSize / 2);
				} while (j != k);
				*(Order[i]) = entity(*(Order[j]), *(Order[k]), OurCoupler);//A párosodás.
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
	//A program kétféleképp hívható meg a parancssorból.
	//A: A név után még 4 paraméterrel, melyek rendre:
	//Egyetlen spinmátrix mérete, egyetlen generáció mérete, összes generáció száma, kimeneti fájl neve.
	//B: Csak a név, argumentumok nélkül. A program ekkor "beszélgetni" kezd a felhasználóval, hogy megtudja, mi a célja.
	//Default: Minden más esetben kiírja, hogy "Ön hibás paraméterekkel hívta meg a programot.", és azzal távozik.
	//Azért jó paraméterek esetén is kiírja, hogyan értelmezte a feladatot.
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
		wcout << "Adja meg a spinmátrix méretét!\nA kilépéshez írja: 0\n";
		while (true)
		{
			wcout << "Ising>";
			wcin >> command;
			try
			{
				ascicommand = string(command.begin(), command.end());
				if (ascicommand.compare("0")==0)
					return 0;
				//Valaki mondja meg, mire való a wchar, ha nem tudom általam meghatározott karakterrel összehasonlítani,
				//de a main (bocsánat: _tmain) függvény ilyen formában várja az argumentumát.
				//Ez valami ASCII helyett UNICODE dolog volna? De akkor mért nem lehet ASCII-bõl UNICODE-ot csinálni?
				msize = stoi(command);
				if (msize > 0)
					break;
			}
			catch (invalid_argument){}
			wcout << "A megadott paraméter helytelen. Próbálja újra!\nA kilpéshez írja: 0\n";
		}
		wcout << "Adja meg az egy generáció egyedszámát!\nA kilépéshez írja: 0\n";
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
			wcout << "A megadott paraméter helytelen. Próbálja újra!\nA kilpéshez írja: 0\n";
		}
		wcout << "Adja meg a párosodással létrejövõ generációk számát!\nA kilépéshez írja: 0\n";
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
			wcout << "A megadott paraméter helytelen. Próbálja újra!\nA kilpéshez írja: 0\n";
		}
		wcout << "Adja meg a kimeneti fájl elérési útját és nevét!\nA kilpéshez írja: 0\n";
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
			wcout << "A megadott paraméter helytelen. Próbálja újra!\nA kilpéshez írja: 0\n";
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
		wcout << "Ön helytelen paraméterekkel hívta meg a programot.\n";
		return 0;
	}
	wcout << "Ön " << msize << "x" << msize << "-os/-es/-ös méretû spinmátrixokkal kíván dolgozni.\n";
	wcout << "A véletlen kezdeti generáció után még " << maxgen << " generációt kíván létrehozni.\n";
	wcout << "Minden egyes generáció " << gensize << " egyedbõl áll.\n";
	wcout << "A mentés " << maxgen << " új generáció után történik,\na mentési fájl:\n" << filename << endl;
	srand(time(NULL));//Ha nem inicializálnánk a véletlen számgenerátort, akkor minden egyes futtatáskor ugyanazokat a számokat generálná.
	population OurPopulation(msize, gensize);
	for (int i = 0; i < maxgen; ++i)
		if (OurPopulation.generate())
		{
			wcout << "A genetikai algoritmus futtatásakor hiba történt!\n";
			return 0;
		}
	wcout << "A genetikai algoritmus lefutott.\n";
	if (OurPopulation.Save(filename))
		wcout << "Az eredmények mentése során hiba történt!\n";
	else
		wcout << "Az eredmények mentése sikeres.\n";
	return 0;
}

