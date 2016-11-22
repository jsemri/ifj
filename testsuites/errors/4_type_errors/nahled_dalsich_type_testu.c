// ------------------
class Main {
    static void run() {
        String a;
        a = 5;	// rvalue is int
    }
}

// ------------------
class Main {
    static void run() {
        String a;
        a = 5.5;	// rvalue is double
    }
}

// ------------------
class Main {
    static void run() {
        int a;
        a = 5.5;	// rvalue is double
    }
}


// ------------------
class Main {
    static void run() {
        String a;
        a = ifj16.length("str"); // lvalue is string
    }
}

// ------------------
class Main {
    static void run() {
        double a;
        ifj16.length(a); // wrong type of parameter
    }
}

// ------------------
class Main {
    static void run() {
        ifj16.length("str1", "str2"); // more parameters than 1
    }
}

// ------------------
class Main {
    static void run(int a, double b) {
    	int a = 5;
        ifj16.compare("fdas", a); // invalid parameter type
    }
}

// ------------------
class Main {
	static String a;
    static void run(int a, double b) {
        a = ifj16.compare("fdas", "fdas"); // lvalue is string
    }
}

// ------------------
class Main {
	static int a;
    static void run() {
        a = ifj16.find("str1"); // only one parameter
    }
}

// ------------------
class Main {
	static String a;
    static void run() {
        a = ifj16.find("str1", "t"); // lvalue is string
    }
}

// ------------------
class Main {
    static double a;
    static void run(int a, double b) {
        a = ifj16.sort("fads"); /// lvalue double
    }
}

// ------------------
class Main {
    static double a;
    static void run(int a, double b) {
        a = ifj16.sort("fads", "str"); /// more parameters
    }
}

// -----------------
class Main {
    static void run() {
        sec.run("str");   // str instead of int
    }
}

class sec {
    static double run(int a) {
        return a;
    }
}

// ------------------

class Main {
    static void run() {
        sec.run("str");   // str instead of double
    }
}

class sec {
    static double run(double a) {
        return a;
    }
}