class Main {
	static int a;
    static void run() {
        static void foo(String s) {}
    }
}

class bar {
    foo(a); // calling function expecting String parameter with int parameter
}