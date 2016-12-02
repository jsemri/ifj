class Main {
    static void run() {
        int i; double d; String s;
        
        p("Délka prázného řetězce");
        /**/ifj16.length("");
        i = ifj16.length("");
        d = ifj16.length("");
        checkEqual(i, d);
        
        p("Délka řetězce o jednom znaku");
        /**/ifj16.length("a");
        s = "a";
        i = ifj16.length("a");
        d = ifj16.length(s);
        checkEqual(i, d);
        
        p("Délka řetězce o 12 znacích");
        s = "asjcw--aasjc";
        /**/ifj16.length(s);
        i = ifj16.length(s);
        d = ifj16.length(s);
        checkEqual(i, d);

        
        Shorcuts.subs("prázdný řetězec", "", 0, 0);
        Shorcuts.subs("nula znaků z delšího řetězce", "Lorem ipsum", 0, 0);
        Shorcuts.subs("nula znaků z delšího řetězce #2", "Lorem ipsum", 5, 0);
        Shorcuts.subs("První znak", "Lorem ipsum", 0, 1);
        Shorcuts.subs("Poslední znak", "Lorem ipsum", 10, 1);
        Shorcuts.subs("Znaky uprostřed", "Lorem ipsum", 3, 2);
        Shorcuts.subs("Celý řetězec", "Lorem ipsum", 0, 11);
        
        Shorcuts.cmp("dva prázdný", "", "");
        Shorcuts.cmp("první prázdný", "", "Ahoj");
        Shorcuts.cmp("druhý prázdný", "Ahoj", "");
        Shorcuts.cmp("dva stejný", "Ahoj", "Ahoj");
        Shorcuts.cmp("první má znak navíc", "Ahoj ", "Ahoj");
        Shorcuts.cmp("druhý má znak navíc", "Ahoj", "Ahoj ");
        Shorcuts.cmp("první menší", "ahoj", "světe");
        Shorcuts.cmp("první větší", "světe", "ahoj");
        
        Shorcuts.find("dva prázdný", "", "");
        Shorcuts.find("první prázdný", "", "Ahoj");
        Shorcuts.find("druhý prázdný", "Ahoj", "");
        Shorcuts.find("dva stejný", "Ahoj", "Ahoj");
        Shorcuts.find("první má znak navíc", "Ahoj ", "Ahoj");
        Shorcuts.find("druhý má znak navíc", "Ahoj", "Ahoj ");
        Shorcuts.find("první menší", "ahoj", "světe");
        Shorcuts.find("první větší", "světe", "ahoj");
        Shorcuts.find("na začátku", "Skákal pes přes oves", "Skákal");
        Shorcuts.find("nutnost se vrátit", "SSSSSSkákal pes", "Skákal");
        Shorcuts.find("další vracení", "SSkáSSkákaSSkákal pes", "Skákal");
        Shorcuts.find("na konci", "Skákal pes přes oves", "oves");
        Shorcuts.find("skoro na konci, ale znak chybí", "Lazy dog", "dogs");
        
        Shorcuts.sort("prázdný řetězec", "");
        Shorcuts.sort("jeden znak", "A");
        Shorcuts.sort("několik stejných znaků", "!!!!!!!!!!");
        Shorcuts.sort("již seřazený řetězec", "cennost");
        Shorcuts.sort("opačně seřazený řetězec", "tsoledb");
        Shorcuts.sort("opakující se znaky", "tamara");
        Shorcuts.sort("normální řetězec", "V saline bylo 42 lidi.");
    }
    
    static int counter = 1;
    static void p(String name) {
        if (counter > 1) {
            ifj16.print("\n");
        }
        ifj16.print("[TEST " + counter + "] " + name + "\n");
        counter = counter + 1;
    }
    
    static void checkEqual(int i, double d) {
        if (d - i > 0.01) {
            ifj16.print("Čísla " + i + " a " + d + " nejsou totožná!\n");
        }
        else if (i - d > 0.01) {
            ifj16.print("Čísla " + i + " a " + d + " nejsou totožná!\n");
        }
        else {
            ifj16.print(i + "\n");
        }
    }
}

class Shorcuts {
    static void subs(String testName, String in, int i, int n) {
        testName = "Substring: " + testName;
        Main.p(testName);
        /**/ifj16.substr(in, i, n);
        String s = ifj16.substr(in, i, n);
        ifj16.print(s + "\n");
    }

    static void cmp(String testName, String a, String b) {
        testName = "Porovnání řetězců: " + testName;
        Main.p(testName);
        /**/ifj16.compare(a, b);
        int i = ifj16.compare(a, b);
        double d = ifj16.compare(a, b);
        Main.checkEqual(i, d);
    }
    
    static void find(String testName, String a, String b) {
        testName = "Hledání podřetězce: " + testName;
        Main.p(testName);
        /**/ifj16.find(a, b);
        int i = ifj16.find(a, b);
        double d = ifj16.find(a, b);
        Main.checkEqual(i, d);
    }
    
    static void sort(String testName, String in) {
        testName = "Seřazení textu: " + testName;
        Main.p(testName);
        /**/ifj16.sort(in);
        String out = ifj16.sort(in);
        ifj16.print(in + " -> " + out + "\n");
    }
}
