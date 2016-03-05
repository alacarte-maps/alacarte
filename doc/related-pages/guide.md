Programing guide	{#guide}
================


Hier sind Sachen die die anderen wissen sollten zusammengefasset.


Kompilieren
-----------

Zum Kompilieren unter Linux:

Abhängigkeiten:
- Boost		(Ubuntu: libboost-all-dev)
- CMake		(Ubuntu: cmake)
- Doxygen	(Ubuntu: doxygen)
- Cairo     (Ubuntu: libcairo2-dev)

1.	Installiere die Abhängigkeiten.
2.	Gehe ins Hauptverzeichnis von alacarte (also da wo ihr "include" und "src" seht)
3.	Erstellt einen Ordner "build" (der wird von git ignoriert)
~~~~~~~~~~~~~~
	mkdir build
	cd build
~~~~~~~~~~~~~~
4.	Führe cmake auf den Hauptordner aus
~~~~~~~~~~~~~~
	cmake ..
~~~~~~~~~~~~~~
5.	Nun ist alles da was ihr zum Kompilieren braucht.
	Wenn ihr jetzt irgendwie kompilieren wollt, einfach in den build ordner gehen und make eingeben
~~~~~~~~~~~~~~
	make
~~~~~~~~~~~~~~
6.	Zum erstellen der Dokumentation (Doxygen muss installiert sein!).
~~~~~~~~~~~~~~
	make doc
~~~~~~~~~~~~~~

Dokumentation
-------------
Einfach [hier](http://www.stack.nl/~dimitri/doxygen/manual.html) schauen, ich bin zu faul dazu was zu sagen.


Related pages
-------------
Ihr könnt auch zusätzliche Seiten in die Dokumentation einfügen. So wie diese hier.
Dafür einfach eine .md Datei unter

> doc/related-pages/

anlegen. Warum .md? Weil markdown einfach einfach ist. Es müssten auch noch andere Sachen gehen,
aber mit markdown geht es schnell und ist einfach. Weite Infos zu markdown in doxygen-Dokumentationen
findet ihr [hier](http://www.stack.nl/~dimitri/doxygen/markdown.html),
denn doxygen unterstützt irgendwie nicht den ganzen markdown-standard.

Exceptions
----------

An sich kann man in C++ alles werfen. Da wir aber einheitlich programmieren wollen, sollte man sich auf etwas einigen.
Zur Zeit wird boost::exceptions verwendet. Dafür ist es besonders wichtig, zu verstehen wie das genze funktioniert.

Also als erstes [diese Seite](http://www.boost.org/doc/libs/1_52_0/libs/exception/doc/motivation.html) lesen!

Da ihr das da oben ja jetzt gelesen habt, weiter im text. Ich hab mir das so überlegt, dass wir unsere Exceptions
in der utils/exceptions.hpp lagern; im Namespace excp. Einfach eine Exception definieren, wie es da schon steht.
Falls ihr Informationen in der Exception mitschicken wollt. einfach so benutzen wie es da steht ;)


Log
---

Wir benutzen log4cpp. Um Etwas aufs log zu schreiben, einfach eine Loginstance holen. Entweder root oder eine subcategorie.

~~~~~~~~~~~~~~~~~~~{.cpp}
log4cpp::Category& rootLog = log4cpp::Category::getRoot();

rootLog << log4cpp::Priority::INFO << "Ich bin eine Nachricht und werde auf der Konsole und in einer Datei ausgegeben";
~~~~~~~~~~~~~~~~~~~

Weitere Infos gibts [hier](http://log4cpp.sourceforge.net/).

Testen
------

Wie ihr ja sicher wisst, benutzen wir boost::test. Hier ein kleiner Einstieg, wie das bis jetzt organisiert ist.
Für genauere Infos siehe [hier](http://www.boost.org/doc/libs/1_52_0/libs/test/doc/html/index.html).


Wir haben dafür einen eigenen tests ordner. Am ende sollen 4 Test Programme herrauskommen:

1. general
2. utils
3. importer
4. server

In diesen Ordnern findet sich immer eine ..._test_module.cpp. Diese gibt nur den Test an, also brauch nicht zu verändert werden.
Dann könnt ihr für jede Klasse einen Unterordner anlegen und in diesem die Tests machen. Also wenn ihr einen test für Geodata schreiben
wollt, macht ihr dass z.b. in 

> tests/general/geodata-test/geodata_functional_test.cpp

### boost::unit_test ###
Ok kommen wir zu dem test inhalt selbst. Boost benutzt eine suite-tree um Tests zu organisieren.
Ganz oben ist das master-suite. Wenn ihr nichts weiter angebt, werden eure Tests hier registriert.
Mit BOOST_AUTO_TEST_SUITE könnt ihr eine suite aufmachen (intern ist es nur ein namespace).
Dann benutzt BOOST_AUTO_TEST_CASE um euren test zu beginnen und zu beenden.
Hier ein Simpler Point test:

~~~~~~~~~~~~~~~~~{.cpp}
#include <boost/test/unit_test.hpp>

#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)

BOOST_AUTO_TEST_CASE( point_test )
{
	BOOST_CHECK_EQUAL( Point(3,3), Point(3,3));
	BOOST_CHECK_EQUAL( Point(4,3), Point(3,3));
	BOOST_CHECK_EQUAL( Point(5,3), Point(3,3));
}

// Zum schluss noch suite schließen.
BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)

~~~~~~~~~~~~~~~~~

Output:

~~~~~~~~~~~~
Running 1 test case...
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(10): error in "point_test": check Point(4,3) == Point(3,3) failed [4, 3 != 3, 3]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(11): error in "point_test": check Point(5,3) == Point(3,3) failed [5, 3 != 3, 3]

*** 2 failures detected in test suite "utils test"
~~~~~~~~~~~~

### Test cases mit Parametern ###

Wenn ihr eine Testfunktion mit verschiedenen Parametern testen  wollt, hab ich da ein kleines Makro gebaut, mit dem das besser funktioniert.

~~~~~~~~~~~~~~~~~~{.cpp}
#include "../../tests.hpp"
#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)


void equal_test(const Point& p1, const Point& p2)
{
	BOOST_CHECK_EQUAL( p1, p2);
}

ALAC_PARAM_TEST_CASE(equal_test, Point(3, 4), Point(3, 4));
ALAC_PARAM_TEST_CASE(equal_test, Point(3, 8), Point(3, 4));
ALAC_PARAM_TEST_CASE(equal_test, Point(3, 4), Point(3, 8));


BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
~~~~~~~~~~~~~~~~~~

Output:

~~~~~~~~~~~~~~~~~~
Running 3 test cases...
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(9): error in "equal_test_in_13": check p1 == p2 failed [3, 8 != 3, 4]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(9): error in "equal_test_in_14": check p1 == p2 failed [3, 4 != 3, 8]

*** 2 failures detected in test suite "utils test"
~~~~~~~~~~~~~~~~~~

### test cases mit Kontext ###

Manchmal möchte man auch erst einen Test vorbereiten, dann die Tests durchführen und dann (manchmal ;) ) auf aufräumen

~~~~~~~~~~~~~~~~~~~{.cpp}
#include "../../tests.hpp"
#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)

struct simple_check_test
{
	// Im Konstruktor bereiten wir unseren test vor.
	// Dabei können wir uns beliebig viele Parameter geben lassen.
	simple_check_test(const Point& p)
	{
		point = p;
	}

	~simple_check_test()
	{
		// Hier würden wir aufräumen, aber hir gibts nichts zu tun.
	}

	// Hier schreiben wir auch direkt unsere test cases rein.
	// Die können wieder beliebig heißen und beliebige parameter entgegen nehmen
	void compare(const Point& p)
	{
		BOOST_CHECK_EQUAL(point, p);
	}

	// Natürlich könen wir auch member für unseren test erstellen
	Point point;
};

// Hier machen wir nun einen test
// Wir geben unsere struktur von oben an
// Danach kommen argumente für den konstruktor
ALAC_START_FIXTURE_TEST(simple_check_test, Point(5, 5))
	// Alle folgenden tests werden oben auf unserer struct ausgeführt
	// Merke: Das attribute point ist jetzt natürlich ein Punkt auf 5,5
	// Jetzt geben wir einfach nur noch die test cases an und rufen sie it parametern auf.
	ALAC_FIXTURE_TEST(compare, Point(4,5));
	ALAC_FIXTURE_TEST(compare, Point(4,6));
	// Normalerweise wird an einen test-namen die zeile rengehängt, um ihn unique zu machen
	// Die meisten makros können aber mit _NAMED dahinter angegeben werden.
	// Das ermöglicht es einen eigenen Namen anzugeben (hier compare_4_and_7).
	// (siehe output für einen unterschied)
	ALAC_FIXTURE_TEST_NAMED(compare, compare_4_and_7, Point(4,7));
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
~~~~~~~~~~~~~~~~~~~
Output:

~~~~~~~~~~~~~~~~~~~
Running 3 test cases...
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(27): error in "compare_in_34": check point == p failed [5, 5 != 4, 5]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(27): error in "compare_in_35": check point == p failed [5, 5 != 4, 6]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(27): error in "compare_4_and_7": check point == p failed [5, 5 != 4, 7]

*** 3 failures detected in test suite "utils test"
~~~~~~~~~~~~~~~~~~~


